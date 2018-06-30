struct VS_INPUT
{
    float3 vPosition : POSITION;
    float  v_rhw : NORMAL;
    float4 vColor : COLOR;
    float2 vTexture : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

VS_OUTPUT DXViewerVertexShader(VS_INPUT Input)
{
    VS_OUTPUT Output;
    Output.vPosition.x = Input.vPosition.x;
    Output.vPosition.y = Input.vPosition.y;
    Output.vPosition.z = Input.vPosition.z;
    Output.vPosition.w = 1;
    Output.vColor = Input.vColor;
    return Output;
}

float4 DXViewerPixelShader(VS_OUTPUT Input) : SV_Target
{
    return float4(Input.vColor.r, Input.vColor.g, Input.vColor.b, 1.0);
}