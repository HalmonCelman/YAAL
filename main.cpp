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

    Sound mob2,mob;
    mob.Open("protector.wav");
    mob2.Open("getout.wav");
    mob.CreateSource();
    mob2.CreateSource();
    mob.Play();
    mob2.Play();
    bool res1,res2;
    do
    {
        res1=mob.Update();
        res2=mob2.Update();
    } while (res1 || res2);
        
    Sound_DeleteListener();
    
    return 0;
}



