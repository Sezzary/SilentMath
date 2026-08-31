#include "Utils/Constants.hlsli"

struct Input
{
    float3              Position   : POSITION0;
    float2              TexCoord   : TEXCOORD0;
    float4              Color      : COLOR0;
    nointerpolation int PaletteIdx : TEXCOORD1;
};

struct Output
{
    float4              Position   : SV_Position;
    float2              TexCoord   : TEXCOORD0;
    float4              Color      : COLOR0;
    nointerpolation int PaletteIdx : TEXCOORD1;
};

// @todo Screen space object scaling is already correct, but it would be nicer to use an orthographic view matrix.
// Might refactor later.
/*cbuffer PerFrame : register(b0, space1)
{
    column_major float4x4 ProjectionMat;
    uint                  HasJitter;
    float                 ViewportAspectRatio;
};

cbuffer PerObject : register(b1, space1)
{
    column_major float4x4 ModelMat;
};

float4 GetPosition(float2 inputPos)
{
    // Compute model-projection matrix.
    column_major float4x4 modelProjMat = mul(ViewProjMat, ModelMat);

    // Compute NDC position.
    float4 pos = mul(modelProjMat, float4(inputPos, 1.0f));

    // Apply jitter.
    if (HasJitter)
    {
        // Compute aspect scale 
        float2 aspectScale = (ViewportAspectRatio >= 1.0f) ? float2(ViewportAspectRatio, 1.0f) :
                                                             float2(1.0f, 1.0f / ViewportAspectRatio);
        float2 jitterRes   = Constants::JITTER_HEIGHT * aspectScale;

        // Compute rounded vertex position to produce jitter.
        float2 ndc = pos.xy / pos.w;
        pos.xy     = (round(ndc * jitterRes) / jitterRes) * pos.w;
    }

    return pos;
}*/

Output main(Input input)
{
    Output output;

    //output.Position   = GetPosition(input.Position);
    output.Position   = float4(input.Position, 1.0f);
    output.TexCoord   = input.TexCoord;
    output.Color      = input.Color;
    output.PaletteIdx = input.PaletteIdx;
    return output;
}
