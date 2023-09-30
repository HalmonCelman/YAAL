#include <iostream>
#include <fstream>
#include <cstring>
#include <bit>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound.h"


int main(){
    Sound_CreateListener();

    Sound mob;
    mob.Open("UT.wav");
    mob.CreateSource();
    mob.Play();
    while(mob.Update());
        
    Sound_DeleteListener();
    
    return 0;
}



