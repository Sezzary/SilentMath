struct Input
{
    float3 Position     : TEXCOORD0;
    float2 TextureCoord : TEXCOORD1;
};

struct Output
{
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
};

Output main(Input input)
{
    Output output;

    output.Position     = float4(input.Position, 1.0f);
    output.TextureCoord = input.TextureCoord;
    return output;
}
