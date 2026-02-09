struct Input
{
    float3 Position : POSITION0;
    float3 Normal   : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

struct Output
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

cbuffer PerFrame : register(b0, space1)
{
    column_major float4x4 ViewProjMat;
};

cbuffer PerObject : register(b1, space1)
{
    column_major float4x4 ModelMat;
};

Output main(Input input)
{
    Output output;

    column_major float4x4 mvpMat = mul(ViewProjMat, ModelMat);

    output.Position = mul(mvpMat, float4(input.Position, 1.0f));
    output.TexCoord = input.TexCoord;
    output.Color    = input.Color;
    return output;
}
