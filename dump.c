#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct {
     unsigned char unknown[716];
} AAX_instance_t;

typedef struct
{
    uint32_t type;
    unsigned char unknown[20];
} AAX_frame_info_t;

__declspec(dllimport) int AAXOpenFileWinA(AAX_instance_t **instance,
                                          const char *filename);

__declspec(dllimport) int AAXGetFileType(AAX_instance_t *instance,
                                         int *type);
__declspec(dllimport) int AAXGetDRMType(AAX_instance_t *instance,
                                         int *type);
__declspec(dllimport) int AAXGetAudioChannelCount(AAX_instance_t *instance,
                                                  int *out);
__declspec(dllimport) int AAXGetSampleRate(AAX_instance_t *instance,
                                           int *out);
__declspec(dllimport) int AAXAuthenticateWin(AAX_instance_t *instance);
__declspec(dllimport) int AAXSeek(AAX_instance_t *instance, int seek);
__declspec(dllimport) int AAXGetNextFrameInfo(AAX_instance_t *instance,
                                              AAX_frame_info_t *info);
__declspec(dllimport) int AAXGetEncodedAudio(AAX_instance_t *instance,
                                             void * buffer,
                                             size_t buffer_size,
                                             int *out_size);
__declspec(dllimport) int AAXDecodePCMFrame(AAX_instance_t *instance,
                                            void * buffer,
                                            size_t buffer_size,
                                            void * out_buffer,
                                            int bytes_left_in_frame,
                                            int *out_processed);

#ifndef DEBUG
#define DEBUG 0
#endif

#define X(x) do { int rv = x; if (DEBUG) fprintf(stderr, "%s = %d\n", #x, rv); if (rv) abort(); } while(0)

int main(int argc, const char *argv[])
{
    AAX_instance_t *aax;
    AAX_frame_info_t frame_info;
    const char *filename;
    int rv;
    int identify = 0;

    if (argc == 3 && !strcmp("-i", argv[1])) {
        filename = argv[2];
        identify = 1;
    } else if (argc == 2) {
        filename = argv[1];
    } else {
        fprintf(stderr, "Usage: %s [-i] audiobook.aa\n", argv[0]);
        return 1;
    }

    X(AAXOpenFileWinA(&aax, filename));

    int filetype, drmtype;
    X(AAXGetFileType(aax, &filetype));
    X(AAXGetDRMType(aax, &drmtype));

    X(AAXAuthenticateWin(aax));

    int sample_rate, channel_cnt;
    X(AAXGetAudioChannelCount(aax, &channel_cnt));
    X(AAXGetSampleRate(aax, &sample_rate));
    fprintf(stderr, "filetype: %d, drmtype: %d sample rate: %d, channels: %d\n",
           filetype, drmtype, sample_rate, channel_cnt);

    if (identify)
    {
        /* print libav/ffmpeg arguments */
        printf("-f s16le -ac %d -ar %d", channel_cnt, sample_rate);
        return 0;
    }

    /* set stdout to binary */
    setmode(fileno(stdout), O_BINARY);

    size_t frame_size = sample_rate * channel_cnt;
    
    int read = 0;

    unsigned char *wavebuffer = malloc(frame_size);
    if (!wavebuffer) abort();
    
    AAXSeek(aax, 0);

    /* While there are more frames to read */
    while (-24 != (rv = AAXGetNextFrameInfo(aax, &frame_info)))
    {
        if (rv) {
            fprintf(stderr, "AAXGetNextFrameInfo: %d\n", rv);
            abort();
        }

        /* Can probably accept "ace3" and "ace2" as well */
        if (frame_info.type == *(const uint32_t*)"mp3 ")
        {
            fprintf(stderr, "Skipping unhandled frame type: %4s\n", (char*)&frame_info.type);
            continue;
        }

        unsigned char buffer[0x400];
        int outsize = 0;
        X(AAXGetEncodedAudio(aax, buffer, sizeof(buffer), &outsize));


        int processed = 0;
        X(AAXDecodePCMFrame(aax, buffer, outsize,
                            wavebuffer + read,
                            frame_size - read,
                            &processed));

        read += processed;
        if ( frame_size - read < 4096 )
        {
            fwrite( wavebuffer, read, 1, stdout );
            read = 0;
        }
    }

    free(wavebuffer);

    return 0;
}
