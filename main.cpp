#include <iostream>
#include <fstream>
#include <cstring>
#include <bit>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound.h"


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


int main(){

ALCdevice *device;

device = alcOpenDevice(NULL);

ALCcontext *context;

context = alcCreateContext(device, NULL);
alcMakeContextCurrent(context);


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

/*


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
    Sound mob,mob2;
    mob.Open("YMCA.wav");
    mob.PrintSummary();
    mob.isLooped = false;
    if(!mob.CreateSource()){
        std::cout<<"from source creation";
    }else{
        if(!mob.Play()){
            std::cout<<"from play function";
        }else{
            while(mob.Update()){
                
            }
        }
    }  

    mob.Open("getout.wav");
    mob.PrintSummary();
    
        if(!mob.Play()){
            std::cout<<"from play function";
        }else{
            while(mob.Update()){
                
            }
        }
      
    
    
    return 0;
}



