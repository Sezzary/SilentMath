// References:
// https://gist.github.com/ompuco/3209f1b32213cec5b7bccf0e67caf3e9

Texture2D<float4> Texture : register(t0, space2);
SamplerState      Sampler : register(s0, space2);

struct Input
{
    float4 Position     : SV_Position;
    float2 TextureCoord : TEXCOORD0;
    float4 Color        : COLOR0;
};

static const int4x4 PSX_DITHER_TABLE = int4x4
(
    0,  8,  2,  10,
    12, 4,  14, 6,
    3,  11, 1,  9,
    15, 7,  13, 5
);

float4 main(Input input) : SV_Target
{
    // Sample texture.
    float4 texColor = Texture.Sample(Sampler, input.TextureCoord);

    // Compute pixel position.
    int2 pixelPos = int2(floor(input.Position.xy));

    // Dithered color.
    int    dither    = PSX_DITHER_TABLE[pixelPos.x % 4][pixelPos.y % 4];
    float3 color8Bit = texColor.rgb * 255.0f;
    color8Bit       += (dither / 2.0f) - 4.0f;
    color8Bit        = lerp((uint3(color8Bit) & 0xF8), 0xF8, step(0xF8, color8Bit));
    color8Bit       /= 255.0f;

    // Compute funal color.
    return float4(color8Bit, 1.0f);
}
