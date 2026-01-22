#include "Common/Utils.hlsli"

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float2 TextureCoord : TEXCOORD0;
    float4 Color        : COLOR0;
};

cbuffer UniformBlock : register(b0, space3)
{
    float UvMinY;
    float UvMaxY;
    float Center;
    uint  GradientSteps;
    //--
    uint  HasGradient;
};

float4 main(Input input) : SV_Target
{
    static const float HIGHLIGHT = 0.97f;
    static const float LOWLIGHT  = 0.65f;

    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);

    // Compute local values.
    float localY = Utils::Remap(input.TextureCoord.y, UvMinY, UvMaxY, 0.0f, 1.0f);
    float dist   = abs(localY - Center) * 2.0f;

    // Compute gradient factor.
    float smooth      = saturate(1.0f - dist);
    float stepped     = floor(smooth * float(GradientSteps)) / max(1.0f, float(GradientSteps));
    float factorAlpha = (GradientSteps > 0) ? stepped : smooth;
    float factor      = lerp(LOWLIGHT, HIGHLIGHT, factorAlpha);

    // Combine color and apply gradient if active.
    float3 finalColor = input.Color.rgb * texColor.rgb;
    finalColor       *= lerp(1.0f, factor, float(HasGradient));

    float alpha = input.Color.a * texColor.a;
    return float4(finalColor, alpha);
}
