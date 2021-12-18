#ifdef __cplusplus
#define cbuffer struct
#define matrix DirectX::XMMATRIX
#define float4 DirectX::XMFLOAT4
#define float3 DirectX::XMFLOAT3
#endif

cbuffer MCB_s
{
    matrix world;
    matrix view;
    matrix projection;
    float4 amblight;
};