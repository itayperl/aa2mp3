#!/bin/bash

set -e

QUALITY=96

fail () {
    for e in "$@"; do echo "(E) $e" >&2; done
    exit 1
}

usage () {
    cat <<EOF
Usage: $0 [-q quality-in-kbps] in.aa out.mp3
Prerequisites: 
 - ffmpeg or avconv
 - Being logged in in the Audible app, either by 'Activate'
   or via the 'Podcast' section.
EOF
    exit 0
}

while getopts "hq:" opt; do
    case $opt in
        "h") usage
             ;;
        "q") expr "$OPTARG" : '[0-9]\+' > /dev/null || \
             fail "Invalid number $OPTARG";
             QUALITY=$OPTARG
             ;;
        "?") exit 1
             ;;
    esac
done


AVCONV_ARGS="-ab ${QUALITY}k"

shift $((OPTIND - 1))

[ $# -eq 2 ] || usage
[ -f "$1" ] || fail "Input file \"$1\" does not exist."
[ -f "$2" ] && fail "Output file \"$2\" exists."
[ -f "dump.exe" ] || fail "dump.exe not found at the root."

FFMPEG=$(which ffmpeg 2> /dev/null || which avconv 2> /dev/null) || \
    fail "Neither ffmpeg(1) nor avconv(1) were found." \
    "Please install one of them---both are not needed."

echo -n "Identifying the input file...  "
PARAMS=$(wine dump.exe -i "$1") || \
    fail "Identification of \"$1\" failed.  Are you logged in in the Audible app," \
    "either by 'Activate' or via the 'Podcast' section?"

echo "Launching conversion (wine dump.exe $1 | $FFMPEG $PARAMS -i - $AVCONV_ARGS $2)"
wine dump.exe "$1" | $FFMPEG $PARAMS -i - $AVCONV_ARGS "$2"
