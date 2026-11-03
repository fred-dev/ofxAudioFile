#!/bin/sh
# Generates the test files: 0.5 s, 441 Hz sine, amplitude 0.5, stereo, 44.1 kHz.
set -e
cd "$(dirname "$0")"
mkdir -p data
for ext in wav flac ogg mp3; do
  ffmpeg -loglevel error -y -f lavfi \
    -i "aevalsrc=0.5*sin(2*PI*441*t)|0.5*sin(2*PI*441*t):s=44100:d=0.5" "data/tone.$ext"
done
