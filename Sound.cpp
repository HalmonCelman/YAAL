/*
Library by KK
*/
#include "Sound.h"

#include <iostream>

#include <fstream>
#include <cstring>
#include <bit>
#include <vector>

#include <AL/al.h>
#include <AL/alc.h>

ALenum Sound::checkALerrors(void){
    ALenum error = 0;
    error=alGetError();
    if (error) {
        std::cerr << "error: " << alGetString(error) << std::endl;
    }
    return error;
}

std::int32_t Sound::convert_to_int(char* buffer, std::size_t len)
{
    std::int32_t a = 0;
    if(std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for(std::size_t i = 0; i < len; ++i)
            reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool Sound::Open(const char * file){
    filename = file;
    in.open(filename,std::ios::binary);
    
    char buffer[4];
    if(!in.is_open())
        return false;

    // the RIFF
    if(!in.read(buffer, 4))
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
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if(!in.read(buffer, 4))
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
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if(!in.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if(!in.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if(!in.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if(!in.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitDepth = convert_to_int(buffer, 2);

    // data chunk header "data"
    if(!in.read(buffer, 4))
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
    if(!in.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if(in.eof())
    {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if(in.fail())
    {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    if(channels == 1 && bitDepth == 8)
        format = AL_FORMAT_MONO8;
    else if(channels == 1 && bitDepth == 16)
        format = AL_FORMAT_MONO16;
    else if(channels == 2 && bitDepth == 8)
        format = AL_FORMAT_STEREO8;
    else if(channels == 2 && bitDepth == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << (int)channels << " channels, "
            << (int)bitDepth << " bps" << std::endl;
        return false;
    }
    

    return true;
}

bool Sound::CreateSource(void){
    alGenSources((ALuint)1, &source);
    if(checkALerrors()) return false;

    alSourcef(source, AL_PITCH, pitch);
    alSourcef(source, AL_GAIN, gain);
    alSource3f(source, AL_POSITION, positionX, positionY, positionZ);
    alSource3f(source, AL_VELOCITY, velocityX, velocityY, velocityZ);
    alSourcei(source, AL_LOOPING, false);
    alSourcei(source, AL_DISTANCE_MODEL, distanceModel);
    if(checkALerrors()) return false;

    std::cout<<"Created source"<<std::endl;
    return true;
}

bool Sound::Play(void){ //by streaming technique
    
    buffersPlayed = 0;
    totalBuffers = size/SOUND_BUFFER_SIZE + (size%SOUND_BUFFER_SIZE)?1:0;
    
    char * data;
    data=new char[SOUND_BUFFER_SIZE*SOUND_BUFFERS];

    std::memset(data,0,SOUND_BUFFER_SIZE*SOUND_BUFFERS);
    in.read(data,SOUND_BUFFER_SIZE*SOUND_BUFFERS);
    
    alGenBuffers(SOUND_BUFFERS,soundBuffers);
    //fill at start
    for(int i=0;i<SOUND_BUFFERS;i++){
        alBufferData(soundBuffers[i], format, data+i*SOUND_BUFFER_SIZE, SOUND_BUFFER_SIZE, sampleRate);
        if(checkALerrors()) return false;
    }
    readCounter=SOUND_BUFFER_SIZE*SOUND_BUFFERS;
    delete [] data;

    alSourceQueueBuffers(source, SOUND_BUFFERS, soundBuffers);
    if(checkALerrors()) return false;
    alSourcePlay(source);
    if(checkALerrors()) return false;
    state = AL_PLAYING;
    isPlaying=true;
    std::cout<<"Started playing sound: "<<filename<<std::endl;
    return true;
}

bool Sound::Update(void){
    if(!isPlaying)
        return false;

    ALint buffersProcessed = 0;
    alGetSourcei(source,AL_BUFFERS_PROCESSED,&buffersProcessed);
    if(buffersProcessed <= 0) return true;

    for(int i=0;i<buffersProcessed;i++){
        
        if(readCounter < size){
    
            char * data;
            data=new char[SOUND_BUFFER_SIZE];

            std::memset(data,0,SOUND_BUFFER_SIZE);

            in.read(data,SOUND_BUFFER_SIZE);
            readCounter+=SOUND_BUFFER_SIZE;

            alSourceUnqueueBuffers(source,1,&soundBuffers[(buffersPlayed % SOUND_BUFFERS)]);
            alBufferData(soundBuffers[(buffersPlayed % SOUND_BUFFERS)], format, data, SOUND_BUFFER_SIZE, sampleRate);
            if(checkALerrors()) return false;
            alSourceQueueBuffers(source,1,&soundBuffers[(buffersPlayed % SOUND_BUFFERS)]);
            counter++;
            
            state=AL_PLAYING;
            delete [] data;
        }
        buffersPlayed++;
    }   
    buffersProcessed=0;
    
    alGetSourcei(source,AL_SOURCE_STATE,&state);
    if(state != AL_PLAYING){
        if(buffersPlayed >= totalBuffers){
        
            ALuint buffer;
            alSourceUnqueueBuffers(source,SOUND_BUFFERS,&buffer);
            in.close();
            if(isLooped){
                Open(filename);
                Play();
                return true;
            }else{
                isPlaying=false;
                return false;
            }
        }else{
            alSourcePlay(source);
        }
    }

    return true;
}


void Sound::PrintSummary(void){
    std::cout<<"======================"<<std::endl;
    std::cout<<"filename: "<<((*filename)?filename:"brak pliku")<<std::endl;
    std::cout<<"distance Model: ";
    switch(distanceModel){
        case AL_NONE:
            std::cout<<"AL_NONE";
            break;
        case AL_INVERSE_DISTANCE:
            std::cout<<"AL_INVERSE_DISTANCE";
            break;
        case AL_INVERSE_DISTANCE_CLAMPED:
            std::cout<<"AL_INVERSE_DISTANCE_CLAMPED";
            break;
        case AL_LINEAR_DISTANCE:
            std::cout<<"AL_LINEAR_DISTANCE";
            break;
        case AL_LINEAR_DISTANCE_CLAMPED:
            std::cout<<"AL_LINEAR_DISTANCE_CLAMPED";
            break;
        case AL_EXPONENT_DISTANCE:
            std::cout<<"AL_EXPONENT_DISTANCE";
            break;
        case AL_EXPONENT_DISTANCE_CLAMPED:
            std::cout<<"AL_EXPONENT_DISTANCE_CLAMPED";
            break;
        default:
            std::cout<<"UNKNOWN!!!";
            break;
    }
    std::cout<<std::endl<<"channels: "<<(int)channels<<std::endl;
    std::cout<<"sampleRate: "<<sampleRate<<std::endl;
    std::cout<<"bitDepth: "<<(int)bitDepth<<std::endl;
    std::cout<<"size: "<<size<<std::endl;
    std::cout<<"format: ";
    switch(format){
        case AL_FORMAT_MONO8:
            std::cout<<"AL_FORMAT_MONO8";
            break;
        case AL_FORMAT_MONO16:
            std::cout<<"AL_FORMAT_MONO16";
            break;
        case AL_FORMAT_STEREO8:
            std::cout<<"AL_FORMAT_STEREO8";
            break;
        case AL_FORMAT_STEREO16:
            std::cout<<"AL_FORMAT_STEREO16";
            break;
        default:
            std::cout << "unrecognised wave format: "
            << (int)channels << " channels, "
            << (int)bitDepth << " bps";
    }
    std::cout<<std::endl;
    std::cout<<"pitch: "<<pitch<<std::endl;
    std::cout<<"gain: "<<gain<<std::endl;
    std::cout<<"isLooped: "<<((isLooped)?"looped":"not looped")<<std::endl;
    std::cout<<std::endl;
    std::cout<<"position - x: "<<positionX<<" y: "<<positionY<<" z: "<<positionZ<<std::endl;
    std::cout<<std::endl;
    std::cout<<"velocity - x: "<<velocityX<<" y: "<<velocityY<<" z: "<<velocityZ<<std::endl;
    std::cout<<std::endl<<"tribe: "<<((is3D)?"3D":"2D")<<std::endl;
    std::cout<<"======================"<<std::endl;
}



Sound::Sound()
:   counter         ( 0 ),
    readCounter     ( 0 ),
    state           ( AL_NONE ),
    distanceModel   ( AL_NONE ),
    
    channels        ( 0 ),
    sampleRate      ( 0 ),
    bitDepth        ( 0 ),
    size            ( 0 ),
    format          ( AL_NONE ),
    pitch           ( 1 ),
    gain            ( 1 ),
    isLooped        ( false ),

    positionX       ( 0 ),
    positionY       ( 0 ),
    positionZ       ( 0 ),

    velocityX       ( 0 ),
    velocityY       ( 0 ),
    velocityZ       ( 0 ),

    is3D            ( false )
{
    std::cout << "Created new sound object" << std::endl; 
}

Sound::~Sound(){
    alDeleteBuffers(SOUND_BUFFERS, soundBuffers);
    alDeleteSources(1, &source);
}

// standalone functions
static ALCdevice *device;
static ALCcontext *context;


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

void Sound_CreateListener(void){
    device = alcOpenDevice(NULL);
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
}

void Sound_DeleteListener(void){
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}