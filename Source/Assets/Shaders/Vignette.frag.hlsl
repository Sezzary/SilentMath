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
    float  Time;
}

float4 main(Input input) : SV_Target
{
    static const float STRENGTH = 0.2f;

    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute UV coordinate.
    float2 uv = input.Position.xy / Resolution.xy;

    // Compute vignette effect.
    float vignette = (((16.0f * uv.x) * uv.y) * (1.0f - uv.x)) * (1.0f - uv.y);

    // Compute final fragment color.
    float3 color = texColor.rgb * pow(vignette, STRENGTH);
    return float4(color, 1.0f);
}
