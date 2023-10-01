#include <iostream>
#include <fstream>
#include <cstring>
#include <bit>
#include <vector>
#include <ctime>

#include <AL/al.h>
#include <AL/alc.h>

#include <Sound.h>
#include <unistd.h>


int main(){
    Sound_CreateListener();

    Sound mob;
    mob.Open("UT.wav");
    mob.CreateSource();
    mob.setPosition(0.0f,0.0f,0.0f);
    
    Sound_SetListenerPosition(0,0,0);
    ALfloat orientation[6]={0,0,-1.0f,0,1.0f,0};
    Sound_SetListenerOrientation(orientation);
    float x=0.0f;
    mob.setDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    mob.setMaxGain(1.0f);
    mob.setMinGain(0.0f);
    alListenerf(AL_GAIN,0.9f);
    mob.setReferenceDistance(1.0f);
    mob.setRollOffFactor(1.0f);
    mob.setMaxDistance(25.0f);
   
    mob.Play();
    
    
    while(mob.Update()){
        mob.PrintSummary();
        x-=0.1f;
        mob.setPosition(x,0.0f,0.0f);
        usleep(100000);
    }
        
    Sound_DeleteListener();
    
    return 0;
}



