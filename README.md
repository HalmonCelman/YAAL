# YAAL - Yet Another Audio Library
it's basicly wrapper for OpenAL - to make things easier

## Features:
- multiple sound sources
- 3D calculation with diffrent sound models
- setting position, gain for sources and listener
- streaming files

## How should I use it?
- just install OpenAL as it's on their github
- add this repo as a submodule and subdirectory
- All sounds you want to use in 3D mode should be mono

## Basic setup
1. create Listener: 
2. create new sound object 
3. open file with music - for now should be *.wav, to use in 3D should be mono channel
4. create source
5. start playing it
6. update as long as Update function is returning true
7. delete listener at end

```
Sound_CreateListener();

Sound <object_name>;

<object_name>.Open("<filename>");

<object_name>.CreateSource();

<object_name>.Play();

while(<object_name>.Update());

Sound_DeleteListener();
``` 

More examples in future, now you can look into Example folder

