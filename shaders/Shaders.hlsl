struct VSInput
{
    float2 position : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
};

struct FSInput
{
    float4 pos : SV_Position;
    float3 color : COLOR;
};

struct CameraData
{
    float4x4 projectionView;
};

ConstantBuffer<CameraData> camera : register(b0, space0);

struct PushConstants
{
    float4x4 model;
};

[[vk::push_constant]]
ConstantBuffer<PushConstants> pc;

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    float4 localPosition = float4(input.position, 0.0, 1.0);

    float4 worldPosition = mul(localPosition, pc.model);
    output.pos = mul(worldPosition, camera.projectionView);
    output.color = input.color;

    return output;
}

[shader("fragment")]
float4 fragmentMain(FSInput input) : SV_Target
{
    return float4(input.color, 1.0);
}
