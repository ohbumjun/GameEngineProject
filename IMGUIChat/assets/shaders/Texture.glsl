#type vertex
#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;

void main()
{
	v_Color      = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;

	// 단일 DrawCall
	// gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
}

#type fragment
#version 330 core
			
layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;
			
uniform sampler2D u_Textures[32];

void main()
{
	// TODO : m_TilingFactor to Batch
	// v_TexIndex 정보도 제대로 넘어온다. 그러면 Texture 가 Binding 이 안된다는 것이다.
	// 아니면, u_Textures[32] 가 제대로 넘어오지 않거나..

    // Texture 가 없는 경우 : 기본 white texture * u_Color 형태로 구현
	// 구체적으로는 Texture 가 없는 경우 texIndex 가 0 이 되고, 0번째 slot 에 Binding 된 것이 WhiteTexture
	// color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
}