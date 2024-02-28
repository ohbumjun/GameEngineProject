#include "LvPrecompiled.h"
#include "engine/component/LvCameraComponent.h"

#include "system/LvReflection.h"

#include "engine/component/LvTransformComponent.h"
#include "engine/LvRenderTexture.h"
using namespace Lv;

LV_NS_ENGINE_BEGIN

LvCameraComponent::LvCameraComponent()
	: _projectionType(ProjectionType::PERSPECTIVE)
	, _renderType(RenderType::BASE)
	, _aspect(1.f)
	, _aaMode(AntiAliasingMode::NONE)
	, _nearP(0.3f)
	, _farP(1000.f)
	, _fovRadian(LvMath::Radians(45.f))
	, _orthoSize(5.f)
	, _depth(0.f)
	, _viewportRect{ 0.f, 0.f, 1.f, 1.f }
	, _backgroundColor(LvColorPreset::GRAY5.vec)
{
}

LvCameraComponent::~LvCameraComponent()
{
	_cameraStack.Clear();
	_baseCamera = nullptr;
}

float LvCameraComponent::GetFOVByDegree() const
{
	return LvMath::Degrees(_fovRadian);
}

void LvCameraComponent::SetFOVByDegree(float angle)
{
	SetFOVRadian(LvMath::Radians(angle));
}


void LvCameraComponent::Serialize(LvArchive& archive)
{
	archive.WriteStartObject(LvObject::_type, this);

	LV_WRITE(archive, _renderType);
	LV_WRITE(archive, _projectionType);
	LV_WRITE(archive, enabled);
	LV_WRITE(archive, _aspect);
	LV_WRITE(archive, _fovRadian);
	LV_WRITE(archive, _nearP);
	LV_WRITE(archive, _farP);
	uint32 cullingMask = static_cast<uint32>(_cullingMask);
	LV_WRITE(archive, cullingMask);
	LV_WRITE(archive, _orthoSize);
	LV_WRITE(archive, _depth);
	LV_WRITE(archive, _viewMatrix);
	LV_WRITE(archive, _projectionMatrix);
	LV_WRITE(archive, _projectionViewMatrix);
	LV_WRITE(archive, _frustum);
	uint32 clearFlag = static_cast<uint32>(_clearFlag);
	LV_WRITE(archive, clearFlag);
	archive.Write("ViewportRect X", _viewportRect.x);
	archive.Write("ViewportRect Y", _viewportRect.y);
	archive.Write("ViewportRect Width", _viewportRect.width);
	archive.Write("ViewportRect Height", _viewportRect.height);
	LV_WRITE(archive, _aaMode);
	archive.WriteKey("_cameraStack");
	archive.WriteStartArray(_cameraStack.Count());
	for (size_t i = 0; i < _cameraStack.Count(); i++)
	{
		archive.WriteReference(_cameraStack[i]);
	}
	archive.WriteEndArray();
	archive.WriteReference("_baseCamera", _baseCamera);
	LvVec4f colorVec = _backgroundColor.vec;
	LV_WRITE(archive, colorVec);

	archive.WriteEndObject();
}

void LvCameraComponent::Deserialize(LvArchive& archive)
{
	archive.ReadStartObject(LvObject::_type, this);

	LV_READ(archive, _renderType);
	LV_READ(archive, _projectionType);
	LV_READ(archive, enabled);
	LV_READ(archive, _aspect);
	LV_READ(archive, _fovRadian);
	LV_READ(archive, _nearP);
	LV_READ(archive, _farP);
	uint32 cullingMask;
	LV_READ(archive, cullingMask);
	_cullingMask = static_cast<LvLayerMask>(cullingMask);
	LV_READ(archive, _orthoSize);
	LV_READ(archive, _depth);
	LV_READ(archive, _viewMatrix);
	LV_READ(archive, _projectionMatrix);
	LV_READ(archive, _projectionViewMatrix);
	LV_READ(archive, _frustum);
	uint32 clearFlag;
	LV_READ(archive, clearFlag);
	_clearFlag = static_cast<LvCamera::Clear>(clearFlag);
	archive.Read("ViewportRect X", _viewportRect.x);
	archive.Read("ViewportRect Y", _viewportRect.y);
	archive.Read("ViewportRect Width", _viewportRect.width);
	archive.Read("ViewportRect Height", _viewportRect.height);
	LV_READ(archive, _aaMode);
	archive.UseKey("_cameraStack");
	const size_t cameraStackSize = archive.ReadStartArray();
	for (size_t i = 0; i < cameraStackSize; i++)
	{
		LvCameraComponent* component = nullptr;
		archive.ReadReference(component);
		_cameraStack.Add(component);
	}
	archive.ReadEndArray();
	archive.ReadReference("_baseCamera", _baseCamera);

	LvVec4f colorVec;
	LV_READ(archive, colorVec);
	_backgroundColor = colorVec;

	archive.ReadEndObject();
}

