#include "Utils/Constants.hlsli"
#include "Utils/Math.hlsli"

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer PerObject : register(b0, space3)
{
    uint  HasGradient;
    uint  GradientSteps;
    float GradientUvMinY;
    float GradientUvMaxY;
};

static const float HIGHLIGHT = 248.0f / (float)Constants::UINT8_MAX;
static const float LOWLIGHT  = 167.0f / (float)Constants::UINT8_MAX;

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute local values.
    float localY = Math::Remap(input.TexCoord.y, GradientUvMinY, GradientUvMaxY, 0.0f, 1.0f);
    float dist   = abs(localY - 0.5f) * 2.0f;

    // Compute gradient factor.
    float smooth      = saturate(1.0f - dist);
    float stepped     = floor(smooth * float(GradientSteps)) / max(1.0f, float(GradientSteps));
    float factorAlpha = (GradientSteps > 0) ? stepped : smooth;
    float factor      = lerp(LOWLIGHT, HIGHLIGHT, factorAlpha);

    // Combine color and apply gradient if active.
    float3 finalColor = input.Color.rgb * texColor.rgb;
    finalColor       *= lerp(1.0f, factor, float(HasGradient));

    // Compute final color.
    float alpha = input.Color.a * texColor.a;
    return float4(finalColor, alpha);
}
