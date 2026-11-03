# ofxAudioFile addon configuration.
#
# Header-only decoders (dr_wav, dr_flac, dr_mp3, stb_vorbis) compiled into
# src/ofxAudioFile.cpp. No system libraries and no compiler flags are needed
# on any platform.

meta:
	ADDON_NAME = ofxAudioFile
	ADDON_DESCRIPTION = loads data from Wav, FLAC, mp3 and ogg
	ADDON_AUTHOR = @npisanti
	ADDON_TAGS = "addon" "audio" "file" "decoder" "mp3" "FLAC" "ogg" "WAV" "sound"
	ADDON_URL = http://github.com/npisanti/ofxAudioFile

common:
	# tests/ has its own Makefile and is never part of an app
	ADDON_SOURCES_EXCLUDE = tests/%
	ADDON_INCLUDES_EXCLUDE = tests/%

linux64:

linux:

linuxaarch64:

linuxarmv6l:

linuxarmv7l:

msys2:

vs:

osx:

ios:

android/armeabi:

android/armeabi-v7a:
