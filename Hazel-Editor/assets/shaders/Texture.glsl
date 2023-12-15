#type vertex
#version 450 
			
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
#version 450
			
// 참고 : OpenGLFrameBuffer 에서 bind 한 TexImage2D 들이 여기에 해당한다.
// 첫번째 color buffer == GL_COLOR_ATTACHMENT0
layout(location = 0) out vec4 color;

// 두번째 color buffer == GL_COLOR_ATTACHMENT0 + 1
// mouse picking 을 통해 해당 pixel 이 어떤 entity 에 속하는지를 
// 판별하는 frame buffer 로 활용하고 있으므로 out vec4 가 아니라, int 가 된다.
layout(location = 1) out int color2;

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
	
	vec4 texColor = v_Color;
	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], v_TexCoord * v_TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], v_TexCoord * v_TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], v_TexCoord * v_TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], v_TexCoord * v_TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], v_TexCoord * v_TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], v_TexCoord * v_TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], v_TexCoord * v_TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], v_TexCoord * v_TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], v_TexCoord * v_TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], v_TexCoord * v_TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], v_TexCoord * v_TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], v_TexCoord * v_TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], v_TexCoord * v_TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], v_TexCoord * v_TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], v_TexCoord * v_TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], v_TexCoord * v_TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], v_TexCoord * v_TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], v_TexCoord * v_TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], v_TexCoord * v_TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], v_TexCoord * v_TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], v_TexCoord * v_TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], v_TexCoord * v_TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], v_TexCoord * v_TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], v_TexCoord * v_TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], v_TexCoord * v_TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], v_TexCoord * v_TilingFactor); break;
	}
	color = texColor;

	color2 = 50; // placeholder for our entity ID
}