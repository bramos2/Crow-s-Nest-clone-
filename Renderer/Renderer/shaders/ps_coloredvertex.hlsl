#include "mvp.hlsli"

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(VS_OUT input) :SV_Target
{
    PS_IN output;
    output.pos = input.pos;
    output.color = input.color;
    return input.color;
}