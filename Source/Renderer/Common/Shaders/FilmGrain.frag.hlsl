#include "Utils/Math.hlsli"

// References:
// https://www.shadertoy.com/view/3sGGRz
// https://www.shadertoy.com/view/Xt23DD

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer PerFrame : register(b0, space3)
{
    float Time;
}

static const float3 LUMA_16MM_FILM  = float3(0.2126f, 0.7152f, 0.0722f);
static const float2 FREQ_SCALE      = float2(12.9898f, 78.233f);
static const float  PHASE_AMPLITUDE = 43758.5453f;
static const float  LUMA_SCALE      = 1.7f;
static const float  MDF             = 0.1f;

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute noise.
    float noise    = frac(sin(dot(input.TexCoord + frac(Time), FREQ_SCALE)) * PHASE_AMPLITUDE);
    float luma     = dot(texColor.rgb, LUMA_16MM_FILM);
    float adjNoise = noise * (luma * LUMA_SCALE);

    // Compute final color.
    return texColor - (adjNoise * MDF);
}
