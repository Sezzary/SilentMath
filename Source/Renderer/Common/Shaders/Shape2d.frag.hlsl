struct Input
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer PerObject : register(b0, space3)
{
    uint IsFastAlpha;
};

float4 main(Input input) : SV_Target
{
    // Compute alpha.
    float alpha = input.Color.a;
    alpha       = lerp(alpha, step(0.5f, alpha), float(IsFastAlpha));

    // Compute final fragment color.
    float3 color = input.Color.rgb;
    return float4(color, alpha);
}
