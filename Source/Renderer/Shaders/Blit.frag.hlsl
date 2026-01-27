Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
    float4 Color        : COLOR0;
};

cbuffer UniformBlock : register(b0, space3)
{
    float Brightness;
    float Contrast;
}

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);
    return texColor;

    // @todo Do this later.

    // Compute final fragment color.
    float3 color = texColor.rgb + Brightness;
    color.rgb = (color.rgb - 0.5f) * Contrast + 0.5f;
    return float4(color, 1.0f);
}
