#pragma once

#include <iostream>
#include <cstdint>
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
    char * data;

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
    bool isLooped;     // false: not looped, true: looped, default: false
    bool isPlaying;

    ALfloat positionX;  // default: 0,0,0
    ALfloat positionY;
    ALfloat positionZ;

    ALfloat velocityX;  // default: 0,0,0
    ALfloat velocityY;
    ALfloat velocityZ;

    //methods
    bool Open(const char * file);      // if error occurs - returns false, unless returns true
    
    bool CreateSource(void);           // if error occurs - returns false, unless returns true
    bool Play(void);                   // if error occurs - returns false, unless returns true
    bool Update(void);                 // if error: false, unless true
    void PrintSummary(void);
    
    void setPosition(ALfloat x,ALfloat y, ALfloat z);
    void setVelocity(ALfloat v_x,ALfloat v_y, ALfloat v_z);
    void setPitch(ALfloat pitchToSet);
    void setGain(ALfloat gainToSet);
    void setDistanceModel(ALenum model);
    void setReferenceDistance(float distance);
    void setMaxDistance(float distance);
    void setRollOffFactor(float factor);
    void setMinGain(float minGain);
    void setMaxGain(float maxGain);
};

void Sound_CreateListener(void);         //connects to deviace, creates context, creates listener - default position:0,0,0
void Sound_DeleteListener(void);
//these 3 functions below should be used AFTER Sound_CreateListener() - this function sets all these values to default
void Sound_SetListenerGain(ALfloat gain); 
void Sound_SetListenerPosition(ALfloat x, ALfloat y, ALfloat z); 
void Sound_SetListenerVelocity(ALfloat v_x, ALfloat v_y, ALfloat v_z);
void Sound_SetListenerOrientation(ALfloat * orientation);