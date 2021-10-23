#include "scb.hlsli"

Texture2D tx_diffuse : register(t0);
Texture2D tx_emissive : register(t1);
Texture2D tx_specular : register(t2);
SamplerState samLinear : register(s0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 nrm : NORMAL;
    float2 uv : TEXCOORD;
    float4 world_pos : POSTIONT;
    float4 eye_pos : EYEPOS;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

static const float4 ambientLight = { 0.75f, 0.75f, 0.75f, 0.0f };

PS_OUTPUT main(VS_OUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    float4 matDiffuse = tx_diffuse.Sample(samLinear, input.uv);
    float4 matSpecular = tx_specular.Sample(samLinear, input.uv);
    float4 emissive = tx_emissive.Sample(samLinear, input.uv);
    float4 ambient = matDiffuse * ambientLight;
   
    float4 color = ambient + emissive;
    
    output.color = color;
    return output;
}