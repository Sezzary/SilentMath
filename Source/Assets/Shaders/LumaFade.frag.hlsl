#include "Common/Math.hlsli"

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
    float FadeAlpha;
};

float4 main(Input input) : SV_Target
{
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);
    float  luma     = 1.0f - dot(texColor.rgb, Math::LUMA_BT601);

    float4 color = texColor * Math::Remap(1.0f - FadeAlpha, luma, 1.0f, 0.0f, 1.0f);
    return color;
}
