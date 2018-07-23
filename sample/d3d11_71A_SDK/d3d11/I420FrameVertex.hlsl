cbuffer MatrixBuffer
{
    //matrix world;
    //matrix view;
    //matrix projection;
    matrix mvp;
};

struct VertexShader_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 tex      : TEXCOORD0;
};

struct PixelShader_INPUT
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 tex    : TEXCOORD0;
};

PixelShader_INPUT I420FrameVertexShader(VertexShader_INPUT input)
{
    PixelShader_INPUT output;
    output.position.w = 1;
    output.position.x = input.position.x;
    output.position.y = input.position.y;
    output.position.z = input.position.z;

    //output.position = mul(output.position, world);
    //output.position = mul(output.position, view);
    //output.position = mul(output.position, projection);
    output.position = mul(output.position, mvp);

    output.color = input.color;
    output.tex = input.tex;
     
    return output;
} 