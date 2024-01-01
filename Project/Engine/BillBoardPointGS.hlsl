#include "global.hlsli"

struct GS_IN
{
    float4 pos : POSITION;
};

struct PS_IN
{
    float4 pos : SV_POSITION; 
    float2 texCoord : TEXCOORD; 
};

[maxvertexcount(4)]
void main(point GS_IN input[1], uint primID : SV_PrimitiveID,
                              inout TriangleStream<PS_IN> outputStream)
{
    // ���μ��� 100
    float scale = 100.f;    
    float hw = 0.5 * scale;
    
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(g_eyeWorld, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);
    
    PS_IN output;

    // LB
    output.pos = input[0].pos - hw * right - hw * up;
    output.pos = mul(output.pos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.texCoord = float2(0.0, 1.0);
    outputStream.Append(output);

    // LT
    output.pos = input[0].pos - hw * right + hw * up;
    output.pos = mul(output.pos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.texCoord = float2(0.0, 0.0);
    outputStream.Append(output);
    
    // RB
    output.pos = input[0].pos + hw * right - hw * up;
    output.pos = mul(output.pos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.texCoord = float2(1.0, 1.0);
    outputStream.Append(output);
    
    // RT
    output.pos = input[0].pos + hw * right + hw * up;
    output.pos = mul(output.pos, g_matView);
    output.pos = mul(output.pos, g_matProj);
    output.texCoord = float2(1.0, 0.0);
    outputStream.Append(output);
}