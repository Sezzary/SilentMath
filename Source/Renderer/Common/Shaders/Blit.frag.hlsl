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
    float Brightness;
}

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TexCoord);

    // Compute final fragment color.
    float3 color = texColor.rgb + Brightness;
    return float4(color, 1.0f);

    // Convert 0-1 UVs to integer pixel coordinates.
    /*uint2 pixelCoord = uint2(input.TexCoord * float2(320, 240));

    // Load exact texel (no interpolation/filtering).
    float4 texColor = Texture.Load(int3(pixelCoord, 0));

    // Apply brightness.
    float3 color = texColor.rgb + Brightness;
    return float4(color, 1.0f);*/
}
