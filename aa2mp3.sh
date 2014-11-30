#!/bin/bash

set -e

AVCONV_ARGS="-ab 96k"

if [ -z "$2" ]; then
    echo Usage: $0 in.aa out.mp3
    exit
fi

if [ -f "$2" ]; then
    echo "Output file exists."
    exit 1
fi

PARAMS=$(wine dump.exe -i $1)
if [ $? -eq 0 ]; then
    wine dump.exe $1 | avconv $PARAMS -i - $AVCONV_ARGS $2
fi
