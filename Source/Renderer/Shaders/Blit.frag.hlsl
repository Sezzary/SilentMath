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
    float Brightness;
}

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute final fragment color.
    float3 color = texColor.rgb + Brightness;
    return float4(color, 1.0f);
}
