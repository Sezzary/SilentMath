#include "Utils/Constants.hlsli"

// References:
// https://gist.github.com/ompuco/3209f1b32213cec5b7bccf0e67caf3e9
// https://github.com/AlexeyNazariev/PS1-Graphics-Kit-URP

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
    float2 Resolution;
    float  VirtualHeight;
};

static const uint   COLOR_MASK   = 0xF8;
static const int    DITHER_SIZE  = 4;
static const int4x4 DITHER_TABLE = int4x4
(
    0,  8,  2,  10,
    12, 4,  14, 6,
    3,  11, 1,  9,
    15, 7,  13, 5
);

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute virtual pixel position relative to virtual height.
    float  rawScale = Resolution.y / VirtualHeight;
    float  scale    = max(1.0, floor(rawScale));
    float2 pixels   = floor(Resolution / scale);
    int2   pixelPos = int2(floor(input.TexCoord * pixels));

    // Compute 8-bit dithered color.
    int    dither    = DITHER_TABLE[pixelPos.x % DITHER_SIZE][pixelPos.y % DITHER_SIZE];
    float3 color8Bit = texColor.rgb * float(Constants::UINT8_MAX);
    color8Bit       += (dither / 2.0f) - 4.0f;

    // 5-bit color truncation.
    color8Bit = lerp((uint3(color8Bit) & COLOR_MASK), COLOR_MASK, step(COLOR_MASK, color8Bit));

    // Compute final color.
    return float4(color8Bit / float(Constants::UINT8_MAX), 1.0f);
}