void LvCameraComponent::Update()
{
	LV_PROFILE_ENGINE();
	if (dirtyFlag.ContainsAny(DirtyFlag::FRUSTUM, DirtyFlag::NEAR_FAR_PLANE, DirtyFlag::PROJECTION_TYPE))
	{
		if (_projectionType == LvCameraComponent::ProjectionType::PERSPECTIVE)
		{
			_projectionMatrix = LvMat4f::PerspectiveLH(_fovRadian, _aspect, _nearP, _farP);
		}
		else
		{
			float width = _orthoSize * _viewportRect.width;
			float height = _orthoSize * _viewportRect.height;

			if (width < 0.f)
			{
				width = 0.f;
			}
			else if (width > 1.f)
			{
				width = _orthoSize * _aspect;
			}

			if (height < 0.f)
			{
				height = 0.f;
			}
			else if (height > 1.f)
			{
				height = _orthoSize;
			}

			_projectionMatrix = LvMat4f::OrthoLH(-width, width, -height, height, _nearP, _farP);
		}

		_projectInverseMatrix = LvMat4f::Inverse(_projectionMatrix);
		// assume that the view matrix is already update with a transform
		_projectionViewMatrix = _projectionMatrix * _viewMatrix;
		_frustum.Update(_projectionViewMatrix);
	}

	if (dirtyFlag.Contains(DirtyFlag::CAMERA_STACK))
	{
		if (nullptr != _baseCamera)
		{
			SetRect(_baseCamera->GetRect());
		}
	}
}

void LvCameraComponent::Transform(const LvTransformComponent& comp)
{
	if (comp.dirtyFlag.IsChanged())
	{
		LvVec3f camWorldPos = comp.GetWorldPosition();
		LvQuatf worldRot = comp.GetWorldRotation();

		_viewMatrix = LvQuatf::ToMat4(LvQuatf::Conjugate(worldRot));
		_viewMatrix[3][0] = -(_viewMatrix[0][0] * camWorldPos.x + _viewMatrix[1][0] * camWorldPos.y + _viewMatrix[2][0] * camWorldPos.z);
		_viewMatrix[3][1] = -(_viewMatrix[0][1] * camWorldPos.x + _viewMatrix[1][1] * camWorldPos.y + _viewMatrix[2][1] * camWorldPos.z);
		_viewMatrix[3][2] = -(_viewMatrix[0][2] * camWorldPos.x + _viewMatrix[1][2] * camWorldPos.y + _viewMatrix[2][2] * camWorldPos.z);

		_projectionViewMatrix = _projectionMatrix * _viewMatrix;
		_frustum.Update(_projectionViewMatrix);
		dirtyFlag.Add(DirtyFlag::FRUSTUM);
		dirtyFlag.Add(DirtyFlag::TRANSFORM);
	}
}

LvVec2f LvCameraComponent::WorldToScreenPoint(const LvVec3f& worldPos) const
{
	const LvMat4f& m = GetProjectionViewMatrix();

	LvVec3f viewportPoint = m.MultiplyPoint(worldPos);

	viewportPoint = LvVec3f(viewportPoint.x + 1.f, viewportPoint.y + 1.f, viewportPoint.z + 1.f) / 2.f;

	LvVec2f v = LvVec2f(viewportPoint.x * _renderTargetResolution.width, viewportPoint.y * _renderTargetResolution.height);
	return v;
	//return LvVec2f(v.x, v.y);
}

LvVec2f LvCameraComponent::WorldToViewportPoint(const LvVec3f& worldPos) const
{
	LvVec3f viewportPoint = _projectionViewMatrix * worldPos;

	viewportPoint = LvVec3f(viewportPoint.x + 1.f, viewportPoint.y + 1.f, viewportPoint.z + 1.f) / 2.f;

	return LvVec2f(viewportPoint.x, viewportPoint.y);
}

LvVec2f LvCameraComponent::ScreenToViewportPoint(const LvVec2f& pos) const
{
	return LvVec2f(pos.x * _renderTargetResolution.width, pos.y * _renderTargetResolution.height);
}

