struct Light
{
    float3 mPosition;
    float3 mDirection;
    float4 mColourType;
};

cbuffer ObjectBuffer : register(b0)
{
    matrix world;
}

cbuffer InstanceBuffer : register(b1)
{
    matrix projection;
}

cbuffer PerFrameBuffer : register(b2)
{
    Light lights[2];
    matrix view;
    float4 eyePos;
    int lightSetting;
}

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
    float3 Normal : NORMAL;
    float3 PosWorld : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(input.Pos, world);
    output.PosWorld = output.Pos.xyz;

    output.Pos = mul(output.Pos, view);
    output.Pos = mul(output.Pos, projection);

    output.Normal = normalize(mul(input.Normal, (float3x3)world));

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float3 DirectionalLight(float3 N, float3 viewDir, Light light)
{
    float3 sunLightDir = normalize(-light.mDirection);
    float sunDiffuse = max(0.0, dot(N, sunLightDir));

    float3 halfVector = normalize(sunLightDir + viewDir); // Halfway vector
    float NdotH = saturate(dot(N, halfVector)); // Specular intensity
    float specularFactor = pow(NdotH, 32); // Specular power

    return (light.mColourType.rgb * ((sunDiffuse * 0.9) + (specularFactor * 0.3)));
}

float3 SpotLight(float3 N, float3 viewDir, float3 posWorld, Light light)
{

    float3 torchDir = normalize(-light.mDirection); // Spotlight direction
    float3 toFrag = posWorld - light.mPosition; // Vector from torch to fragment
    float distance = length(toFrag); // Distance from torch to fragment
    toFrag = normalize(toFrag);

    // Diffuse lighting
    float torchDiffuse = max(0.0, dot(N, -toFrag)); // Light coming from the torch's direction
    torchDiffuse = floor(torchDiffuse * 4.0) / 4.0;

    float3 halfVector = normalize(torchDir + viewDir); // Halfway vector
    float NdotH = saturate(dot(N, halfVector)); // Specular intensity
    float shininess = 32.0f; // Material shininess
    float specularFactor = pow(NdotH, shininess); // Specular power

    // Spotlight effect (cone angle)
    float theta = dot(torchDir, -toFrag); // Angle between spotlight direction and toFrag
    float spotEffect = smoothstep(0.7 - 0.1, 0.7, theta);

    // Attenuation based on distance
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.05 * distance * distance);

    // Combine effects 
    return (light.mColourType.rgb * ((torchDiffuse * 0.9 + specularFactor * 0.3) * spotEffect * attenuation));
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 viewDir = normalize(eyePos - input.PosWorld).xyz;

    float4 finalLight = float4(0.1, 0.1, 0.1, 1.0);
    float3 N = input.Normal;

    if (lightSetting == 0)
    {
        finalLight.rgb += DirectionalLight(N, viewDir, lights[0]);
    }
    else if (lightSetting == 1)
    {
        finalLight.rgb += SpotLight(N, viewDir, input.PosWorld, lights[1]);
    }

    return floor(finalLight * 3) / 3;
}
