#include "Common/Math.hlsli"

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float2 TextureCoord : TEXCOORD0;
};

cbuffer UniformBlock : register(b0, space3)
{
    float BlendAlpha;
};

float4 main(Input input) : SV_Target
{
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);
    float  luma     = dot(texColor.rgb, Math::LUMA_BT601);

    float4 color = texColor * Math::Remap(BlendAlpha, 1.0f - luma, 1.0f, 0.0f, 1.0f);
    return color;
}
