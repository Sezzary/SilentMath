#include "Utils/Math.hlsli"

// References:
// https://github.com/TombEngine/TombEngine/blob/cd29462dbde01c15a1797e3029ad362747d22fc4/TombEngine/Shaders/FXAA.hlsl

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
    float  VirtualHeight; // @todo
}

static const float SPAN_MAX    = 8.0f;
static const float REDUCE_MULT = 1.0f / 4.0f;
static const float REDUCE_MIN  = 1.0f / 64.0f;

float4 main(Input input) : SV_TARGET
{
    // Compute texel size.
    float2 texelSize = 1.0f / Resolution;

    // Sample texture in surrounding pixels.
    float3 colorCenter      = Texture.Sample(Sampler, input.TexCoord);
    float3 colorTopLeft     = Texture.Sample(Sampler, input.TexCoord + float2(-texelSize.x, -texelSize.y));
    float3 colorTopRight    = Texture.Sample(Sampler, input.TexCoord + float2( texelSize.x, -texelSize.y));
    float3 colorBottomLeft  = Texture.Sample(Sampler, input.TexCoord + float2(-texelSize.x,  texelSize.y));
    float3 colorBottomRight = Texture.Sample(Sampler, input.TexCoord + float2( texelSize.x,  texelSize.y));

    // Compute luma of surrounding pixels.
    float lumaCenter      = Math::Luma709(colorCenter);
    float lumaTopLeft     = Math::Luma709(colorTopLeft);
    float lumaTopRight    = Math::Luma709(colorTopRight);
    float lumaBottomLeft  = Math::Luma709(colorBottomLeft);
    float lumaBottomRight = Math::Luma709(colorBottomRight);

    // Compute luma range.
    float lumaMin = min(lumaCenter, min(min(lumaTopLeft, lumaTopRight), min(lumaBottomLeft, lumaBottomRight)));
    float lumaMax = max(lumaCenter, max(max(lumaTopLeft, lumaTopRight), max(lumaBottomLeft, lumaBottomRight)));

    // Compute edge normal vector (perpendicular to contrast gradient) using surrounding luma.
    float2 dir = float2(-((lumaTopLeft + lumaTopRight)   - (lumaBottomLeft + lumaBottomRight)),
                          (lumaTopLeft + lumaBottomLeft) - (lumaTopRight   + lumaBottomRight));

    // Compute minimum blur threshold to prevent division by zero and noise artifacting on flat areas.
    float dirReduce = max((lumaTopLeft + lumaTopRight + lumaBottomLeft + lumaBottomRight) * (0.25f * REDUCE_MULT), REDUCE_MIN);

    // Normalize direction by shortest component, applying threshold.
    float rcpDirMin = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    // Clamp blur length to `SPAN_MAX` pixels and convert to UV space.
    dir = min(float2(SPAN_MAX, SPAN_MAX), max(float2(-SPAN_MAX, -SPAN_MAX), dir * rcpDirMin)) * texelSize;

    // Narrow blur pass.
    float3 narrowBlur = (1.0f / 2.0f) * (Texture.Sample(Sampler, input.TexCoord + dir * ((1.0f / 3.0f) - 0.5f)) +
                                         Texture.Sample(Sampler, input.TexCoord + dir * ((2.0f / 3.0f) - 0.5f)));

    // Wide blur pass.
    float3 wideBlur = (narrowBlur * (1.0f / 2.0f)) +
                      ((1.0f / 4.0f) * (Texture.Sample(Sampler, input.TexCoord + (dir * ((0.0f / 3.0f) - 0.5f))) +
                                        Texture.Sample(Sampler, input.TexCoord + (dir * ((3.0f / 3.0f) - 0.5f)))));

    // Compute final color.
    float luma = Math::Luma709(wideBlur);
    if (luma < lumaMin || luma > lumaMax)
    {
        return float4(narrowBlur, 1.0f);
    }
    else
    {
        return float4(wideBlur, 1.0f);
    }
}
