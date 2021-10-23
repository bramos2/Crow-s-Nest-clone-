#include "scb.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 nrm : NORMAL;
    float2 uv : TEXCOORD;
    float4 world_pos : POSTIONT;
    float4 eye_pos : EYEPOS;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    output.world_pos = mul(float4(input.pos, 1.0f), world);
    output.pos = mul(output.world_pos, mul(view, projection));
    
    output.nrm = mul(float4(input.nrm, 0.0f), world);
    
    output.uv = input.uv;
    
    for (int i = 0; i < 3; ++i)
    {
        output.eye_pos[i] = -dot(view[3].xyz, view[i].xyz);
    }
    output.eye_pos.w = 1.0f;
    
    return output;
}