#include "mcb.hlsli"

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

static const float4 ambientLight = { 0.5f, 0.5f, 0.5f, 0.0f };

PS_OUTPUT main(VS_OUT input)
{
    
    //PS_OUTPUT output;
    //float3 light_dir = light_pos - input.world_pos.xyz;
    //float sq_distance = dot(light_dir, light_dir);
    
    //light_dir = light_dir / sqrt(sq_distance);
    //float3 eye_dir = input.eye_pos.xyz - input.world_pos.xyz;
    //float sq_distance_eye = dot(eye_dir, eye_dir);
    //float distance_eye = sqrt(sq_distance_eye);
    //eye_dir = eye_dir / distance_eye;
    //float3 norm = normalize(input.normal.xyz);
    //float nl = dot(norm, light_dir);
    //float diffuse_intensity = saturate(nl);
    //float3 half_vector = normalize(light_dir + eye_dir);
    //float nh = dot(norm, half_vector);
    //float specular_intensity = pow(saturate(nh), 1 + surface_shininess);
    //float4 light_intensity = float4(light_color, 0.0f) * light_power / sq_distance;
    //float4 mat_diffuse = tx_diffuse.Sample(samLinear, input.uv); // *float4(surface_diffuse, 0.0f) * surface_diffuse_factor;
    //float4 mat_specular = tx_specular.Sample(samLinear, input.uv); // *float4(surface_specular, 0.0f) * surface_specular_factor;
    //float4 mat_emissive = tx_emissive.Sample(samLinear, input.uv); // *float4(surface_emissive, 0.0f) * surface_emissive_factor;
    //float4 emissive = mat_emissive;
    //float4 ambient = mat_diffuse * ambient_light;
    //float4 specular = mat_specular * specular_intensity * light_intensity;
    //float4 diffuse = mat_diffuse * diffuse_intensity * light_intensity;
    //// hacky conservation of energy
    //diffuse.xyz -= specular.xyz;
    //diffuse.xyz = saturate(diffuse.xyz);
    //float4 color = ambient + specular + diffuse + emissive;
        
    //output.color = color;
    //return output;
    
    //need in constant buffer
    //point light color as float4
    //point light pos as float3
    //surface shininess as float
    //point light power as float
    
    PS_OUTPUT output = (PS_OUTPUT)0;
    float3 lightDir = lightPos - input.world_pos.xyz;
    float sqDist = dot(lightDir, lightDir);
    
    lightDir = lightDir / sqrt(sqDist);
    float3 eyeDir = input.eye_pos.xyz - input.world_pos.xyz;
    float sqDistEye = dot(eyeDir, eyeDir);
    float distEye = sqrt(sqDistEye);
    eyeDir = eyeDir / distEye;
    float3 norm = normalize(input.nrm.xyz);
    float nl = dot(norm, lightDir);
    float diffuseIntensity = saturate(nl);
    float3 halfVector = normalize(lightDir + eyeDir);
    float nh = dot(norm, halfVector);
    float specularIntensity = pow(saturate(nh), 1 + surfaceShininess);
    float4 lightIntensity = float4(lightColor.xyz, 0.0f) * lightPower / sqDist;
    float4 matDiffuse = tx_diffuse.Sample(samLinear, input.uv);
    float4 matSpecular = tx_specular.Sample(samLinear, input.uv);
    float4 emissive = tx_emissive.Sample(samLinear, input.uv);
    float4 ambient = matDiffuse * ambientLight;
    float4 specular = matSpecular * specularIntensity * lightIntensity;
    float4 diffuse = matDiffuse * diffuseIntensity * lightIntensity;
    
    diffuse.xyz -= specular.xyz;
    diffuse.xyz = saturate(diffuse.xyz);
    float4 color = ambient + specular + diffuse + emissive;
    
    output.color = color;
	return output;
}