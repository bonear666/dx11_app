cbuffer MatrixesConstantBuffer : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float4x4 rotationAroundAxis;
};

cbuffer angleConstantBuffer : register(b1)
{
    float angle;
    float angle1;
    float angle2;
    float angle3;
};

struct VS_OUTPUT
{
    float4 outPos : SV_POSITION;
    float4 outColor : COLOR;
};

VS_OUTPUT main(float4 pos : POSITION, float4 color : COLOR)
{
    float4 newPos = pos;
    
    // умножать на world matrix не нужно
    newPos = mul(newPos, view);
    newPos = mul(newPos, projection);

    VS_OUTPUT output;
    output.outPos = newPos;
    output.outColor = color;
    return output;
}