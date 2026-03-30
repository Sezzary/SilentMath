#include "Framework.h"
#include "Audio/Audio.h"

#include "Application.h"
#include "Audio/Spu.h"
#include "Utils/Video.h"

using namespace Silent::Utils;

namespace Silent::Audio
{
    constexpr int SAMPLE_RATE    = 44100;
    constexpr int BUFFER_SAMPLES = 4096;

    struct VoiceTest
    {
        float phase = 0.0f;
        float frequency = 0.0f;
        float volumeLeft = 0.0f;
        float volumeRight = 0.0f;
        bool  active = false;
    };

    static VoiceTest voice;

    void GenerateSamples(int16* buffer, int sampleCount)
    {
        for (int i = 0; i < sampleCount; i++)
        {
            float left  = 0.0f;
            float right = 0.0f;

            if (voice.active)
            {
                float sample = sin((voice.phase * 2.0f) * PI);

                voice.phase += voice.frequency / SAMPLE_RATE;
                if (voice.phase >= 1.0f)
                {
                    voice.phase -= 1.0f;
                }

                left  += sample * voice.volumeLeft;
                right += sample * voice.volumeRight;
            }

            // Clamp to `int16`.
            int l = (int)(left * 32767.0f);
            int r = (int)(right * 32767.0f);

            if (l > 32767)
            {
                l = 32767;
            }
            if (l < -32768)
            {
                l = -32768;
            }
            if (r > 32767)
            {
                r = 32767;
            }
            if (r < -32768)
            {
                r = -32768;
            }

            buffer[i * 2]       = (int16)l;
            buffer[(i * 2) + 1] = (int16)r;
        }
    }

    void AudioManager::Initialize()
    {
        _device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        auto destSpec = SDL_AudioSpec{};
        SDL_GetAudioDeviceFormat(_device, &destSpec, nullptr);

        auto srcSpec = SDL_AudioSpec
        {
            .format   = SDL_AUDIO_F32,
            .channels = 2,
            .freq     = SAMPLE_RATE
        };
        _stream = SDL_CreateAudioStream(&destSpec, &destSpec);

        SDL_BindAudioStream(_device, _stream);
        SDL_ResumeAudioDevice(_device);

        _spu.Initialize();

        // Init fake voice.
        voice.phase = 0.0f;
        voice.frequency = 440.0f; // A4
        voice.volumeLeft = 0.5f;
        voice.volumeRight = 0.5f;
        voice.active = true;
    }

    void AudioManager::Deinitialize()
    {
        SDL_CloseAudioDevice(_device);
    }

    void AudioManager::Update()
    {
        auto& video = g_App.GetVideo();

        // @todo Ear-bursting distortion.
        return;

        auto buffer = video.GetAudioFrame();
        if (!buffer.empty())
        {
            SDL_PutAudioStreamData(_stream, buffer.data(), buffer.size() * sizeof(float));
        }
    }
}
