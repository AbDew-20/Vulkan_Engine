struct VSInput
{
    [[vk::location(0)]] float2 Pos : POSITION0;
    [[vk::location(1)]] float3 Color : COlOR0;
};

struct VSOutput
{
    float4 Pos : SV_Position;
    [[vk::location(0)]] float3 Color : COLOR0;
};
struct MeshPushConstants
{
    float2 data;
};
[[vk::push_constant]] MeshPushConstants pushConstants;


   VSOutput main(VSInput input) 
{   
    VSOutput output = (VSOutput) 0;
    output.Color = input.Color;
    output.Pos = float4(input.Pos+pushConstants.data, 0.0, 1.0);
    return output;
}