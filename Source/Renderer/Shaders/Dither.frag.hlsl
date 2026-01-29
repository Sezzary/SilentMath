#include "Common/Math.hlsli"

// References:
// https://gist.github.com/ompuco/3209f1b32213cec5b7bccf0e67caf3e9

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
    float4 Color        : COLOR0;
};

static const int4x4 PSX_DITHER_TABLE = int4x4
(
    0,  8,  2,  10,
    12, 4,  14, 6,
    3,  11, 1,  9,
    15, 7,  13, 5
);

float4 main(Input input) : SV_Target
{
    static const int  DITHER_SIZE = 4;
    static const uint DITHER_CEIL = 0xF8;

    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);

    // Compute pixel position.
    int2 pixelPos = int2(floor(input.Position.xy));

    // Compute 8-bit dithered color.
    int    dither    = PSX_DITHER_TABLE[pixelPos.x % DITHER_SIZE][pixelPos.y % DITHER_SIZE];
    float3 color8Bit = texColor.rgb * float(Math::UINT8_MAX);
    color8Bit       += (dither / 2.0f) - 4.0f;

    // 5-bit color truncation.
    color8Bit = lerp((uint3(color8Bit) & DITHER_CEIL), DITHER_CEIL, step(DITHER_CEIL, color8Bit));

    // Compute final color.
    return float4(color8Bit / float(Math::UINT8_MAX), 1.0f);
}
