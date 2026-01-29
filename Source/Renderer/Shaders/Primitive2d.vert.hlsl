struct Input
{
    float3 Position     : TEXCOORD0;
    float2 TexCoord : TEXCOORD1;
    float4 Color    : COLOR0;
};

struct Output
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

Output main(Input input)
{
    Output output;

    output.Position     = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    output.Color        = input.Color;
    return output;
}
