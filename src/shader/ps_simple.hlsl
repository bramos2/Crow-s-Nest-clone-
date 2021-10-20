// placeholder shader for when things aren't working

struct VSOut {
    float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

float4 main(VSOut input) : SV_Target {
	return float4(1.0f, 1.0f, 1.0f, 1.0f); 
}