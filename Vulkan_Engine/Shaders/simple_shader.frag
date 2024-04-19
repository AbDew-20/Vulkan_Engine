struct FSInput
{
    [[vk::location(0)]] float3 color : COLOR0;
    [[vk::location(1)]] float2 Tex : TEXCOORD0;
    [[vk::location(2)]] float3 Normal : NORMAL0;
    [[vk::location(3)]] uint Instance : BLENDINDICES0;
};

struct MeshPushConstants
{
    float4 diffuseColor;
    float3 lightDirection;
};
[[vk::push_constant]] MeshPushConstants pc;

SamplerState samplerColor[] : register(s1);
Texture2D<float4> texturecolor[] : register(t1);

float4 main(FSInput input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    textureColor = float4(texturecolor[input.Instance].Sample(samplerColor[input.Instance], input.Tex).rgb, 1.0);
    lightDir = -pc.lightDirection;
    lightIntensity = saturate(dot(lightDir, input.Normal));
    color = saturate(pc.diffuseColor * lightIntensity);
    color = color * textureColor;
    
    return color;
}