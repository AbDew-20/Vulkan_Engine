struct FSInput
{
    [[vk::location(0)]] float3 color : COLOR0;
    [[vk::location(1)]] float2 Tex : TEXCOORD0;
    [[vk::location(2)]] float3 Normal : NORMAL0;
    [[vk::location(3)]] uint Instance : BLENDINDICES0;
    [[vk::location(4)]] float3 FragPos : TEXCOORD1;
};

struct MeshPushConstants
{
    float4 diffuseColor;
    float3 lightDirection;
};
[[vk::push_constant]] MeshPushConstants pc;

SamplerState samplerColor[] : register(s1);
Texture2D<float4> texturecolor[] : register(t1);

struct Light
{
    float3 lightPos;
    float3 lightColor;
    float lightIntensity;
};

cbuffer light : register(b2)
{
    Light light;
}

float4 main(FSInput input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color = {0.3f,0.3f,0.3f,1.0f };
    textureColor = float4(texturecolor[input.Instance].Sample(samplerColor[input.Instance], input.Tex).rgb, 1.0);
    lightDir = normalize(light.lightPos - input.FragPos);
    lightIntensity = saturate(dot(lightDir, input.Normal)*light.lightIntensity);
    color += float4(light.lightColor, 1.0f) * lightIntensity;
    color = saturate(color);
    color = color * textureColor;
    //color = textureColor;
    
    return color;
}