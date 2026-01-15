Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float2 TextureCoord : TEXCOORD0;
    float4 Color        : COLOR0;
};

cbuffer UniformBlock : register(b0, space3)
{
    uint UseTexture;  // `bool`
    uint IsFastAlpha; // `bool`
};

float4 main(Input input) : SV_Target
{
    // @todo Temporary. Untextured primitives are invisible, probably wrong math below.
    if (UseTexture)
    {
        return Texture.Sample(Sampler, input.TextureCoord);
    }
    else
    {
        return input.Color;
    }

    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);
    texColor.a *= float(UseTexture);

    // Compute vertex and texture alpha combination.
    float alpha = input.Color.a * texColor.a;
    alpha       = lerp(alpha, step(0.5f, alpha), float(IsFastAlpha));

    // Compute fragment color.
    float3 color = lerp(input.Color.rgb, input.Color.rgb * texColor.rgb, float(UseTexture));
    return float4(color, alpha);
}
