# ofxAudioFile (fred-dev fork, `ai-refactor` branch)

## Changes in this fork

The public API (`load`, `free`, `loaded`, `data`, `sample`, `samplerate`,
`channels`, `length`, `path`, `setVerbose`, copy and assignment) is unchanged.
Projects that use the addon need no code changes.

| Change | Why | Platforms |
|---|---|---|
| `#include <cstdint>` / `<cstddef>` in the header | does not compile on openFrameworks main or any current compiler without it | all |
| Buffers freed with the matching function (was `delete` on `new[]` memory) | undefined behaviour, heap corruption when reloading files | all |
| Copy constructor no longer calls `free()` on uninitialised members; self assignment is safe; `length()` is copied | crash or garbage when copying a loaded file | all |
| `load_ogg` frees the previous buffer and frees stb_vorbis output with `free()` (was `delete`) | memory leak on every reload, undefined behaviour | all |
| `length()` initialised to 0 and reset by `free()` | returned garbage before the first load | all |
| Decoded data is used directly instead of being copied into a second buffer | halves peak memory and removes a full copy per load (wav, flac, mp3); matters on a Raspberry Pi | all |
| Ogg length uses frames x channels on every platform | the old `#ifndef __ANDROID__` meant Android only read part of each file | all (fixes Android) |
| Files without an extension are rejected cleanly | `find_last_of` returned npos | all |
| Decoders updated: dr_wav 0.10.1 -> 0.14.6, dr_flac 0.12.1 -> 0.13.4, dr_mp3 0.4.7 -> 0.7.4, stb_vorbis -> 1.22 | years of security and correctness fixes; UTF-8 paths on Windows | all |
| `addon_config.mk` lists every platform (incl. `linuxaarch64`, `osx`) and excludes `tests/` | explicit, no flags needed anywhere | all |
| `update_libs.sh` works whatever the folder is called | it refused to run in a clone named differently | all |
| `tests/`: stand-alone tests for wav/flac/ogg/mp3, copy, assign and repeated reloads, built with AddressSanitizer (`make -C tests`, needs ffmpeg to make the test files) | regression safety | n/a |

None of these is specific to one project, so they are suitable for the
`master` branch of this fork and as a pull request upstream.

---

ofxAudioFile
=====================================
[![Build status](https://travis-ci.org/npisanti/ofxAudioFile.svg?branch=master)](https://travis-ci.org/npisanti/ofxAudioFile) [![Build status](https://ci.appveyor.com/api/projects/status/e5pnayuh8g8vb04r?svg=true)](https://ci.appveyor.com/project/npisanti/ofxaudiofile)


cross platform audio file loading addon for openFrameworks, wrapping [dr_wav](https://github.com/mackron/dr_libs/blob/master/dr_wav.h), [dr_mp3](https://github.com/mackron/dr_libs/blob/master/dr_mp3.h), [dr_flac](https://github.com/mackron/dr_libs/blob/master/dr_flac.h) and [stb_vorbis](https://github.com/nothings/stb/blob/master/stb_vorbis.c), so you can read Wav, mp3, FLAC and Ogg Vorbis. Works on Linux (desktop and raspberry pi), Windows, OSX, Android and iOS. This addon does not play back samples, it just let you access the float values of the audio file waveform. For sample playing you could use the oF included classes or for more advanced sample playback you could try [ofxPDSP](https://github.com/npisanti/ofxPDSP).   
   
As just standard c++ headers are needed for compiling this addon, it should be also possible to use it without openFrameworks.   
   
Example app loads a mp3 file, or you can drag and drop files in the window if your platform let you do that.   
   
License 
------------   
As all those libraries are public domain, also this addon have an [unlicense](http://unlicense.org/).   
