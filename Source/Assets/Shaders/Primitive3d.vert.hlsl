struct Input
{
    float3 Position : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float4 Color    : COLOR0;
};

struct Output
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer UniformBlock : register(b0, space3)
{
    float4x4 ModelMat;
    float4x4 ViewProjMat;
};

Output main(Input input)
{
    Output output;

    float4x4 mvpMat = mul(ViewProjMat, ModelMat);

    output.Position = mul(mvpMat, float4(input.Position, 1.0f));
    output.TexCoord = input.TexCoord;
    output.Color    = input.Color;
    return output;
}
