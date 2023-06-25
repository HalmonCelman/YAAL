#pragma once

#include <iostream>
#include <fstream>
#include <AL/al.h>
#include <AL/alc.h>

#define SOUND_BUFFERS (4)
#define SOUND_BUFFER_SIZE (64*1024)


class Sound{
private:

    ALuint soundBuffers[SOUND_BUFFERS];

    ALuint source;
    int totalBuffers;
    int buffersPlayed;
    ALint  distanceModel;   // default: AL_NONE - 2D
    int readCounter;
    std::ifstream in;

    ALenum checkALerrors(void);
    std::int32_t convert_to_int(char* buffer, std::size_t len);
public:
    Sound();
    ~Sound();

    int counter;
    ALint state;

    const char * filename;
    // sound parameters
    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitDepth;   // bits per sample
    ALsizei size;       // in bytes
    ALenum  format; 
    ALfloat pitch;      // speed of play, default: 1
    ALfloat gain;       // gain (how loud), default: 1 - each division by 2: -6dB
    ALint isLooped;     // false: not looped, true: looped, default: false

    ALfloat positionX;  // default: 0,0,0
    ALfloat positionY;
    ALfloat positionZ;

    ALfloat velocityX;  // default: 0,0,0
    ALfloat velocityY;
    ALfloat velocityZ;

    bool is3D;       // false: 2D, true: 3D, default: false

    //methods
    bool Open(const char * file);      // if error occurs - returns false, unless returns true
    bool CreateSource(void);           // if error occurs - returns false, unless returns true
    bool Play(void);                   // if error occurs - returns false, unless returns true
    bool Update(void);                 // if error: false, unless true
    void PrintSummary(void);
};