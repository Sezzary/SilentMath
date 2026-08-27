#include "Utils/Constants.hlsli"

// References:
// https://github.com/AlexeyNazariev/PS1-Graphics-Kit-URP/blob/f047fabf5b2492d9572ed85b4ac0cde2ff3b4004/Shaders/PS1_ObjectShader.shader

struct Input
{
    float3              Position   : POSITION0;
    float3              Normal     : NORMAL0;
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

cbuffer PerFrame : register(b0, space1)
{
    column_major float4x4 ViewProjectionMat;
    uint                  HasJitter;
    float                 ViewportAspectRatio;
};

cbuffer PerObject : register(b1, space1)
{
    column_major float4x4 ModelMat;
};

float4 GetPosition(float3 inputPos)
{
    // Compute model-view-projection matrix.
    column_major float4x4 modelViewProjMat = mul(ViewProjectionMat, ModelMat);

    // Compute NDC position.
    float4 pos = mul(modelViewProjMat, float4(inputPos, 1.0f));

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
}

Output main(Input input)
{
    Output output;
    output.Position   = GetPosition(input.Position);
    output.TexCoord   = input.TexCoord;
    output.Color      = input.Color;
    output.PaletteIdx = input.PaletteIdx;
    return output;
}
