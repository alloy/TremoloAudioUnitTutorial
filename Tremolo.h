#include "AUEffectBase.h"
#include "TremoloVersion.h"

#if AU_DEBUG_DISPATCHER
  #include "AUDebugDispatcher.h"
#endif


#ifndef __Tremolo_h__
#define __Tremolo_h__


#pragma mark ____Tremolo Parameters

// parameters
static CFStringRef kParamName_Tremolo_Freq      = CFSTR("Frequency");
static const float kDefaultValue_Tremolo_Freq   = 2.0;
static const float kMinimumValue_Tremolo_Freq   = 0.5;
static const float kMaximumValue_Tremolo_Freq   = 20.0;

static CFStringRef kParamName_Tremolo_Depth     = CFSTR("Depth");
static const float kDefaultValue_Tremolo_Depth  = 50.0;
static const float kMinimumValue_Tremolo_Depth  = 0.0;
static const float kMaximumValue_Tremolo_Depth  = 100.0;

static CFStringRef kParamName_Tremolo_Waveform  = CFSTR("Waveform");
static const int kSineWave_Tremolo_Waveform     = 1;
static const int kSquareWave_Tremolo_Waveform   = 2;
static const int kDefaultValue_Tremolo_Waveform = kSineWave_Tremolo_Waveform;

// menu item names for the waveform parameter
static CFStringRef kMenuItem_Tremolo_Sine = CFSTR("Sine");
static CFStringRef kMenuItem_Tremolo_Square = CFSTR("Square");

// parameter identifiers
enum {
  kParameter_Frequency = 0,
  kParameter_Depth     = 1,
  kParameter_Waveform  = 2,
  kNumberOfParameters  = 3
};

#pragma mark ____Tremolo Factory Preset Constants

static const float kParameter_Preset_Frequency_Slow = 2.0;
static const float kParameter_Preset_Frequency_Fast = 20.0;
static const float kParameter_Preset_Depth_Slow     = 50.0;
static const float kParameter_Preset_Depth_Fast     = 90.0;
static const float kParameter_Preset_Waveform_Slow  = kSineWave_Tremolo_Waveform;
static const float kParameter_Preset_Waveform_Fast  = kSquareWave_Tremolo_Waveform;

enum {
  kPreset_Slow   = 0,
  kPreset_Fast   = 1,
  kNumberPresets = 2
};

static AUPreset kPresets[kNumberPresets] = {
  { kPreset_Slow, CFSTR("Slow & Gentle") },
  { kPreset_Fast, CFSTR("Fast & Hard") }
};

static const int kPreset_Default = kPreset_Slow;

#pragma mark ____Tremolo
class Tremolo : public AUEffectBase {
public:
  Tremolo(AudioUnit component);
#if AU_DEBUG_DISPATCHER
  virtual ~Tremolo () { delete mDebugDispatcher; }
#endif

  virtual AUKernelBase *    NewKernel() { return new TremoloKernel(this); }

  virtual ComponentResult GetPresets(CFArrayRef *outData) const;

  virtual OSStatus NewFactoryPresetSet(const AUPreset &inNewFactoryPreset);

  virtual ComponentResult GetParameterValueStrings(AudioUnitScope inScope, AudioUnitParameterID inParameterID, CFArrayRef *outStrings);
  virtual ComponentResult GetParameterInfo(AudioUnitScope inScope, AudioUnitParameterID inParameterID, AudioUnitParameterInfo &outParameterInfo);

  virtual OSStatus      GetPropertyInfo(AudioUnitPropertyID    inID,
                        AudioUnitScope      inScope,
                        AudioUnitElement    inElement,
                        UInt32 &      outDataSize,
                        Boolean  &      outWritable );
  
  virtual OSStatus      GetProperty(AudioUnitPropertyID inID,
                      AudioUnitScope     inScope,
                      AudioUnitElement     inElement,
                      void *      outData);

  virtual bool SupportsTail() { return true; }

  /*! @method Version */
  virtual OSStatus    Version() { return kTremoloVersion; }

protected:

  class TremoloKernel : public AUKernelBase {   // most of the real work happens here

  public:
    TremoloKernel(AUEffectBase *inAudioUnit);

    // *Required* overrides for the process method for this effect
    // processes one channel of interleaved samples
    virtual void Process(const Float32   *inSourceP,
                         Float32         *inDestP,
                         UInt32          inFramesToProcess,
                         UInt32          inNumChannels,
                         bool            &ioSilence);

    virtual void    Reset();

  private:
    enum    { kWaveArraySize = 2000 };
    float   mSine[kWaveArraySize];
    float   mSquare[kWaveArraySize];
    float   *waveArrayPointer;
    Float32 mSampleFrequency;
    long    mSamplesProcessed;
    enum    { sampleLimit = (int)10E6 };
    float   mCurrentScale;
    float   mNextScale;
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif
