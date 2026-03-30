#pragma once

//  A stripped down emulation of a PlayStation 1 SPU, and potentially most of the PS2 SPU if the voice and RAM limits are increased.
//  Implements the most commonly used functionality of the SPU, and specifically all the functionality required by PlayStation Doom.
//  It is completely self isolated (apart from supplied external inputs) and can safely run in a separate thread.
//  Largely based on the SPU implementation of the Avocado PlayStation emulator, and follows it's approach in various places.
//
//  What was removed from this SPU emulation:
//      - All links to a host system, interrupts, system bus reading/writing, DMA etc.
//      - Noise generation
//      - Voice pitch modulation by another voice (used for LFO style effects)
//      - Sweep volume mode for SPU voices

#define SIMPLE_SPU_FLOAT_SPU 1

namespace Silent::Audio
{
    constexpr int    ADPCM_BLOCK_SIZE        = 16;      // The size in bytes of a PSX format ADPCM block
    constexpr int    ADPCM_BLOCK_NUM_SAMPLES = 28;      // The number of samples in a PSX format ADPCM block
    constexpr ushort MAX_SAMPLE_RATE         = 0x4000;  // The PSX cannot do sample rates over 176,400 Hz
    constexpr short  MIN_MASTER_VOLUME       = -0x3FFF; // Minimum master volume level (divided by 2)
    constexpr short  MAX_MASTER_VOLUME       = 0x3FFF;  // Maximum master volume level (divided by 2)
    constexpr short  MIN_ENV_LEVEL           = 0;       // Minimum allowed envelope level
    constexpr short  MAX_ENV_LEVEL           = 0x7FFF;  // Maximum allowed envelope level

    // Flags read from the 2nd byte of a PSX ADPCM block.
    //
    // Meanings:
    //  LOOP_END:       If set then goto the repeat address after we are finished with the current ADPCM block
    //  REPEAT:         Only used if 'LOOP_END' is set, whether we are repeating normally or silencing the voice.
    //                  If NOT set when reaching a sample end , then the volumne envelope is immediately silenced.
    //  LOOP_START:     If set then save the current ADPCM address as the repeat address
    constexpr uint8 ADPCM_FLAG_LOOP_END   = 1 << 0;
    constexpr uint8 ADPCM_FLAG_REPEAT     = 1 << 1;
    constexpr uint8 ADPCM_FLAG_LOOP_START = 1 << 2;

    // Holds information about where we are sampling from in a block of ADPCM samples.
    union AdpcmBlockPos
    {
        // These are what the bits of the counter mean, starting with the least significant bits
        struct
        {
            uint gaussIdxFrac : 4  = 0; // Lower fractional bits of the gauss index
            uint gaussIdx     : 8  = 0; // Gauss index: this is an index into an interpolation table for interpolating the 4 most recent samples
            uint sampleIdx    : 20 = 0; // Index of the current sample in the ADPCM block. Once this exceeds '28' we need to read more ADPCM blocks
        } fields;

        // This is simply incremented by the pitch of the voice.
        // If incremented by '0x1000' then it means we are playing the sample @ 44.1 KHz since the SPU samples at that rate and
        // discarding the lower 12 bits of that number leaves us with '1', or an advancement of 1 sample per 44.1 KHz SPU sample.
        uint counter = 0;
    };

    // Stores the settings for a voice ADSR envelope.
    // Note: this is same bit layout that the PSX spu uses also.
    struct AdsrEnvelope
    {
        uint sustainLevel : 4 = 0; // 0-15: At what envelope level (inclusive) do we go from the decay phase into the sustain phase. The actual envelope level is computed as follows: max((sustainLevel + 1) << 11, 0x7FFF)
        uint decayShift   : 4 = 0; // 0-15: Affects how long the decay portion of the envelope lasts. Lower values mean a faster decay.

        // 0-3: Affects how long the attack portion of the envelope lasts; lower values mean a faster attack.
        // The actual step is computed as follows: 7 - step.
        uint attackStep : 2 = 0;

        // 0-31: Affects how long the attack portion of the envelope lasts.
        // Lower values mean a faster attack.
        uint attackShift  : 5 = 0;
        uint bAttackExp   : 1 = 0; // If set then attack mode is exponential rather than linear
        uint releaseShift : 5 = 0; // 0-31: Affects how long the release portion of the envelope lasts. Lower values mean a faster release.
        uint bReleaseExp  : 1 = 0; // If set then release mode is exponential rather than linear

        // How much to step the envelope in sustain mode.
        // The meaning of this depends on whether the sustain direction is 'increase' or 'decrease'.
        //  Increase: step =  7 - sustainStep
        //  Decrease: step = -8 + sustainStep
        uint sustainStep : 2 = 0;

        // 0-31: Affects the scaling of the sustain envelope phase step. Lower values mean a bigger step amount.
        uint sustainShift : 5 = 0;

        
        uint _unused     : 1 = 0; // An unused bit of the envelope
        uint bSustainDec : 1 = 0; // If set then the sustain envelope is decreased over time. If NOT set then it increases.
        uint bSustainExp : 1 = 0; // Whether the sustain portion of the envelope increases or decreases exponentially. If set then the change is exponential.
    };

    // Settings/params for a particular phase of the envelope. Note that due to the way PSX envelopes work in exponential mode,
    // these parameters can sometimes change midway through the envelope phase depending on the current envelope level.
    struct EnvPhaseParams
    {
        int targetLevel = 0; // What level the current envelope phase is trying to reach: -1 if not applicable
        int step        = 0; // The size of the envelope step
        int stepCycles  = 0; // How many cycles must be waited before doing an envelope step
    };

    // What phase of an envelope a voice is in
    enum class EnvPhase : uint8
    {
        Off,
        Attack,
        Decay,
        Sustain,
        Release
    };

    // Holds stereo volume levels.
    // Note that if the volumes are negative then the wave is phase inverted.
    struct Volume
    {
        short left;
        short right;
    };

    // Convert between a 16-bit sample and floating point
    constexpr float toFloatSample(short sample)
    {
        return (float)sample * (1.0f / 32768.0f);
    }

    constexpr short toInt16Sample(float sample)
    {
        return (short)std::clamp(sample * 32768.0f, float(INT16_MIN), float(INT16_MAX));
    }

#if !SIMPLE_SPU_FLOAT_SPU
    // Do a saturated/clamped addition and subtraction of 16-bit sample values
    short sampleAdd(short sample1, short sample2)
    {
        int result32 = (int)sample1 + (int)sample2;
        return (short)std::clamp<int>(result32, INT16_MIN, INT16_MAX);
    }

    short sampleSub(short sample1, short sample2)
    {
        int result32 = (int)sample1 - (int)sample2;
        return (short)std::clamp<int>(result32, INT16_MIN, INT16_MAX);
    }

    // Do volume attenuation/scaling for a sample; the volume level is a completely fractional number.
    // Note that due to the way 2s complement works, +1.0 can never be expressed fully so we lose a little volume on each multiply.
    // I.E the volume range multiplier is from -0x8000 to +0x7FFF and we divide by 0x8000 essentially via shifting.
    short sampleAttenuate(short sample, short volume)
    {
        int frac32 = (int)sample * (int)volume;
        return (short)(frac32 >> 15);
    }
#endif

    // Convenience container for a 16-bit sample which supports sample add, subtract & attenuate operations
    struct Sample
    {
#if SIMPLE_SPU_FLOAT_SPU
        float value = 0.0f;

        Sample() = default;
        Sample(float value) : value(value) {}
        Sample(short value) : value(toFloatSample(value)) {} // Convenience auto-conversion from 16-bit

        Sample(const Sample& other) = default;
        Sample& operator=(const Sample& other) = default;

        // Convenience overloads for attenuating by a 16-bit volume level
        Sample operator*(const short other) const
        {
            return value * toFloatSample(other);
        }

        void operator*=(const short other)
        {
            value *= toFloatSample(other);
        }

        Sample operator*(float other) const { return value * other; }
        void   operator*=(float other)      { value *= other;       }
        Sample operator+(float other) const { return value + other; }
        void   operator+=(float other)      { value += other;       }
        Sample operator-(float other) const { return value - other; }
        void   operator-=(float other)      { value -= other;       }

        operator float() const { return value; }
#else
        short value = 0;

        Sample() = default;
        Sample(short value) : value(value) {}

        Sample(const Sample& other) = default;
        Sample& operator=(const Sample& other) = default;

        // Convenience overloads for attenuating by a float volume level
        Sample operator*(float other) const
        {
            return toInt16Sample(toFloatSample(value) * other);
        }

        void operator*=(float other)
        {
            value = (*this) * other;
        }

        Sample operator*(short other) const { return sampleAttenuate(value, other);  }
        void   operator*=(short other)      { value = sampleAttenuate(value, other); }
        Sample operator+(short other) const { return sampleAdd(value, other);        }
        void   operator+=(short other)      { value = sampleAdd(value, other);       }
        Sample operator-(short other) const { return sampleSub(value, other);        }
        void   operator-=(short other)      { value = sampleSub(value, other);       }

        operator short() const { return value; }
#endif
    };

    // Holds a stereo sample and allows add/subtract/attenuate operations on that stereo sample
    struct StereoSample
    {
        Sample left  = Sample();
        Sample right = Sample();

        StereoSample operator+(const StereoSample other) const
        {
            return StereoSample
            {
                .left  = left  + other.left,
                .right = right + other.right
            };
        }

        void operator+=(const StereoSample other)
        {
            left  += other.left;
            right += other.right;
        }

        StereoSample operator-(const StereoSample& other) const
        {
            return StereoSample
            {
                .left  = left  - other.left,
                .right = right - other.right
            };
        }

        void operator-=(const StereoSample& other)
        {
            left  -= other.left;
            right -= other.right;
        }

        StereoSample operator*(const Volume& volume) const
        {
            return StereoSample
            {
                .left  = left  * volume.left,
                .right = right * volume.right
            };
        }

        StereoSample operator*(float scale) const
        {
            return StereoSample
            {
                .left  = left  * scale,
                .right = right * scale
            };
        }

        void operator*=(const Volume& volume)
        {
            left  *= volume.left;
            right *= volume.right;
        }

        void operator*=(float scale)
        {
            left  *= scale;
            right *= scale;
        }
    };

    // Reverb registers: determine how reverb is processed.
    // These are from the NO$PSX spec and in the same memory arrangement as the PSX.
    //
    // Notes:
    //  (1) All address offset values are in terms of 8 byte multiples; multiply by 8 to get the real offset.
    //  (2) For more details see the NO$PSX spec: https://problemkaputt.de/psx-spx.htm#soundprocessingunitspu
    //
    struct ReverbRegs
    {
        ushort dispAPF1   = 0; // Reverb APF Offset 1
        ushort dispAPF2   = 0; // Reverb APF Offset 2
        short  volIIR     = 0; // Reverb Reflection Volume 1
        short  volComb1   = 0; // Reverb Comb Volume 1
        short  volComb2   = 0; // Reverb Comb Volume 2
        short  volComb3   = 0; // Reverb Comb Volume 3
        short  volComb4   = 0; // Reverb Comb Volume 4
        short  volWall    = 0; // Reverb Reflection Volume 2
        short  volAPF1    = 0; // Reverb APF Volume 1
        short  volAPF2    = 0; // Reverb APF Volume 2
        ushort addrLSame1 = 0; // Reverb Same Side Reflection Address 1: Left
        ushort addrRSame1 = 0; // Reverb Same Side Reflection Address 1: Right
        ushort addrLComb1 = 0; // Reverb Comb Address 1: Left
        ushort addrRComb1 = 0; // Reverb Comb Address 1: Right
        ushort addrLComb2 = 0; // Reverb Comb Address 2: Left
        ushort addrRComb2 = 0; // Reverb Comb Address 2: Right
        ushort addrLSame2 = 0; // Reverb Same Side Reflection Address 2: Left
        ushort addrRSame2 = 0; // Reverb Same Side Reflection Address 2: Right
        ushort addrLDiff1 = 0; // Reverb Different Side Reflect Address 1: Left
        ushort addrRDiff1 = 0; // Reverb Different Side Reflect Address 1: Right
        ushort addrLComb3 = 0; // Reverb Comb Address 3: Left
        ushort addrRComb3 = 0; // Reverb Comb Address 3: Right
        ushort addrLComb4 = 0; // Reverb Comb Address 4: Left
        ushort addrRComb4 = 0; // Reverb Comb Address 4: Right
        ushort addrLDiff2 = 0; // Reverb Different Side Reflect Address 2: Left
        ushort addrRDiff2 = 0; // Reverb Different Side Reflect Address 2: Right
        ushort addrLAPF1  = 0; // Reverb APF Address 1: Left
        ushort addrRAPF1  = 0; // Reverb APF Address 1: Right
        ushort addrLAPF2  = 0; // Reverb APF Address 2: Left
        ushort addrRAPF2  = 0; // Reverb APF Address 2: Right
        short  volLIn     = 0; // Reverb Input Volume: Left
        short  volRIn     = 0; // Reverb Input Volume: Right
    };

    // Holds all of the state for a hardware SPU voice
    struct Voice
    {
        // How many previous decoded samples to store for an SPU voice; these are required sometimes for sample interpolation
        static constexpr int NUM_PREV_SAMPLES = 3;

        // How many samples there are in the sample buffer for the voice
        static constexpr int SAMPLE_BUFFER_SIZE = NUM_PREV_SAMPLES + ADPCM_BLOCK_NUM_SAMPLES;

        // Start address (in 8 byte units) of the current sound.
        // The current address of the voice is set from this on the 'key on' event.
        // Note: on the original PSX SPU this was a 16-bit quantity, so it could only reference up to 512 KiB of SRAM.
        uint adpcmStartAddr8 = 0;

        // Current address that the voice is reading ADPCM samples from in SRAM, in 8 byte units.
        // Note: on the original PSX SPU this was a 16-bit quantity, so it could only reference up to 512 KiB of SRAM.
        uint adpcmCurAddr8 = 0;

        // Repeat address (in 8 byte units) of the current sound.
        // If an ADPCM packet header has a 'loop start' flag set, then this field will be set.
        // If an ADPCM packet header has a 'loop end' flag set, then the SPU will jump to this address.
        // Note: on the original PSX SPU this was a 16-bit quantity, so it could only reference up to 512 KiB of SRAM.
        uint adpcmRepeatAddr8 = 0;

        AdpcmBlockPos adpcmBlockPos = {}; // Where we are currently in the ADPCM block

        // Current pitch/sample-rate of the voice.
        // A value of 0x1000 means 44,100 Hz, half that is 22,050 Hz and so on.
        ushort sampleRate = 0;

        uint8 bDisabled       : 1 = 0; // Mix in this voice?
        uint8 bRepeat         : 1 = 0; // If set then goto the repeat address next time we load samples
        uint8 bReachedLoopEnd : 1 = 0; // Set when we reach an ADPCM block with 'ADPCM_FLAG_LOOP_END' set and cleared on 'key on'; tells if the sample has reached the end at least once
        uint8 bSamplesLoaded  : 1 = 0; // If set then the voice has loaded an ADPCM sample block
        uint8 bDoReverb       : 1 = 0; // If set then reverb is enabled for the voice
        uint8 _unused         : 3 = 0; // Unused bit flags

        EnvPhase envPhase; // Current envelope phase
        AdsrEnvelope env; // The ADSR envelope to use

        // How many cycles to wait before processing the envelope.
        // This is generally always '1' but can be larger for really slow envelopes.
        int envWaitCycles = 0;

        // Left and right volume levels, divided by 2.
        // Note: I am not supporting the 'sweep volume' mode that original PSX SPU used, when the highest bit of these volume level fields was set.
        // These values are just purely fixed volume levels instead.
        Volume volume = {};

        short envLevel = 0; // Current ADSR envelope volume level

        // Previously decoded samples from the last ADPCM block (3 previous samples) and the currently decoded ADPCM block.
        // At the beginning of the buffer there is 'NUM_PREV_SAMPLES' samples from the last ADPCM block, with the most recent sample last.
        // Those previous samples are used for gaussian interpolation.
        Sample samples[SAMPLE_BUFFER_SIZE];
    };

    // A callback which is invoked by the SPU to provide external input.
    // Can be used to mix in CD audio or anything else and run it through the reverb processing of the SPU.
    // The callback takes a single piece of user data and must return 1 sound sample.
    typedef StereoSample (*ExtInputCallback)(void* pUserData);

    /** @brief Virutal PSX SPU device core. */
    class VirtualSpu
    {
    public:
        // =======
        // Fields
        // =======

        std::byte*         pRam                 = {};      // Sound RAM used by the SPU core
        uint               ramSize              = 0;       // How big the RAM size for the SPU core
#if SIMPLE_SPU_FLOAT_SPU
        float*             pReverbRam           = nullptr; // Holds floating point reverb samples for the extended floating point SPU
        uint               reverbRamSampleCount = 0;       // The number of floating point samples in reverb RAM
#endif
        std::vector<Voice> pVoices              = {};      // Each of the hardware voices for the SPU
        Volume             masterVol            = {};      // Master volume. Note: expected to be from -0x3FFF to +0x3FFF.
        Volume             reverbVol            = {};      // Reverb volume level
        Volume             extInputVol          = {};      // External input volume (I'm using this for CD audio mixing)
        bool               bUnmute              = false;   // If 'true' then the output from voices is mixed into the output
        bool               bReverbWriteEnable   = false;   // Whether reverb can write output to the reverb work area
        bool               bExtEnabled          = false;   // Whether to mix input from the external input source
        bool               bExtReverbEnable     = false;   // Whether to apply reverb on the input from the external source
        ExtInputCallback   pExtInputCallback    = nullptr; // Callback used to source external input: if null no external input is mixed with SPU voices
        void*              pExtInputUserData    = nullptr; // User data passed to the external input callback
        uint               cycleCount           = 0;       // How many cycles has the SPU done (44,100 == 1 second of audio): each cycle is generating a 16-bit left & right audio sample
        uint               reverbBaseAddr8      = 0;       // Start address of the reverb work area in 8 byte units; anything past this address in SPU RAM is for reverb
        uint               reverbCurAddr        = 0;       // Used for relative reads and writes to the reverb work area; continously incremented and wrapped as reverb is processed
        StereoSample       processedReverb      = {};      // The processed reverb that is to be added into the final mix: only updated at 22,050 Hz instead of 44,100 Hz (every 2 SPU steps)
        ReverbRegs         reverbRegs           = {};      // Registers with settings determining how reverb is processed: determines the type of reverb

        // =============
        // Constructors
        // =============

        VirtualSpu() = default;

        // ==========
        // Utilities
        // ==========

#if SIMPLE_SPU_FLOAT_SPU
        void Initialize(int reverbRamSampleCount = 128 * 1024); // More than big enough for any of the reverb modes in LIBSPU
#else
        void Initialize();
#endif

        void Deinitialize();

        /** @brief Steps the SPU core. */
        StereoSample Step();

        // Key on or off the given SPU voice
        void SetKeyOn(Voice& voice);
        void SetKeyOff(Voice& voice);

    private:
        void stepVoice(Voice& voice, StereoSample& output, StereoSample& outputToReverb);

        void stepVoices(StereoSample& output, StereoSample& outputToReverb);

        void mixExternalInput(const ExtInputCallback pExtCallback, void* pExtCallbackUserData, Volume extVolume,
                              bool bExtReverbEnabled,
                              StereoSample& output, StereoSample& outputToReverb);

        void doMasterMix(const StereoSample& dryOutput, const StereoSample& reverbOutput, const Volume& masterVol,
                         StereoSample& output);
    };

    // @todo Create LIBSPU compatibility layer.
    //uint SpuGetReverbVoice();
    //int  SpuGetKeyStatus(uint voice_bitmask);
    //void SpuGetVoiceAttr(SpuVoiceAttr* attr);
    //void SpuSetKey(long on_off, u_long voice_bit);
    //int  SpuReserveReverbWorkArea(int rev_mode);
    //void SpuQuit(void);
}
