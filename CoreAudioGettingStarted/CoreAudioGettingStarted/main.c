//
//  main.c
//  CoreAudioGettingStarted
//
//  Created by Marcus Ficner on 27.02.21.
//

#include <stdio.h>
#include <CoreAudio/CoreAudio.h>

OSStatus audioIOProc(AudioObjectID         inDevice,
                   const AudioTimeStamp*   inNow,
                   const AudioBufferList*  inInputData,
                   const AudioTimeStamp*   inInputTime,
                   AudioBufferList*        outOutputData,
                   const AudioTimeStamp*   inOutputTime,
                   void* __nullable        inClientData) {
    
    // We have interleaved audio, so there is only one buffer in the outOutputData AudioBufferList
    AudioBuffer buffer = outOutputData->mBuffers[0];
        
    static float gain = 0.02;
    for (int frame = 0; frame < buffer.mDataByteSize / (sizeof(float)); frame += buffer.mNumberChannels) {
        float y = (((float)rand()) / RAND_MAX) * 2.0f - 1.0f; // Random value between -1.0 and 1.0
        for (int channel = 0; channel < buffer.mNumberChannels; channel++) {
            ((float *)buffer.mData)[frame+channel] = gain * y;
        }
    }

    return noErr;
}

void printCoreAudioErrorAndExit(OSStatus error) {
    if (error != noErr) {
        printf("Error: %d", error);
        exit(-1);
    }
}

int main(int argc, const char * argv[]) {
    
    // initialize random seed
    srand((unsigned int)time(0));
            
    AudioObjectPropertyAddress property = (AudioObjectPropertyAddress) {
        .mSelector = kAudioHardwarePropertyDefaultOutputDevice,
        .mElement = kAudioObjectPropertyElementMaster,
        .mScope = kAudioObjectPropertyScopeGlobal
    };
            
    OSStatus err = noErr;
    
    UInt32 propertySize;
    err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &property, 0, NULL, &propertySize);
    printCoreAudioErrorAndExit(err);
    
    AudioObjectID defaultOutputDevice;
    err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &property, 0, NULL, &propertySize, &defaultOutputDevice);
    printCoreAudioErrorAndExit(err);
    
    AudioDeviceIOProcID procID;
    err = AudioDeviceCreateIOProcID(defaultOutputDevice, audioIOProc, NULL, &procID);
    printCoreAudioErrorAndExit(err);
    
    err = AudioDeviceStart(defaultOutputDevice, procID);
    printCoreAudioErrorAndExit(err);
    
    printf("Press ENTER to stop.\n");
    getchar();
    
    err = AudioDeviceStop(defaultOutputDevice, NULL);
    printCoreAudioErrorAndExit(err);
    
    err = AudioDeviceDestroyIOProcID(defaultOutputDevice, procID);
    printCoreAudioErrorAndExit(err);
    
    return 0;
}
