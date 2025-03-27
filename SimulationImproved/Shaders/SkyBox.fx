cbuffer InstanceBuffer : register(b1)
{
    matrix projection;
}

cbuffer PerFrameBuffer : register(b2)
{
    matrix view;
    float4 eyePos;
}

Texture2D txColour : register(t0);

SamplerState txSampler : register(s0);

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 pos = input.Pos;
    pos.xyz += eyePos.xyz;
    pos = mul(pos, view);
	pos = mul(pos, projection);
    output.Pos = pos;
    output.TexCoord = -input.TexCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return txColour.Sample(txSampler, input.TexCoord);
}