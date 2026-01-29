Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer UniformBlock : register(b0, space3)
{
    uint UseTexture;
    uint IsFastAlpha;
};

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute vertex and texture alpha combination.
    float alpha = lerp(input.Color.a, input.Color.a * texColor.a, float(UseTexture));
    alpha       = lerp(alpha, step(0.5f, alpha), float(IsFastAlpha));

    // Compute final fragment color.
    float3 color = lerp(input.Color.rgb, input.Color.rgb * texColor.rgb, float(UseTexture));
    return float4(color, alpha);
}
