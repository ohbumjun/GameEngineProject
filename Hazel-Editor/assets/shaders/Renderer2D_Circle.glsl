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
    // x,y 가 - 1 혹은 1 의 값을 지니게 된다.
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
// ShaderToy -> Soft Circle 의 uv 를 대체하기 위한 변수
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
	Input.LocalPosition : center 기준 각 pixel 의 상대적 위치
	length(Input.LocalPosition) : center 로 부터의 길이 계산
	1.0 - length(Input.LocalPosition) : nomalized distance 를 계산한다.
	0 은 center, 1 은 outer edge 를 의미하게 된다.
	*/
    float distance = 1.0 - length(Input.LocalPosition);

	/*
	smoothstep(a, b, x) : x 가 a 보다 작으면 0을 리턴 하고
	x 가 b 보다 크면 1을 리턴. a, b 사이의 smooth interpolation 을 시도
	중앙에서 거리가 멀어질 수록 Fade 되게 하는 효과
	1번째 smoothstep 은 0 (center) 에서 outer edge 로 갈 수록
	fade effect 를 주고자 하는 것으로 보인다. 

	리턴값의 범위는 0.0 ~ Input.Fade 이다.
	즉, circle 중심으로부터 거리가 0.0 ~ Input.Fade 이내일 경우네는
	circleAlpha 값이 0 에서 1 사이로 점차 증가한다는 것이다.

	Input.Fade 보다 큰 distance 는 1 값이 된다.
	*/
    float circleAlpha = smoothstep(0.0, Input.Fade, distance);

	/*
	ThickNess 에 근거한 추가적인 transparency 처리를 해주는 것으로 보인다.

	smoothStep  의 range 는 Input.Thickness + Input.Fade 에서
	Input.Thickness 사이의 값이다.

	Input.Thickness + Input.Fade 는 outer boundary of circle 로 보이고
	Input.thickness 는 실제 circle 의 thickness 이다.

	즉, circle 의 특정 thickness 범위 안에 있는 pixel 만 alpha 가 adjust
	될 수 있도록 하는 것이다.
	*/
    circleAlpha *= smoothstep(Input.Thickness + Input.Fade, 
			Input.Thickness, distance);

	/*
	circle 밖에 있는 대상은 그리지 않기 위한 일종의 optimization 으로 보인다.
	circle 밖 혹은, Circle 내부 빈 공간이 있을 경우
	해당 Pixel 은 최종 Fragment 에 기록하지 않는 것이다.
	*/
	if (circleAlpha == 0.0)
		discard;

    // Set output color
    o_Color = Input.Color;
	o_Color.a *= circleAlpha;

	o_EntityID = v_EntityID;
}