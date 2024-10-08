#include "struct.hlsli"
#include "global.hlsli"
#include "func.hlsli"

#define FadeCoef g_float_0
#define FarZ g_float_1

float4 main(PS_IN _in) : SV_Target
{
    float3 color = g_postprocess_Tex.Sample(g_LinearClampSampler, _in.vUV0).rgb;

    float DepthMaskingViewZ = TexcoordToView(DepthMaskingTex, _in.vUV0).z;
    DepthMaskingViewZ /= FarZ;
    
    float bias = 0.1f;
    
    // 마스킹 되어있지않은 부분은 원본 색상으로 렌더링
    if (1.f - DepthMaskingViewZ < bias)
    {
        color = color * FadeCoef;
    }
   
    return float4(color, 1.f);
}