#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "global.hlsli"
#include "struct.hlsli"

StructuredBuffer<tParticle> g_ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule> g_ParticleModule : register(t21);

#define Particle g_ParticleBuffer[_in.iInstID]

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
// ������ ������ų �� �ִ� ���̴�
// 1. ��� ��ƼŬ�� ��Ȱ��ȭ ������ ���, �������� �������� �ܰ迡�� �ߴܽ�Ű��
// 2. ������ ������ ���Ǽ�
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
        
    // GS �� ����ϴ� ��ƼŬ ������ �����´�.    
    tParticle particle = g_ParticleBuffer[_in[0].iInstID];
    if (0 == particle.Active)
    {
        return;
    }
    
    // ��ƼŬ�� �߽����� ����� �̵�    
    float3 vWorldPos = particle.vWorldPos.xyz;
    
    // ��ƼŬ�� ViewSpace �󿡼��� �߽� ������ ���ϱ�
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
       
    
    // ������� ���
    if (g_ParticleModule[0].arrModuleCheck[6])
    {
        // �ӵ��� ���� ���� ���
        if (g_ParticleModule[0].VelocityAlignment)
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
    
    // View ��ǥ�� �̵�, ������� ����
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
        
    if (g_ParticleModule[0].arrModuleCheck[6])
    {
        // �ӵ��� ���� ���� ���
        if (g_ParticleModule[0].VelocityAlignment)
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

float4 PS_ParticleRender(GS_Output _in) : SV_Target
{
    tParticle particle = g_ParticleBuffer[_in.iInstID];
    tParticleModule module = g_ParticleModule[0];
    
    // ��� ����
    float4 vOutColor = particle.vColor;
    
    if (g_btex_0)
    {
        float4 vSampleColor = g_tex_0.Sample(g_LinearSampler, _in.vUV);
        vOutColor.rgb *= vSampleColor.rgb;
        vOutColor.a = vSampleColor.a;
    }
    
    if (0.f >= vOutColor.a)
        discard;
    
    // ��������� ���� ������
    if (module.arrModuleCheck[6])
    {
        if (1 == module.AlphaBasedLife) // Normalize Age
        {
            vOutColor.a = saturate(1.f - clamp(particle.NormalizeAge, 0.f, 1.f));
        }
        else if (2 == module.AlphaBasedLife) // Max Age
        {
            float fRatio = particle.Age / module.AlphaMaxAge;
            vOutColor.a = saturate(1.f - clamp(fRatio, 0.f, 1.f));
            
        }
    }
    
    return vOutColor;
}

#endif