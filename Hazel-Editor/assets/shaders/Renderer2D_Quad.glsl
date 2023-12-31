//--------------------------
// - Hazel 2D -
// Renderer2D Quad Shader
// --------------------------

#type vertex
#version 450 
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

// uniform mat4 u_ViewProjection;
// uniform mat4 u_Transform;
// GPU 에 모든 Material, Shader 에 동일하게 적용될
// 데이터를 메모리에 할당하게 한다.
// 그 메모리를 Uniform Buffer 라고 하는 것 같다.
// 각 Shader 에서는 해당 메모리에 접근하게 하는 방식으로
// 진행하게 한다.
// 이를 통해 각 Shader 마다 uniform storage 를
// 만들지 않아서 최적화를 진행할 수 있는 것으로 보인다.
// 그리고 binding 이라는 idx 형태로 접근할 수 있게 하는 것으로
// 보인다.
layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};


layout (location = 0) out VertexOutput Output;

// 16 byte 마다 location 값이 1 증가한다.
// VertexOutput 의 경우, padding 까지 다 해서 16  * 4
// 만큼의 크기를 차지하는 것으로 보인다.
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;
	Output.TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;

	// 단일 DrawCall
	// gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#type fragment
#version 450
			
// 참고 : OpenGLFrameBuffer 에서 bind 한 TexImage2D 들이 여기에 해당한다.
// 첫번째 color buffer == GL_COLOR_ATTACHMENT0
layout(location = 0) out vec4 o_Color;

// 두번째 color buffer == GL_COLOR_ATTACHMENT0 + 1
// mouse picking 을 통해 해당 pixel 이 어떤 entity 에 속하는지를 
// 판별하는 frame buffer 로 활용하고 있으므로 out vec4 가 아니라, int 가 된다.
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;
			
// uniform sampler2D u_Textures[32];
layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	// TODO : m_TilingFactor to Batch
	// v_TexIndex 정보도 제대로 넘어온다. 그러면 Texture 가 Binding 이 안된다는 것이다.
	// 아니면, u_Textures[32] 가 제대로 넘어오지 않거나..

    // Texture 가 없는 경우 : 기본 white texture * u_Color 형태로 구현
	// 구체적으로는 Texture 가 없는 경우 texIndex 가 0 이 되고, 0번째 slot 에 Binding 된 것이 WhiteTexture
	// color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
	
	vec4 texColor = Input.Color;

	switch(int(Input.TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], Input.TexCoord * Input.TilingFactor); break;	
	}
	
	if (texColor.a == 0.0)
		discard;

	o_Color = texColor;

	o_EntityID = v_EntityID; // placeholder for our entity ID
}