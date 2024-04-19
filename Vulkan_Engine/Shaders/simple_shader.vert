struct VSInput
{
    [[vk::location(0)]] float2 Pos : POSITION0;
    [[vk::location(1)]] float3 Color : COlOR0;
    [[vk::location(2)]] float2 Tex : TEXCOORD0;
    [[vk::location(3)]] float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 Pos : SV_Position;
    [[vk::location(0)]] float3 Color : COLOR0;
    [[vk::location(1)]] float2 Tex : TEXCOORD0;
    [[vk::location(2)]] float3 Normal : NORMAL0;
};

struct UBO
{
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 projMatrix;
};
cbuffer ubo : register(b0)
{
    UBO ubo;
}


   VSOutput main(VSInput input,uint instance:SV_InstanceID)
{   
    VSOutput output = (VSOutput) 0;
    output.Color = input.Color;
    output.Pos = mul(ubo.projMatrix,mul(ubo.viewMatrix, mul(ubo.modelMatrix, float4(input.Pos, 0.0, 1.0))));
    output.Tex = input.Tex;
    output.Normal = mul((float3x3)ubo.modelMatrix, input.Normal );
    output.Normal = normalize(output.Normal);
    return output;
}