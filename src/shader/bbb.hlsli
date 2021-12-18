#ifdef __cplusplus
#define cbuffer struct
#define matrix DirectX::XMMATRIX
#endif

cbuffer BBB_t
{
	matrix modeling;
	matrix view;
	matrix projection;
	float4 pos;
	float4 uv_extents;
	float4 etc;
};
