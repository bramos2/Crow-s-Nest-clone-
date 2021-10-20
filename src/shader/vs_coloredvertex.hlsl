#include "mvp.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    output.pos = mul(float4(input.pos, 1.0f), modeling);
    output.pos = mul(output.pos, mul(view, projection));
    output.color = input.color;
    return output;
}