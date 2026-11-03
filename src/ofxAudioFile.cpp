
#include "ofxAudioFile.h"

#define DR_WAV_IMPLEMENTATION
#include "../libs/dr_wav.h"

#define DR_MP3_IMPLEMENTATION
#include "../libs/dr_mp3.h"

#define DR_FLAC_IMPLEMENTATION
#include "../libs/dr_flac.h"

#include "../libs/stb_vorbis.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <new>

// ------------------------- buffer release functions --------------------------
// Each decoder allocates its output differently, so the matching release
// function is stored next to the buffer.
namespace {
    void releaseNew( float* p )  { delete [] p; }
    void releaseWav( float* p )  { drwav_free( p, nullptr ); }
    void releaseMp3( float* p )  { drmp3_free( p, nullptr ); }
    void releaseFlac( float* p ) { drflac_free( p, nullptr ); }
}

ofxAudioFile::ofxAudioFile()  {
    buffer = nullptr;
    releaseBuffer = nullptr;
    verbose = false;
    free();
}

ofxAudioFile::~ofxAudioFile(){
    free();
}

ofxAudioFile::ofxAudioFile(const ofxAudioFile & other) : ofxAudioFile() {
    copyFrom( other );
}

ofxAudioFile & ofxAudioFile::operator= (const ofxAudioFile & other) {
    if( this != &other ){
        copyFrom( other );
    }
    return *this;
}

void ofxAudioFile::copyFrom( const ofxAudioFile & other ){
    free();
    this->verbose = other.verbose;
    if( other.loaded() ){
        float* copy = new (std::nothrow) float[other.buffersize];
        if( copy == nullptr ){
            std::cout<<"[ofxAudioFile] ERROR : out of memory copying "<<other.filePath<<"\n";
            return;
        }
        std::memcpy( copy, other.buffer, other.buffersize * sizeof(float) );
        adopt( copy, releaseNew, other.nchannels, other.sampleRate, other.slength, other.filePath );
    }
}

void ofxAudioFile::setVerbose(bool verbose){
    this->verbose = verbose;
}

void ofxAudioFile::free(){
    if( buffer != nullptr && releaseBuffer != nullptr ){
        releaseBuffer( buffer );
    }
    filePath = "file not loaded";
    buffer = nullptr;
    releaseBuffer = nullptr;
    nchannels = 0;
    buffersize = 0;
    slength = 0;
    sampleRate = 11050;
}

bool ofxAudioFile::loaded() const{
    return buffer != nullptr;
}

void ofxAudioFile::adopt( float* data, void (*release)(float*), unsigned int channels,
                          unsigned int rate, uint64_t frames, const std::string& path ){
    free();
    if( data == nullptr || channels == 0 ){
        if( data != nullptr ) release( data );
        return;
    }
    this->buffer = data;
    this->releaseBuffer = release;
    this->nchannels = channels;
    this->sampleRate = rate;
    this->slength = frames;
    this->buffersize = frames * channels;
    this->filePath = path;
    if(verbose) std::cout<<"[ofxAudioFile] loading "<<this->filePath<<" | sample rate : "<< this->sampleRate <<" | channels : "<<this->nchannels<<" | length : "<<this->slength<<"\n";
}


// ------------------------- AUDIO LOADING ROUTINES ---------------------------
void ofxAudioFile::load( std::string path ){
    size_t dot = path.find_last_of(".");
    std::string extension = ( dot == std::string::npos ) ? "" : path.substr( dot + 1 );
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    if( extension == "wav" ){
        load_wav( path );
    }else if ( extension == "flac" ){
        load_flac( path );
    }else if ( extension == "mp3" ){
        load_mp3( path );
    }else if ( extension == "ogg" ){
        load_ogg( path );
    }else{
        std::cout<<"[ofxAudioFile] ERROR : "<<extension<<" is an unsupported file extension\n";
    }

}

void ofxAudioFile::load_wav( std::string path ){
    unsigned int channels = 0;
    unsigned int rate = 0;
    drwav_uint64 frames = 0;
    float* data = drwav_open_file_and_read_pcm_frames_f32( path.c_str(), &channels, &rate, &frames, nullptr );
    if ( data == nullptr ) {
        std::cout<<"[ofxAudioFile] ERROR loading wav file "<<path<<"\n";
        return;
    }
    adopt( data, releaseWav, channels, rate, frames, path );
}

void ofxAudioFile::load_mp3( std::string path ){
    drmp3_config config {};
    drmp3_uint64 frames = 0;
    float* data = drmp3_open_file_and_read_pcm_frames_f32( path.c_str(), &config, &frames, nullptr );
    if ( data == nullptr ) {
        std::cout<<"[ofxAudioFile] ERROR loading mp3 file "<<path<<"\n";
        return;
    }
    adopt( data, releaseMp3, config.channels, config.sampleRate, frames, path );
}

void ofxAudioFile::load_flac( std::string path ){
    unsigned int channels = 0;
    unsigned int rate = 0;
    drflac_uint64 frames = 0;
    float* data = drflac_open_file_and_read_pcm_frames_f32( path.c_str(), &channels, &rate, &frames, nullptr );
    if ( data == nullptr ) {
        std::cout<<"[ofxAudioFile] ERROR loading FLAC file "<<path<<"\n";
        return;
    }
    adopt( data, releaseFlac, channels, rate, frames, path );
}

void ofxAudioFile::load_ogg( std::string path ){
    short int* decoded = nullptr;
    int channels = 0;
    int rate = 0;
    // Returns the number of frames per channel, or -1 on error.
    int frames = stb_vorbis_decode_filename( path.c_str(), &channels, &rate, &decoded );
    if( frames <= 0 || channels <= 0 || decoded == nullptr ){
        std::cout<<"[ofxAudioFile] ERROR loading ogg/vorbis file "<<path<<"\n";
        std::free( decoded );
        return;
    }
    size_t samples = static_cast<size_t>( frames ) * static_cast<size_t>( channels );
    float* data = new (std::nothrow) float[samples];
    if( data == nullptr ){
        std::cout<<"[ofxAudioFile] ERROR : out of memory loading "<<path<<"\n";
        std::free( decoded );
        return;
    }
    for( size_t n=0; n<samples; ++n ){
        data[n] = static_cast<float>( decoded[n] ) / 32768.0f;
    }
    std::free( decoded ); // stb_vorbis allocates with malloc
    adopt( data, releaseNew, static_cast<unsigned int>( channels ), static_cast<unsigned int>( rate ), static_cast<uint64_t>( frames ), path );
}
