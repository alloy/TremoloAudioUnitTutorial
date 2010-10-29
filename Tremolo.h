/*
*  File:    Tremolo.h
*  
*  Version:  1.0
* 
*  Created:  27-10-10
*  
*  Copyright:  Copyright © 2010 __MyCompanyName__, All Rights Reserved
* 
*  Disclaimer:  IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc. ("Apple") in 
*        consideration of your agreement to the following terms, and your use, installation, modification 
*        or redistribution of this Apple software constitutes acceptance of these terms.  If you do 
*        not agree with these terms, please do not use, install, modify or redistribute this Apple 
*        software.
*
*        In consideration of your agreement to abide by the following terms, and subject to these terms, 
*        Apple grants you a personal, non-exclusive license, under Apple's copyrights in this 
*        original Apple software (the "Apple Software"), to use, reproduce, modify and redistribute the 
*        Apple Software, with or without modifications, in source and/or binary forms; provided that if you 
*        redistribute the Apple Software in its entirety and without modifications, you must retain this 
*        notice and the following text and disclaimers in all such redistributions of the Apple Software. 
*        Neither the name, trademarks, service marks or logos of Apple Computer, Inc. may be used to 
*        endorse or promote products derived from the Apple Software without specific prior written 
*        permission from Apple.  Except as expressly stated in this notice, no other rights or 
*        licenses, express or implied, are granted by Apple herein, including but not limited to any 
*        patent rights that may be infringed by your derivative works or by other works in which the 
*        Apple Software may be incorporated.
*
*        The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO WARRANTIES, EXPRESS OR 
*        IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY 
*        AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE 
*        OR IN COMBINATION WITH YOUR PRODUCTS.
*
*        IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL 
*        DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*        OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, 
*        REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER 
*        UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN 
*        IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#include "AUEffectBase.h"
#include "TremoloVersion.h"

#if AU_DEBUG_DISPATCHER
  #include "AUDebugDispatcher.h"
#endif


#ifndef __Tremolo_h__
#define __Tremolo_h__


#pragma mark ____Tremolo Parameters

// parameters
static const float kDefaultValue_ParamOne = 0.5;

static CFStringRef kParameterOneName = CFSTR("Parameter One");

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
statuc CFStringRef kMenuItem_Tremolo_Square = CFSTR("Square");

// parameter identifiers
enum {
  kParameter_Frequency = 0;
  kParameter_Depth     = 1;
  kParameter_Waveform  = 2;
  kNumberOfParameters  = 3;
};

#pragma mark ____Tremolo
class Tremolo : public AUEffectBase
{
public:
  Tremolo(AudioUnit component);
#if AU_DEBUG_DISPATCHER
  virtual ~Tremolo () { delete mDebugDispatcher; }
#endif
  
  virtual AUKernelBase *    NewKernel() { return new TremoloKernel(this); }
  
  virtual  OSStatus      GetParameterValueStrings(AudioUnitScope      inScope,
                             AudioUnitParameterID    inParameterID,
                             CFArrayRef *      outStrings);
    
  virtual  OSStatus      GetParameterInfo(AudioUnitScope      inScope,
                         AudioUnitParameterID  inParameterID,
                         AudioUnitParameterInfo  &outParameterInfo);
    
  virtual OSStatus      GetPropertyInfo(AudioUnitPropertyID    inID,
                        AudioUnitScope      inScope,
                        AudioUnitElement    inElement,
                        UInt32 &      outDataSize,
                        Boolean  &      outWritable );
  
  virtual OSStatus      GetProperty(AudioUnitPropertyID inID,
                      AudioUnitScope     inScope,
                      AudioUnitElement     inElement,
                      void *      outData);
  
   virtual  bool        SupportsTail () { return false; }
  
  /*! @method Version */
  virtual OSStatus    Version() { return kTremoloVersion; }
  
    
  
protected:
    class TremoloKernel : public AUKernelBase    // most of the real work happens here
  {
public:
    TremoloKernel(AUEffectBase *inAudioUnit )
    : AUKernelBase(inAudioUnit)
  {
  }
    
    // *Required* overides for the process method for this effect
    // processes one channel of interleaved samples
        virtual void     Process(  const Float32   *inSourceP,
                    Float32       *inDestP,
                    UInt32       inFramesToProcess,
                    UInt32      inNumChannels,
                    bool      &ioSilence);
    
        virtual void    Reset();
    
    //private: //state variables...
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif
