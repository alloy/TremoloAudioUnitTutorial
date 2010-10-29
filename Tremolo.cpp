/*=============================================================================
  Tremolo.cpp
  
=============================================================================*/
#include "Tremolo.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

COMPONENT_ENTRY(Tremolo)


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::Tremolo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tremolo::Tremolo(AudioUnit component) : AUEffectBase(component) {
  CreateElements();
  Globals()->UseIndexedParameters(kNumberOfParameters);

  SetParameter(kParameter_Frequency, kDefaultValue_Tremolo_Freq);
  SetParameter(kParameter_Depth, kDefaultValue_Tremolo_Depth);
  SetParameter(kParameter_Waveform, kDefaultValue_Tremolo_Waveform);

#if AU_DEBUG_DISPATCHER
  mDebugDispatcher = new AUDebugDispatcher(this);
#endif
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetParameterValueStrings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus Tremolo::GetParameterValueStrings(AudioUnitScope inScope, AudioUnitParameterID inParameterID, CFArrayRef *outStrings) {
  return kAudioUnitErr_InvalidProperty;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetParameterInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus      Tremolo::GetParameterInfo(AudioUnitScope    inScope,
                                                        AudioUnitParameterID  inParameterID,
                                                        AudioUnitParameterInfo  &outParameterInfo )
{
  OSStatus result = noErr;

  outParameterInfo.flags =   kAudioUnitParameterFlag_IsWritable
            |    kAudioUnitParameterFlag_IsReadable;
    
    if (inScope == kAudioUnitScope_Global) {
        switch(inParameterID)
        {
            case kParam_One:
                AUBase::FillInParameterName (outParameterInfo, kParameterOneName, false);
                outParameterInfo.unit = kAudioUnitParameterUnit_LinearGain;
                outParameterInfo.minValue = 0.0;
                outParameterInfo.maxValue = 1;
                outParameterInfo.defaultValue = kDefaultValue_ParamOne;
                break;
            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
            }
  } else {
        result = kAudioUnitErr_InvalidParameter;
    }
    


  return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetPropertyInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus      Tremolo::GetPropertyInfo (AudioUnitPropertyID  inID,
                                                        AudioUnitScope    inScope,
                                                        AudioUnitElement  inElement,
                                                        UInt32 &    outDataSize,
                                                        Boolean &    outWritable)
{
  return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetProperty
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus      Tremolo::GetProperty(  AudioUnitPropertyID inID,
                                                        AudioUnitScope     inScope,
                                                        AudioUnitElement   inElement,
                                                        void *      outData )
{
  return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}


#pragma mark ____TremoloEffectKernel


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::TremoloKernel::Reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    Tremolo::TremoloKernel::Reset()
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::TremoloKernel::Process
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    Tremolo::TremoloKernel::Process(  const Float32   *inSourceP,
                                                    Float32       *inDestP,
                                                    UInt32       inFramesToProcess,
                                                    UInt32      inNumChannels, // for version 2 AudioUnits inNumChannels is always 1
                                                    bool      &ioSilence )
{

  //This code will pass-thru the audio data.
  //This is where you want to process data to produce an effect.

  
  UInt32 nSampleFrames = inFramesToProcess;
  const Float32 *sourceP = inSourceP;
  Float32 *destP = inDestP;
  Float32 gain = GetParameter( kParam_One );
  
  while (nSampleFrames-- > 0) {
    Float32 inputSample = *sourceP;
    
    //The current (version 2) AudioUnit specification *requires* 
      //non-interleaved format for all inputs and outputs. Therefore inNumChannels is always 1
    
    sourceP += inNumChannels;  // advance to next frame (e.g. if stereo, we're advancing 2 samples);
                  // we're only processing one of an arbitrary number of interleaved channels

      // here's where you do your DSP work
                Float32 outputSample = inputSample * gain;
    
    *destP = outputSample;
    destP += inNumChannels;
  }
}

