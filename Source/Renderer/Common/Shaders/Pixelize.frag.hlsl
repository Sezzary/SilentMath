#include "Utils/Math.hlsli"

// References:
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

float4 main(Input input) : SV_Target
{
    // Compute integer-snapped scale based on target virtual height.
    float rawScale = Resolution.y / VirtualHeight;
    float scale    = max(1.0, floor(rawScale));

    // Compute texture coord snapped to pixel grid.
    float2 pixels   = floor(Resolution / scale);
    float2 texCoord = floor(input.TexCoord * pixels) / pixels;

    // Sample final color.
    return Texture.Sample(Sampler, texCoord);
}
