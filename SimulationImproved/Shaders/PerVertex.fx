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
    float4 Colour : TEXCOORD0;
};

float3 DirectionalLight(float4 posWorld, float3 normal, Light light)
{
    float3 viewDir = normalize(eyePos - posWorld); // View direction

    float3 sunLightDir = normalize(-light.mDirection);
    float sunDiffuse = max(0.0, dot(normal, sunLightDir));

    float3 halfVector = normalize(sunLightDir + viewDir); // Halfway vector
    float NdotH = saturate(dot(normal, halfVector)); // Specular intensity
    float shininess = 32.0f; // Material shininess
    float specularFactor = pow(NdotH, shininess); // Specular power

    return (light.mColourType.rgb * ((sunDiffuse * 0.9) + (specularFactor * 0.3)));
}

float3 SpotLight(float4 posWorld, float3 normal, Light light)
{
    float3 N = normalize(normal); // Normal
    float3 viewDir = normalize(eyePos - posWorld);

    float3 torchDir = normalize(-light.mDirection); // Spotlight direction
    float3 toFrag = posWorld - light.mPosition; // Vector from torch to fragment
    float distance = length(toFrag); // Distance from torch to fragment
    toFrag = normalize(toFrag);

    // Diffuse lighting
    float torchDiffuse = max(0.0, dot(N, -toFrag)); // Light coming from the torch's direction

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

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = mul(input.Pos, world);
    float3 normal = normalize(mul(input.Normal, (float3x3)world));

    float4 finalColor = float4(0.1f, 0.1f, 0.1f, 1.0f);

    if (lightSetting == 0)
    {
        finalColor.rgb += DirectionalLight(output.Pos, normal, lights[0]);
    }
    else if (lightSetting == 1)
    {
        finalColor.rgb += SpotLight(output.Pos, normal, lights[1]);
    }

	output.Pos = mul(output.Pos, view);
    output.Pos = mul(output.Pos, projection);
    output.Colour = finalColor;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    return input.Colour;
}