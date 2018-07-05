struct SIMPLE_VS_INPUT
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
};

struct SIMPLE_VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

SIMPLE_VS_OUTPUT DXViewerSimpleVertexShader(SIMPLE_VS_INPUT input)
{
    SIMPLE_VS_OUTPUT output;
    output.vPosition.x = input.vPosition.x;
    output.vPosition.y = input.vPosition.y;
    output.vPosition.z = input.vPosition.z;
    output.vPosition.w = 1;
    output.vColor = input.vColor;
    return output;
}

float4 DXViewerSimplePixelShader(SIMPLE_VS_OUTPUT input) : SV_Target
{
    return float4(input.vColor.r, input.vColor.g, input.vColor.b, 1.0);
}

struct VIDEO_VS_INPUT
{
    float3 vPosition : POSITION;
    float2 vTex : TEXCOORD0;
};

struct VIDEO_VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float2 vTex : TEXCOORD0;
};

//vertex shader  
cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

VIDEO_VS_OUTPUT DXViewerVideoVertexShader(VIDEO_VS_INPUT input)
{
    VIDEO_VS_OUTPUT output;
    output.vPosition.x = input.vPosition.x;
    output.vPosition.y = input.vPosition.y;
    output.vPosition.z = input.vPosition.z;
    output.vPosition.w = 1;  //Just to change the position vector to be 4 units, for proper matrix calculations.

    output.vPosition = mul(output.vPosition, world);
    output.vPosition = mul(output.vPosition, view);
    output.vPosition = mul(output.vPosition, projection);
    
    //Store the texture coordinates for the pixel shader
    output.vTex = input.vTex;

    return output;
}

//pixel shader  
Texture2D u_texY;
Texture2D u_texU;
Texture2D u_texV;
SamplerState SampleType;
float4 DXViewerVideoPixelShader(VIDEO_VS_OUTPUT input) : SV_TARGET
{
    //Sample the y or u or v from the texture using the sampler at this texture coordinate location
    float y = u_texY.Sample(SampleType, input.vTex).r;
    float u = u_texU.Sample(SampleType, input.vTex).r - 0.5f;
    float v = u_texV.Sample(SampleType, input.vTex).r - 0.5f;
    float r = y + 1.14f * v;
    float g = y - 0.394f * u - 0.581f * v;
    float b = y + 2.03f * u;
    return float4(r,g,b, 1.0f);
}