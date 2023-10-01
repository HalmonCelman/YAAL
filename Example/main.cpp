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
    //listener setup
    Sound_CreateListener();

    Sound_SetListenerPosition(0,0,0);
    ALfloat orientation[6]={0,0,-1.0f,0,1.0f,0};
    Sound_SetListenerOrientation(orientation);
    Sound_SetListenerGain(0.7f);

    //source setup
    Sound mob;
    mob.Open("UT.wav");
    mob.CreateSource();
    mob.setPosition(0.0f,0.0f,0.0f);
    mob.setDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    mob.setMaxGain(1.0f);
    mob.setMinGain(0.0f);
    mob.setReferenceDistance(1.0f);
    mob.setRollOffFactor(1.0f);
    mob.setMaxDistance(25.0f);
   
    mob.Play();
    
    float x=0.0f;
    while(mob.Update()){
        mob.PrintSummary();
        x-=0.1f;
        mob.setPosition(x,0.0f,0.0f);
        usleep(100000);
    }
        
    Sound_DeleteListener();
    
    return 0;
}



