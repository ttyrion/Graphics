  cbuffer MatrixBuffer
  {
      //matrix world;
      //matrix view;
      //matrix projection;
      matrix mvp;
  };
  
  struct VertexShader_INPUT
  {
      float3 position : POSITIONTEST1;
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
  
  SamplerState sample_state;
  Texture2D tex_y;
  Texture2D tex_u;
  Texture2D tex_v;
  float4 I420FramePixelShader(PixelShader_INPUT input) : SV_TARGET
  {
      float y = tex_y.Sample(sample_state, input.tex).r;
      float u = tex_u.Sample(sample_state, input.tex).r - 0.5f;
      float v = tex_v.Sample(sample_state, input.tex).r - 0.5f;
      float r = y + 1.14f * v;
      float g = y - 0.394f * u - 0.581f * v;
      float b = y + 2.03f * u;
  
      //return float4(input.color.r, input.color.g, input.color.b, 1.0f);
      return float4(r, g, b, 0.0f);
  }