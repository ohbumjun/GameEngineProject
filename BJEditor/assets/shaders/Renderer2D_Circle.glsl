//--------------------------
// - Hazel 2D -
// Renderer2D Circle Shader
// --------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
    // x,y �� - 1 Ȥ�� 1 �� ���� ���ϰ� �ȴ�.
	Output.LocalPosition = a_LocalPosition;
	Output.Color = a_Color;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;

	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
// ShaderToy -> Soft Circle �� uv �� ��ü�ϱ� ���� ����
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

void main()
{	
	/*
    Calculate distance and fill circle with white
	Input.LocalPosition : center ���� �� pixel �� ����� ��ġ
	length(Input.LocalPosition) : center �� ������ ���� ���
	1.0 - length(Input.LocalPosition) : nomalized distance �� ����Ѵ�.
	0 �� center, 1 �� outer edge �� �ǹ��ϰ� �ȴ�.
	*/
    float distance = 1.0 - length(Input.LocalPosition);

	/*
	smoothstep(a, b, x) : x �� a ���� ������ 0�� ���� �ϰ�
	x �� b ���� ũ�� 1�� ����. a, b ������ smooth interpolation �� �õ�
	�߾ӿ��� �Ÿ��� �־��� ���� Fade �ǰ� �ϴ� ȿ��
	1��° smoothstep �� 0 (center) ���� outer edge �� �� ����
	fade effect �� �ְ��� �ϴ� ������ ���δ�. 

	���ϰ��� ������ 0.0 ~ Input.Fade �̴�.
	��, circle �߽����κ��� �Ÿ��� 0.0 ~ Input.Fade �̳��� ���״�
	circleAlpha ���� 0 ���� 1 ���̷� ���� �����Ѵٴ� ���̴�.

	Input.Fade ���� ū distance �� 1 ���� �ȴ�.
	*/
    float circleAlpha = smoothstep(0.0, Input.Fade, distance);

	/*
	ThickNess �� �ٰ��� �߰����� transparency ó���� ���ִ� ������ ���δ�.

	smoothStep  �� range �� Input.Thickness + Input.Fade ����
	Input.Thickness ������ ���̴�.

	Input.Thickness + Input.Fade �� outer boundary of circle �� ���̰�
	Input.thickness �� ���� circle �� thickness �̴�.

	��, circle �� Ư�� thickness ���� �ȿ� �ִ� pixel �� alpha �� adjust
	�� �� �ֵ��� �ϴ� ���̴�.
	*/
    circleAlpha *= smoothstep(Input.Thickness + Input.Fade, 
			Input.Thickness, distance);

	/*
	circle �ۿ� �ִ� ����� �׸��� �ʱ� ���� ������ optimization ���� ���δ�.
	circle �� Ȥ��, Circle ���� �� ������ ���� ���
	�ش� Pixel �� ���� Fragment �� ������� �ʴ� ���̴�.
	*/
	if (circleAlpha == 0.0)
		discard;

    // Set output color
    o_Color = Input.Color;
	o_Color.a *= circleAlpha;

	o_EntityID = v_EntityID;
}