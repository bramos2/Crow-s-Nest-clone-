#ifdef __cplusplus
#define cbuffer struct
#define matrix DirectX::XMMATRIX
#define float4 DirectX::XMFLOAT4
#define float3 DirectX::XMFLOAT3
#endif

cbuffer MCB_t
{
    matrix modeling;
    matrix view;
    matrix projection;
    matrix matrices[30];
    float3 lightColor;
    float surfaceShininess;
    float3 lightPos;
    float lightPower;
    //need inverse bindpose
    //need tween joint transforms
};