LvVec3f LvCameraComponent::ScreenToWorldPoint(const LvVec2f& position) const
{
	const LvVec4f viewSpacePickPosition(
		(((2.0f * position.x) / _renderTargetResolution.width) - 1) / _projectionMatrix[0][0],
		(((2.0f * position.y) / _renderTargetResolution.height) - 1) / _projectionMatrix[1][1],
		1.0f,
		1.0f
	);

	return  LvMat4f::Inverse(_viewMatrix) * viewSpacePickPosition;
}

LvVec2f LvCameraComponent::ViewportPointToScreen(const LvVec2f& pos) const
{

	return LvVec2f(pos.x * (_renderTargetResolution.width * _viewportRect.width)
		, pos.y * (_renderTargetResolution.height * _viewportRect.height));
}

LvRay LvCameraComponent::ViewportPointToRay(const LvVec2f& pos) const
{
	LvVec4f origin = _viewMatrix * LvVec4f(0.f, 0.f, 0.f, 1.f); // origin 
	LvVec4f direction = _projectInverseMatrix * LvVec4f(pos.x, pos.y, 0.f, 1.0f); // to world

	direction = _viewMatrix * direction;
	direction = LvVec4f::Normalize(direction);

	return LvRay(origin, direction);
}

LvRay LvCameraComponent::ScreenPointToRay(const LvVec2f& pos) const
{
	return ViewportPointToRay(ScreenToViewportPoint(pos));
}

LvRay LvCameraComponent::ScreenPointToUIRay(const LvVec2f& pos)
{
	return LvRay();
}

void LvCameraComponent::SetDepth(float p)
{
	_depth = p;
	dirtyFlag.Add(DirtyFlag::DEPTH);
}

void LvCameraComponent::SetBackgroundColor(LvVec4f bgc)
{
	_backgroundColor = bgc;
	dirtyFlag.Add(DirtyFlag::BACKGROUND_COLOR);
}


void LvCameraComponent::SetCameraStack(LvCameraComponent* component, size_t index)
{
	LV_ASSERT(_cameraStack.Count() >= index, "Invalid index!");

	if (nullptr != _cameraStack[index])
	{
		_cameraStack[index]->SetBaseCamera(nullptr); // @yongsik-im. 정상적으로 동작하는지(Stack에 존재하던 Overlay Camera를 제거했을 때 제거한 카메라의 Base가 null이 되는지) 확인이 필요합니다.
	}

	// component의 RenderType이 Overlay가 아니면 카메라 스택에 추가하지 않는다.
	if (nullptr != component && component->GetRenderType() != LvCameraComponent::RenderType::OVERLAY)
	{
		component = nullptr;
	}

	_cameraStack[index] = component;
	dirtyFlag.Add(DirtyFlag::CAMERA_STACK);

	if (nullptr != component)	//@yongsik-im. component가 null이 아니라면 해당 컴포넌트의 base camera를 현재 카메라로 설정합니다.
	{
		component->SetBaseCamera(this);
	}
}

void LvCameraComponent::ResizeCaemraStack(size_t length)
{
	size_t count = _cameraStack.Count();

	if (length < count)
	{
		_cameraStack.Resize(length);
	}
	else
	{
		while (length > count)
		{
			_cameraStack.Add(nullptr);
			count++;
		}
	}
}

LvCameraComponent* LvCameraComponent::GetStackedCamera(size_t index) const
{
	LV_ASSERT(index >= 0 && index < _cameraStack.Count(), "Invalid Index");
	return _cameraStack[index];
}

void LvCameraComponent::RemoveStackedCamera(size_t index)
{
	LV_ASSERT(index >= 0 && index < _cameraStack.Count(), "Invalid Index"); 
	_cameraStack[index]->SetBaseCamera(nullptr);
	_cameraStack[index] = nullptr;

	for (size_t i = index; i < _cameraStack.Count() - 1; i++)
	{
		_cameraStack[i] = _cameraStack[i + 1];
	}

	ResizeCaemraStack(_cameraStack.Count() - 1);
	dirtyFlag.Add(DirtyFlag::CAMERA_STACK);
}

void LvCameraComponent::SetBaseCamera(LvCameraComponent* entity)
{
	_baseCamera = entity;
	dirtyFlag.Add(DirtyFlag::CAMERA_STACK);
}

LV_NS_ENGINE_END