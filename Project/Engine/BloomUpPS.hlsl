#include "global.hlsli"
#include "struct.hlsli"

#define TextureWidth g_float_0 
#define TextureHeight g_float_1 
#define FilterRadius g_float_2 

float4 main(PS_IN input) : SV_TARGET
{
    float x = input.vUV0.x;
    float y = input.vUV0.y;
    
    float dx = FilterRadius / TextureWidth;
    float dy = FilterRadius / TextureHeight;
    
    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    float3 a = g_tex_0.Sample(g_LinearClampSampler, float2(x - dx, y + dy)).rgb;
    float3 b = g_tex_0.Sample(g_LinearClampSampler, float2(x, y + dy)).rgb;
    float3 c = g_tex_0.Sample(g_LinearClampSampler, float2(x + dx, y + dy)).rgb;
    
    float3 d = g_tex_0.Sample(g_LinearClampSampler, float2(x - dx, y)).rgb;
    float3 e = g_tex_0.Sample(g_LinearClampSampler, float2(x, y)).rgb;
    float3 f = g_tex_0.Sample(g_LinearClampSampler, float2(x + dx, y)).rgb;
    
    float3 g = g_tex_0.Sample(g_LinearClampSampler, float2(x - dx, y - dy)).rgb;
    float3 h = g_tex_0.Sample(g_LinearClampSampler, float2(x, y - dy)).rgb;
    float3 i = g_tex_0.Sample(g_LinearClampSampler, float2(x + dx, y - dy)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5
    // This shows 5 square areas that are being sampled. But some of them overlap,
    // so to have an energy preserving downsample we need to make some adjustments.
    // The weights are the distributed, so that the sum of j,k,l,m (e.g.)
    // contribute 0.5 to the final color output. The code below is written
    // to effectively yield this sum. We get:
    // 0.125*5 + 0.03125*4 + 0.0625*4 = 1
    float3 upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;
    
    return float4(upsample, 1.f);
}
