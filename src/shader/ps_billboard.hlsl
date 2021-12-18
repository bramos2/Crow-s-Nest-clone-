Texture2D in_texture : register(t0);
SamplerState samLinear : register(s0);

// just do your thing bro
float4 main(float4 inpos : SV_POSITION,
			float2 inuv : TEXCOORD,
			float4 inetc : ETC) : SV_TARGET  {	
	float4 col = in_texture.Sample(samLinear, inuv.xy);
	col.w *= inetc.x;

	return col;
}