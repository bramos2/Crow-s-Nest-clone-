#include "bbb.hlsli"

struct VS_OUT {
	float4 outpos : SV_POSITION;
	float2 outuv  : TEXCOORD;
	float4 etc    : ETC;
};

VS_OUT main(uint i : SV_VertexId) {
	VS_OUT output;
	
	// load position
	output.outpos = float4(pos.xyz, 1);
	output.etc = etc;

	// matrix multiplication
	output.outpos = mul(output.outpos, modeling);
	output.outpos = mul(output.outpos, view);
	output.outpos = mul(output.outpos, projection);
	
	// generate uv map
	output.outuv = float3(i % 2, (i % 4) >> 1, 0);

	// apply uv spacing to position
    output.outpos += float4((output.outuv.x - pos.w / 2) * pos.w, (output.outuv.y - pos.w / 2) * -pos.w, 0, 0);

	return output;
}