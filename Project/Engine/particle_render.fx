#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "global.hlsli"
#include "struct.hlsli"

StructuredBuffer<tParticle> g_ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule> g_ParticleModule : register(t21);

#define Particle g_ParticleBuffer[_in.iInstID]

#define SPAWN_MODULE 0
#define DRAG_MODULE 1
#define SCALE_MODULE 2
#define ADD_VELOCITY_MODULE 3
#define SPIRAL_VELOCITY_MODULE 4
#define NOISE_FORCE_MODULE 5
#define CALCULATE_FORCE_MODULE 6
#define RENDER_MODULE 7

struct VS_Input
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

struct VS_Output
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

VS_Output VS_ParticleRender(VS_Input _in)
{
    VS_Output output = (VS_Output) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.iInstID = _in.iInstID;
    
    return output;
}

// Geometry Shader
// 정점을 생성시킬 수 있는 쉐이더
// 1. 담당 파티클이 비활성화 상태인 경우, 렌더링을 정점연산 단계에서 중단시키기
// 2. 빌보드 구현의 편의성
struct GS_Output
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

[maxvertexcount(12)]
void GS_ParticleRender(point VS_Output _in[1], inout TriangleStream<GS_Output> _OutStream)
{
    GS_Output output[4] = { (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f };
    GS_Output output_cross[4] = { (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f, (GS_Output) 0.f };
        
    // GS 가 담당하는 파티클 정보를 가져온다.    
    tParticle particle = g_ParticleBuffer[_in[0].iInstID];
    if (0 == particle.Active)
    {
        return;
    }
    
    // 파티클의 중심점을 월드로 이동    
    float3 vWorldPos = particle.vWorldPos.xyz;
    
    // 파티클의 ViewSpace 상에서의 중심 포지션 구하기
    float4 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
    
    // 0 -- 1	     
	// | \  |	     
	// 3 -- 2
    output[0].vPosition = float4((particle.vWorldScale.x * -0.5f), (particle.vWorldScale.y * 0.5f), 0.f, 1.f);
    output[1].vPosition = float4((particle.vWorldScale.x * 0.5f), (particle.vWorldScale.y * 0.5f), 0.f, 1.f);
    output[2].vPosition = float4((particle.vWorldScale.x * 0.5f), (particle.vWorldScale.y * -0.5f), 0.f, 1.f);
    output[3].vPosition = float4((particle.vWorldScale.x * -0.5f), (particle.vWorldScale.y * -0.5f), 0.f, 1.f);
          
    output_cross[0].vPosition = float4((particle.vWorldScale.x * -0.5f), 0.f, (particle.vWorldScale.y * 0.5f), 1.f);
    output_cross[1].vPosition = float4((particle.vWorldScale.x * 0.5f), 0.f, (particle.vWorldScale.y * 0.5f), 1.f);
    output_cross[2].vPosition = float4((particle.vWorldScale.x * 0.5f), 0.f, (particle.vWorldScale.y * -0.5f), 1.f);
    output_cross[3].vPosition = float4((particle.vWorldScale.x * -0.5f), 0.f, (particle.vWorldScale.y * -0.5f), 1.f);
    
    output_cross[0].vUV = output[0].vUV = float2(0.f, 0.f);
    output_cross[1].vUV = output[1].vUV = float2(1.f, 0.f);
    output_cross[2].vUV = output[2].vUV = float2(1.f, 1.f);
    output_cross[3].vUV = output[3].vUV = float2(0.f, 1.f);
       
    
    // 렌더모듈 기능
    if (g_ParticleModule[0].arrModuleCheck[RENDER_MODULE])
    {
        // 속도에 따른 정렬 기능
        if (0 != g_ParticleModule[0].VelocityAlignment)
        {
            float3 vR = normalize(mul(float4(particle.vVelocity.xyz, 0.f), g_matView).xyz);
            float3 vF = normalize(cross(vR, float3(0.f, 1.f, 0.f)));
            float3 vU = normalize(cross(vF, vR));
            
            float3x3 vRot =
            {
                vR,
                vU,
                vF
            };
            
            for (int i = 0; i < 4; ++i)
            {
                output[i].vPosition.xyz = mul(output[i].vPosition.xyz, vRot);
                output_cross[i].vPosition.xyz = mul(output_cross[i].vPosition.xyz, vRot);
            }
        }
    }
    
    // View 좌표로 이동, 투영행렬 적용
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition.xyz += vViewPos.xyz;
        output[i].vPosition = mul(output[i].vPosition, g_matProj);
        
        output_cross[i].vPosition.xyz += vViewPos.xyz;
        output_cross[i].vPosition = mul(output_cross[i].vPosition, g_matProj);
        
        output[i].iInstID = _in[0].iInstID;
        output_cross[i].iInstID = _in[0].iInstID;
    }
      
    _OutStream.Append(output[0]);
    _OutStream.Append(output[2]);
    _OutStream.Append(output[3]);
    _OutStream.RestartStrip();
    
    _OutStream.Append(output[0]);
    _OutStream.Append(output[1]);
    _OutStream.Append(output[2]);
    _OutStream.RestartStrip();
        
    if (g_ParticleModule[0].arrModuleCheck[RENDER_MODULE])
    {
        // Cross Mesh 
        if (2 == g_ParticleModule[0].VelocityAlignment)
        {
            _OutStream.Append(output_cross[0]);
            _OutStream.Append(output_cross[2]);
            _OutStream.Append(output_cross[3]);
            _OutStream.RestartStrip();
    
            _OutStream.Append(output_cross[0]);
            _OutStream.Append(output_cross[1]);
            _OutStream.Append(output_cross[2]);
            _OutStream.RestartStrip();
        }
    }
}

// ==================================================
// Render
// ==================================================

float4 PS_ParticleRender(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // 출력 색상
    float4 vOutColor = particle.vColor;
    vOutColor.a = 1.f;
    
    if (g_btex_0)
    {
        float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
        vOutColor.rgb *= vSampleColor.rgb;
        vOutColor.a = vSampleColor.a;
    }
    
    // 렌더모듈이 켜져 있으면
    if (module.arrModuleCheck[RENDER_MODULE])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vOutColor;
}

#define GlowEnable g_int_0
#define GlowThreshold g_float_0
#define GlowColor g_vec4_0

struct PS_Std2D_Glow_Output
{
    float4 RenderTargetColor : SV_Target0;
    float4 BloomTextureColor : SV_Target1;
};

PS_Std2D_Glow_Output PS_ParticleRender_Glow(GS_Output _in)
{
    PS_Std2D_Glow_Output output;
    
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // 출력 색상
    float4 vOutColor = particle.vColor;
    vOutColor.a = 1.f;
    
    if (g_UseAnim2D)
    {
        float2 vUV = float2(0.0, 0.0);
        
        if (g_UseBackGround)
        {
            float2 vBackgroundLeftTop = g_vLeftTop + (g_vSliceSize / 2.f) - (g_vBackGround / 2.f);
            vBackgroundLeftTop -= g_vOffset;
            vUV = vBackgroundLeftTop + (g_vBackGround * _in.vUV);
        }
        else
        {
            float2 LT = g_vLeftTop - g_vOffset;
            vUV = LT + (g_vSliceSize * _in.vUV);
        }
        
        if (vUV.x < g_vLeftTop.x || (g_vLeftTop.x + g_vSliceSize.x) < vUV.x
            || vUV.y < g_vLeftTop.y || (g_vLeftTop.y + g_vSliceSize.y) < vUV.y)
        {
            discard;
        }
        else
        {
            float4 vSampleColor = g_anim2d_tex.Sample(g_LinearWrapSampler, vUV);
            vOutColor.rgb *= vSampleColor.rgb;
            vOutColor.a = vSampleColor.a;
        }
    }
    else
    {
        if (g_btex_0)
        {
            float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
            vOutColor.rgb *= vSampleColor.rgb;
            vOutColor.a = vSampleColor.a;
        }
        else
        {
            discard;
        }
    }
        
    if (0.1f >= vOutColor.a)
        discard;
    
    // Relative Luminance : 픽셀의 색이 밝은지 어두운지의 기준값
    float RelativeLuminance = dot(vOutColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
        
    // 일정 GlowThreshold 이상인 경우 지정된 색상으로 마스킹
    if (GlowEnable && RelativeLuminance > GlowThreshold)
    {
        output.BloomTextureColor = GlowColor;
    }
    else
    {
        output.BloomTextureColor = float4(0.f, 0.f, 0.f, 1.f);
    }
  
    // 렌더모듈이 켜져 있으면
    if (module.arrModuleCheck[RENDER_MODULE])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    output.RenderTargetColor = vOutColor;
    
    return output;
}

#define MaskColor g_int_0
#define LerpRatio g_float_1
#define InitFireColor g_vec4_1
#define EndFireColor g_vec4_2

#define MtrlAlbedo g_vAlbedo

float4 PS_ParticleRender_Fire(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // Age 에 따라 두 색을 보간하여 색상 결정
    float4 vOutColor = (float4) 0.f;
    vOutColor.rgb = lerp(InitFireColor.rgb, EndFireColor.rgb, pow(abs(particle.NormalizeAge), 1.f / LerpRatio));
    vOutColor.a = 1.f;
    
    // Texture 적용
    if (g_btex_0)
    {
        // 좌우 반전
        if (0 == _in.iInstID % 2)
        {
            _in.vUV.x = 1.f - _in.vUV.x;
        }
        
        float4 vSampleColor = g_tex_0.Sample(g_LinearWrapSampler, _in.vUV);
        vOutColor.a = vSampleColor.a;
        
        // base color의 검은부분 색 변경
        if (MaskColor)
        {
            float WhiteRatio = dot(vSampleColor.rgb, float3(1.f, 1.f, 1.f)) / 3.f;
            vSampleColor = vSampleColor * (WhiteRatio) + MtrlAlbedo * (1.f - WhiteRatio);
            
            if (vOutColor.a != 0.f)
            {
                vOutColor.a = vOutColor.a * WhiteRatio + MtrlAlbedo.a * (1.f - WhiteRatio);
            }
            
        }
        
        vOutColor.rgb *= vSampleColor.rgb;

    }

    // 렌더모듈이 켜져 있으면
    if (module.arrModuleCheck[RENDER_MODULE])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vOutColor;
}

float4 PS_ParticleRender_Distortion(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    float4 vColor = (float4) 0.f;
    vColor.a = 1.f;
        
    // 픽셀쉐이더에 SV_Position 으로 입력된 값은 픽셀 쉐이더를 호출한 해당 픽셀의 좌표가 들어있다.
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;
    
    //vScreenUV.y += cos((vScreenUV.x + (g_time * (속도) )) * (주파수)) * (진폭);
    //vScreenUV.y += cos((vScreenUV.x + (g_time *  0.1f))   *   40.f)  *  0.1f;
    
    if (g_btex_0)
    {
        float2 vUV = _in.vUV;
        vUV.x += g_Time * 0.1f;
        
        float2 vNoise = g_tex_0.Sample(g_LinearWrapSampler, vUV).rg;
        vNoise = (vNoise.xy - 0.5f) * 0.1f;
        
        vScreenUV += vNoise;
    }
        
    vColor = g_postprocess_Tex.Sample(g_LinearClampSampler, vScreenUV);
    
    // 렌더모듈이 켜져 있으면
    if (module.arrModuleCheck[RENDER_MODULE])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vColor;
}

#define WhiteMappingColor g_vec4_0
#define MappingTex g_tex_0

// USAGE : 흑백 텍스쳐의 흰색과 검은색을 각각 2개의 컬러로 매핑하는 셰이더
float4 PS_ParticleRender_ColorMap(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    float4 vOutColor = (float4) 0.f;
    
    // Color Mapping
    if (g_btex_0)
    {
        float4 vSampleColor = MappingTex.Sample(g_LinearWrapSampler, _in.vUV);
        float WhiteRatio = dot(vSampleColor.rgb, float3(1.f, 1.f, 1.f)) / 3.f;
        vOutColor = WhiteMappingColor * WhiteRatio + MtrlAlbedo * (1.f - WhiteRatio);
        vOutColor.a *= vSampleColor.a;
    }

    // 렌더모듈이 켜져 있으면
    if (module.arrModuleCheck[RENDER_MODULE])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a *= saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a *= saturate(1.f - clamp(fRatio, 0.f, 1.f));
        }
    }
    
    return vOutColor;
}

#endif