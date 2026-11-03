CXX ?= g++
CXXFLAGS ?= -std=c++17 -O1 -g -Wall -fsanitize=address,undefined -fno-omit-frame-pointer

run: test_runner data/tone.wav
	./test_runner

data/tone.wav:
	./make_test_files.sh

test_runner: test_main.cpp ../src/ofxAudioFile.cpp ../src/ofxAudioFile.h
	$(CXX) $(CXXFLAGS) test_main.cpp ../src/ofxAudioFile.cpp -o $@

clean:
	rm -rf test_runner data

.PHONY: run clean
