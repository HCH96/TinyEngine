#include "struct.hlsli"
#include "global.hlsli"
#include "UnrealPBRCommon.hlsli"

#define AmbientTex g_tex_0
#define PositionTex g_tex_1
#define NormalTex g_tex_2
#define MRATex g_tex_3

#define LIGHT_INDEX g_int_0

float4 main(PS_IN input) : SV_Target
{
    float3 vWorldPos = PositionTex.Sample(g_LinearWrapSampler, input.vUV).xyz;
    
    // x,y,z 전부 0이라면 discard
    if (!any(vWorldPos))
    {
        discard;
    }

    float3 albedo = AmbientTex.Sample(g_LinearWrapSampler, input.vUV).rgb;
    float3 normalWorld = NormalTex.Sample(g_LinearWrapSampler, input.vUV).rgb;
    float metallic = MRATex.Sample(g_LinearWrapSampler, input.vUV).r;
    float roughness = MRATex.Sample(g_LinearWrapSampler, input.vUV).g;
    float ao = MRATex.Sample(g_LinearWrapSampler, input.vUV).b;
    if (ao >= 1.f)
    {
        ao = SSAOTex.Sample(g_LinearWrapSampler, input.vUV).r;
    }

    float3 directLighting = float3(0, 0, 0);

    DirectLighting(LIGHT_INDEX, vWorldPos, normalWorld, albedo, ao, metallic, roughness, directLighting);
    
    return float4(directLighting, 1.f);
}

