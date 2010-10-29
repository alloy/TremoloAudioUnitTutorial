/*=============================================================================
  Tremolo.cpp
  
=============================================================================*/
#include "Tremolo.h"
#include <math.h>

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

  SetAFactoryPresetAsCurrent(kPresets[kPreset_Default]);

#if AU_DEBUG_DISPATCHER
  mDebugDispatcher = new AUDebugDispatcher(this);
#endif
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetParameterValueStrings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult Tremolo::GetParameterValueStrings(AudioUnitScope inScope, AudioUnitParameterID inParameterID, CFArrayRef *outStrings) {
  if ((inScope == kAudioUnitScope_Global) && (inParameterID == kParameter_Waveform)) {
    if (outStrings == NULL) {
      return noErr;
    }

    CFStringRef strings[] = { kMenuItem_Tremolo_Sine, kMenuItem_Tremolo_Square };

    *outStrings = CFArrayCreate(NULL, (const void **)strings, (sizeof(strings) / sizeof(strings[0])), NULL);

    return noErr;
  }
  return kAudioUnitErr_InvalidParameter;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::GetParameterInfo
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ComponentResult Tremolo::GetParameterInfo(AudioUnitScope inScope, AudioUnitParameterID inParameterID, AudioUnitParameterInfo &outParameterInfo) {
  ComponentResult result = noErr;

  outParameterInfo.flags = kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;

  if (inScope == kAudioUnitScope_Global) {
    switch(inParameterID) {
      case kParameter_Frequency:
          AUBase::FillInParameterName(outParameterInfo, kParamName_Tremolo_Freq, false);
          outParameterInfo.unit = kAudioUnitParameterUnit_Hertz;
          outParameterInfo.minValue = kMinimumValue_Tremolo_Freq;
          outParameterInfo.maxValue = kMaximumValue_Tremolo_Freq;
          outParameterInfo.defaultValue = kDefaultValue_Tremolo_Freq;
          outParameterInfo.flags |= kAudioUnitParameterFlag_DisplayLogarithmic;
          break;

      case kParameter_Depth:
          AUBase::FillInParameterName(outParameterInfo, kParamName_Tremolo_Depth, false);
          outParameterInfo.unit = kAudioUnitParameterUnit_Percent;
          outParameterInfo.minValue = kMinimumValue_Tremolo_Depth;
          outParameterInfo.maxValue = kMaximumValue_Tremolo_Depth;
          outParameterInfo.defaultValue = kDefaultValue_Tremolo_Depth;

      case kParameter_Waveform:
          AUBase::FillInParameterName(outParameterInfo, kParamName_Tremolo_Waveform, false);
          outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
          outParameterInfo.minValue = kSineWave_Tremolo_Waveform;
          outParameterInfo.maxValue = kSquareWave_Tremolo_Waveform;
          outParameterInfo.defaultValue = kDefaultValue_Tremolo_Waveform;

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

#pragma mark ____Factory Presets

ComponentResult Tremolo::GetPresets(CFArrayRef *outData) const {
  if (outData == NULL) {
    return noErr;
  }

  CFMutableArrayRef presetsArray = CFArrayCreateMutable(NULL, kNumberPresets, NULL);

  for (int i = 0; i < kNumberPresets; ++i) {
    CFArrayAppendValue(presetsArray, &kPresets[i]);
  }

  *outData = (CFArrayRef)presetsArray;
  return noErr;
}

OSStatus Tremolo::NewFactoryPresetSet(const AUPreset &inNewFactoryPreset) {
  SInt32 chosenPreset = inNewFactoryPreset.presetNumber;

  if (chosenPreset == kPreset_Slow || chosenPreset == kPreset_Fast) {
    for (int i = 0; i < kNumberPresets; ++i) {
      if (chosenPreset == kPresets[i].presetNumber) {
        switch (chosenPreset) {
          case kPreset_Slow:
            SetParameter(kParameter_Frequency, kParameter_Preset_Frequency_Slow);
            SetParameter(kParameter_Depth, kParameter_Preset_Depth_Slow);
            SetParameter(kParameter_Waveform, kParameter_Preset_Waveform_Slow);
            break;

          case kPreset_Fast:
            SetParameter(kParameter_Frequency, kParameter_Preset_Frequency_Fast);
            SetParameter(kParameter_Depth, kParameter_Preset_Depth_Fast);
            SetParameter(kParameter_Waveform, kParameter_Preset_Waveform_Fast);
            break;
        }
        SetAFactoryPresetAsCurrent(kPresets[i]);
        return noErr;
      }
    }
  }
  return kAudioUnitErr_InvalidProperty;
}

#pragma mark ____TremoloEffectKernel

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    Tremolo::TremoloKernel::TremoloKernel()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tremolo::TremoloKernel::TremoloKernel(AUEffectBase *inAudioUnit) : AUKernelBase(inAudioUnit), mSamplesProcessed(0), mCurrentScale(0) {
  for (int i = 0; i < kWaveArraySize; ++i) {
    double radians = i * 2.0 * M_PI / kWaveArraySize;
    mSine[i] = (sin(radians) + 1.0) * 0.5; // the sinus of radians, made positive between 0 and 1
  }

  for (int i = 0; i < kWaveArraySize; ++i) {
      double radians = i * 2.0 * M_PI / kWaveArraySize;
      radians = radians + 0.32;
      mSquare[i] = (sin(radians) +
                     0.3 * sin(3 * radians) +
                     0.15 * sin(5 * radians) +
                     0.075 * sin(7 * radians) +
                     0.0375 * sin(9 * radians) +
                     0.01875 * sin(11 * radians) +
                     0.009375 * sin(13 * radians) +
                     0.8
                   ) * 0.63;
  }

  mSampleFrequency = GetSampleRate();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::TremoloKernel::Reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tremolo::TremoloKernel::Reset() {
  mCurrentScale = 0;
  mSamplesProcessed = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Tremolo::TremoloKernel::Process
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tremolo::TremoloKernel::Process(const Float32 *inSourceP,
                                     Float32       *inDestP,
                                     UInt32        inSamplesToProcess,
                                     UInt32        inNumChannels, // for version 2 AudioUnits inNumChannels is always 1
                                     bool          &ioSilence) {

  if (!ioSilence) {
    const Float32 *sourceP = inSourceP;

    Float32 *destP = inDestP,
            inputSample,
            outputSample,
            tremoloFrequency,
            tremoloDepth,
            samplesPerTremoloCycle,
            rawTremoloGain,
            tremoloGain;

    int tremoloWaveform;

    // Setup values
    tremoloFrequency = GetParameter(kParameter_Frequency);
    tremoloDepth     = GetParameter(kParameter_Depth);
    tremoloWaveform  = (int)GetParameter(kParameter_Waveform);

    if (tremoloWaveform == kSineWave_Tremolo_Waveform) {
      waveArrayPointer = &mSine[0];
    } else {
      waveArrayPointer = &mSquare[0];
    }

    if (tremoloFrequency < kMinimumValue_Tremolo_Freq) {
      tremoloFrequency = kMinimumValue_Tremolo_Freq;
    } else if (tremoloFrequency > kMaximumValue_Tremolo_Freq) {
      tremoloFrequency = kMaximumValue_Tremolo_Freq;
    }

    if (tremoloDepth < kMinimumValue_Tremolo_Depth) {
      tremoloDepth = kMinimumValue_Tremolo_Depth;
    } else if (tremoloDepth > kMaximumValue_Tremolo_Depth) {
      tremoloDepth = kMaximumValue_Tremolo_Depth;
    }

    if (tremoloWaveform != kSineWave_Tremolo_Waveform && tremoloWaveform != kSquareWave_Tremolo_Waveform) {
      tremoloWaveform = kDefaultValue_Tremolo_Waveform;
    }

    samplesPerTremoloCycle = mSampleFrequency / tremoloFrequency;
    mNextScale = kWaveArraySize / samplesPerTremoloCycle;

    // Process loop
    for (int i = inSamplesToProcess; i > 0; --i) {
      int index = static_cast<long>(mSamplesProcessed * mCurrentScale) % kWaveArraySize;

      if (mNextScale != mCurrentScale && index == 0) {
        mCurrentScale = mNextScale;
        mSamplesProcessed = 0;
      }

      if (mSamplesProcessed > sampleLimit && index == 0) {
        mSamplesProcessed = 0;
      }

      rawTremoloGain     = waveArrayPointer[index];
      tremoloGain        = (rawTremoloGain * tremoloDepth - tremoloDepth + 100.0) * 0.01;
      inputSample        = *sourceP;
      outputSample       = inputSample * tremoloGain;
      *destP             = outputSample;
      sourceP           += 1;
      destP             += 1;
      mSamplesProcessed += 1;
    }
  }
}

