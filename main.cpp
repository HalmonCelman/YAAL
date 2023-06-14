#include <iostream>
#include <fstream>
#include <cstring>
#include <bit>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound.h"

void own_strcpy(char * source, char * destination, int bytes){
    for(int i=0;i<bytes;i++){
        *destination++ = *source++;
    }
}

static void list_audio_devices(const ALCchar *devices)
{
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        fprintf(stdout, "Devices list:\n");
        fprintf(stdout, "----------\n");
        while (device && *device != '\0' && next && *next != '\0') {
                fprintf(stdout, "%s\n", device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        fprintf(stdout, "----------\n");
}

void checkOpenALError(const std::string& message) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << message << ": " << alGetString(error) << std::endl;
        exit(1);
    }
}


std::int32_t convert_to_int(char* buffer, std::size_t len)
{
    std::int32_t a = 0;
    if(std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for(std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool load_wav_file_header(std::ifstream& file,
                          std::uint8_t& channels,
                          std::int32_t& sampleRate,
                          std::uint8_t& bitsPerSample,
                          ALsizei& size)
{
    char buffer[4];
    if(!file.is_open())
        return false;

    // the RIFF
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read RIFF" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
        return false;
    }

    // the size of the file
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read WAVE" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "WAVE", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
        return false;
    }

    // "fmt/0"
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if(!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data chunk header" << std::endl;
        return false;
    }
    if(std::strncmp(buffer, "data", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
        return false;
    }

    // size of data
    if(!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if(file.eof())
    {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if(file.fail())
    {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    return true;
}

char* load_wav(const std::string& filename,
               std::uint8_t& channels,
               std::int32_t& sampleRate,
               std::uint8_t& bitsPerSample,
               ALsizei& size)
{
    std::ifstream in(filename, std::ios::binary);
    if(!in.is_open())
    {
        std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
        return nullptr;
    }
    if(!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
    {
        std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
        return nullptr;
    }

    char* data = new char[size];

    in.read(data, size);

    return data;
}

int main(){
/*
ALCdevice *device;

device = alcOpenDevice(NULL);

ALCcontext *context;

context = alcCreateContext(device, NULL);
alcMakeContextCurrent(context);
checkOpenALError("Błąd przy tworzeniu kontekstu OpenAL");

ALboolean enumeration;

enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
if (enumeration == AL_FALSE)
        std::cout<< "enumeration not supported\n";
else
       std::cout<< "enumeration supported\n";

list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));


ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

alListener3f(AL_POSITION, 0, 0, 1.0f);
// check for errors
alListener3f(AL_VELOCITY, 0, 0, 0);
// check for errors
alListenerfv(AL_ORIENTATION, listenerOri);

ALuint source;


alGenSources((ALuint)1, &source);
// check for errors
checkOpenALError("Błąd przy tworzeniu zrodla");

alSourcef(source, AL_PITCH, 1);
// check for errors

alSourcef(source, AL_GAIN, 1);
// check for errors
alSource3f(source, AL_POSITION, 0, 0, 0);
// check for errors
alSource3f(source, AL_VELOCITY, 0, 0, 0);
// check for errors
alSourcei(source, AL_LOOPING, AL_TRUE);
alSourcei(source, AL_DISTANCE_MODEL, AL_INVERSE_DISTANCE_CLAMPED);
checkOpenALError("Błąd przy tworzeniu zrodla");


 // Tworzenie buforów dźwiękowych
    ALuint buffers[2];
    alGenBuffers(2, buffers);
    checkOpenALError("Błąd przy generowaniu buforów dźwiękowych");




int32_t sampleRate ;
uint8_t bitDepth ;

int numSamples;
ALsizei size;

uint8_t numChannels;

char * soundData = load_wav("getout.wav",numChannels,sampleRate,bitDepth,size);

ALenum format;
    if(numChannels == 1 && bitDepth == 8)
        format = AL_FORMAT_MONO8;
    else if(numChannels == 1 && bitDepth == 16)
        format = AL_FORMAT_MONO16;
    else if(numChannels == 2 && bitDepth == 8)
        format = AL_FORMAT_STEREO8;
    else if(numChannels == 2 && bitDepth == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << numChannels << " channels, "
            << bitDepth << " bps" << std::endl;
        return 0;
    }


// Wypełnianie buforów dźwiękowych początkowymi danymi
    alBufferData(buffers[0], format, soundData, sizeof(ALuint), 44100);
    alBufferData(buffers[1], format, soundData, sizeof(ALuint), 44100);
    checkOpenALError("Błąd przy przekazywaniu danych do buforów dźwiękowych");


// Powiązanie buforów dźwiękowych z źródłem dźwięku
    alSourceQueueBuffers(source, 2, buffers);
    checkOpenALError("Błąd przy powiązywaniu buforów z źródłem dźwięku");


alBufferData(buffer, format, soundData, size, sampleRate);
checkOpenALError("Błąd przy tworzeniu bufora zrodla");
alSourcei(source, AL_BUFFER, buffer);

alSourcePlay(source);
// check for errors

checkOpenALError("Błąd przy odtwarzaniu zrodla");
ALint source_state;
alGetSourcei(source, AL_SOURCE_STATE, &source_state);
// check for errors
while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        // check for errors
}

alDeleteSources(1, &source);
alDeleteBuffers(1, &buffer);
device = alcGetContextsDevice(context);
alcMakeContextCurrent(NULL);
alcDestroyContext(context);
alcCloseDevice(device);
*/

    Sound mob;
    mob.PrintSummary();
    mob.Open("YMCA.wav");
    mob.PrintSummary();
    return 0;
}



