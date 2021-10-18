#include "mcb.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD;
    int4 indices : BLENDINDICES;
    float4 weights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 nrm : NORMAL;
    float2 uv : TEXCOORD;
    float4 world_pos : POSTIONT;
    float4 eye_pos : EYEPOS;
};

VS_OUT main( VS_IN input )
{
    VS_OUT output = (VS_OUT)0;
    
    float4 skinned_pos = { 0.0f, 0.0f, 0.0f, 0.0f };
    float4 skinned_norm = { 0.0f, 0.0f, 0.0f, 0.0f };

    for (int j = 0; j < 4; ++j)
    {
        //matrix temp = mul(bindPose[input.indices[j]], tweenTransforms[input.indices[j]]);
        skinned_pos += mul(float4(input.pos.xyz, 1.0f), matrices[input.indices[j]]) * input.weights[j];
        skinned_norm += mul(float4(input.nrm.xyz, 0.0f), matrices[input.indices[j]]) * input.weights[j];
    }
    
    output.world_pos = mul(float4(skinned_pos.xyz, 1.0f), modeling);
    output.pos = mul(output.world_pos, mul(view, projection));
    
    for (int i = 0; i < 3; ++i){
        output.eye_pos[i] = -dot(view[3].xyz, view[i].xyz);
    }
    output.eye_pos.w = 1.0f;
    
    output.nrm = mul(float4(skinned_norm.xyz, 0.0f), modeling);
    output.uv = input.uv;
    
	return output;
}