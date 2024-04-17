struct FSInput
{
    [[vk::location(0)]] float3 color : COLOR0;
    [[vk::location(1)]] float2 Tex : TEXCOORD0;
};


SamplerState samplerColor : register(s1);
Texture2D<float3> texturecolor : register(t1);

float4 main(FSInput input) : SV_TARGET
{   
    return float4(texturecolor.Sample(samplerColor, input.Tex),1.0f);
}