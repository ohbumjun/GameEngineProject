#pragma once
#ifndef __LV_CAMERA_COMPONENT_H__
#define __LV_CAMERA_COMPONENT_H__

#include "system/math/LvMat4f.h"
#include "system/math/LvVec2f.h"
#include "system/util/LvColor.h"

#include "engine/LvFlag.h"
#include "engine/component/LvComponent.h"
#include "engine/attribute/LvSerializeAttribute.h"
#include "engine/attribute/LvSerializeVersionAttribute.h"
#include "engine/LvGeometry.h"
#include "engine/LvLayerMask.h"
#include "engine/LvCamera.h"
#include "engine/LvTexture.h"

/* #include "engine/component/LvTransformComponent.h" */ namespace Lv { namespace Engine { class LvTransformComponent; } }

LV_NS_ENGINE_BEGIN

class LV_API
	LV_REFLECT
	LV_ATTRIBUTE(LvShowOrderAttribute(10))
	LV_ATTRIBUTE(LvSerializeVersionAttribute(3))
	LV_ATTRIBUTE(LvInspectorNameAttribute("Camera"))
	LvCameraComponent : public LvComponent
{
	LV_REFLECTION_FRIEND

	friend class LvCameraComponentSystem;

public:

	enum class DirtyFlag : uint32
	{
		NONE = 0,
		PROJECTION_TYPE = 1 << 0,
		RENDER_TYPE = 1 << 1,
		TRANSFORM = 1 << 2,
		FRUSTUM = 1 << 3,
		NEAR_FAR_PLANE = 1 << 4,
		VIEWPORT_RECT = 1 << 5,
		BACKGROUND_COLOR = 1 << 6,
		DEPTH = 1 << 7,
		RESOLUTION = 1 << 8,
		CULLING_MASK = 1 << 9,
		ANTI_ALIASING = 1 << 10,
		CAMERA_STACK = 1 << 11,
		CLEAR = 1 << 12,
	};

	enum class LV_REFLECT ProjectionType : uint8
	{
		PERSPECTIVE = 0,
		ORTHOGONAL,
	};

	enum class LV_REFLECT AntiAliasingMode : uint8
	{
		NONE,
		FXAA,
	};

	enum class LV_REFLECT RenderType : uint8
	{
		BASE,
		OVERLAY,
	};

	LV_ATTRIBUTE(LvNonSerializeAttribute())
	LvDirtyFlag<DirtyFlag> dirtyFlag;

	LvCameraComponent();
	~LvCameraComponent();

	void SetDirty(uint32 dirtyflag) override { dirtyflag == LV_UINT_MAX ? dirtyFlag.Clear(true) : dirtyFlag.Set(static_cast<DirtyFlag>(dirtyflag)); }

	// TODO : move to camera component system
	void Update();

	// TODO : move to camera component system
	void Transform(const LvTransformComponent& comp);

	/**
	* @brief 카메라의 컬링 마스크를 반환합니다.
	*/
	LV_FORCEINLINE uint32 GetCullingMask() const { return (uint32)_cullingMask; }
	LV_REFLECT
	LV_FORCEINLINE void SetCullingMask(uint32 cullingMask)
	{
		_cullingMask = (LvLayerMask)cullingMask;
		dirtyFlag.Add(DirtyFlag::CULLING_MASK);
	}

	LV_FORCEINLINE void AddCullingMask(uint32 cullingMask)
	{
		_cullingMask = (LvLayerMask)((uint32)_cullingMask | (uint32)cullingMask);
		dirtyFlag.Add(DirtyFlag::CULLING_MASK);
	}

	/**
	* @brief Camera 가 직교인지 투영인지 타입을 반환합니다.
	*/
	LV_FORCEINLINE ProjectionType GetProjectionType() const { return _projectionType; }
	LV_REFLECT
	LV_FORCEINLINE void SetProjectionType(ProjectionType type)
	{
		if (_projectionType != type)
		{
			_projectionType = type;
			dirtyFlag.Add(DirtyFlag::PROJECTION_TYPE);
		}
	}

	/**
	* @brief 해상도의 width/height 비율을 반환합니다.
	*/
	LV_FORCEINLINE float GetAspect() const { return _aspect; }
	LV_REFLECT
	LV_FORCEINLINE void SetAspect(float aspect)
	{
		if (LvMath::Abs(_aspect - aspect) > LV_EPS)
		{
			_aspect = aspect;
			dirtyFlag.Add(DirtyFlag::FRUSTUM);
			dirtyFlag.Add(DirtyFlag::TRANSFORM);
		}
	}
	LV_FORCEINLINE AntiAliasingMode GetAntiAliasingMode() const { return _aaMode; }
	LV_REFLECT
	LV_FORCEINLINE void SetAntiAliasingMode(AntiAliasingMode aaMode) 
	{ 
		_aaMode = aaMode; 
		dirtyFlag.Add(DirtyFlag::ANTI_ALIASING); 
	}

	LV_FORCEINLINE RenderType GetRenderType() const { return _renderType; }
	LV_FORCEINLINE LV_REFLECT void SetRenderType(RenderType renderType)
	{
		_renderType = renderType;
		dirtyFlag.Add(DirtyFlag::RENDER_TYPE);
	}

	float GetFOVByDegree() const;
	void SetFOVByDegree(float angle);

	/**
	* @brief field of view 의 radian 값을 반환합니다.
	*/
	LV_FORCEINLINE float GetFOVRadian() const { return _fovRadian; }
	LV_REFLECT
	LV_FORCEINLINE void SetFOVRadian(float radian)
	{
		if (LvMath::Abs(_fovRadian - radian) > LV_EPS)
		{
			_fovRadian = radian;
			dirtyFlag.Add(DirtyFlag::FRUSTUM);
			dirtyFlag.Add(DirtyFlag::TRANSFORM);
		}
	}

	/**
	* @brief 카메라 직교 투영일 경우 직교 사이즈를 반환합니다.
	*/
	LV_FORCEINLINE float GetOrthoSize() const { return _orthoSize; }
	LV_REFLECT
	LV_FORCEINLINE void SetOrthoSize(float size)
	{
		if (LvMath::Abs(_orthoSize - size) > LV_EPS)
		{
			_orthoSize = size;
			dirtyFlag.Add(DirtyFlag::TRANSFORM);
			dirtyFlag.Add(DirtyFlag::VIEWPORT_RECT);
		}
	}

	/**
	* @brief 절두체의 가까운 거리를 반환합니다.
	*/
	LV_FORCEINLINE float GetNearPlane() const { return _nearP; }
	LV_REFLECT
	LV_FORCEINLINE void SetNearPlane(float nearPlane)
	{
		float nearP = nearPlane;

		/*
		* Unity Logic
		*/
		if (nearP < 0.01f && _projectionType == ProjectionType::PERSPECTIVE)
			nearP = 0.01f;

		if (LvMath::Abs(nearP - _nearP) > LV_EPS)
		{
			if (nearP >= _farP)
			{
				SetFarPlane(_nearP);
			}

			_nearP = nearP;
			dirtyFlag.Add(DirtyFlag::NEAR_FAR_PLANE);
		}
	}

	/**
	* @brief 절두체의 먼 거리를 반환합니다.
	*/
	LV_FORCEINLINE float GetFarPlane() const { return _farP; }
	LV_REFLECT
	LV_FORCEINLINE void SetFarPlane(float farPlane)
	{
		/*
		* Unity Logic
		*/
		if (farPlane < _nearP)
			farPlane = _nearP + 0.01f;

		if (LvMath::Abs(farPlane - _farP) > LV_EPS)
		{
			_farP = farPlane;
			dirtyFlag.Add(DirtyFlag::NEAR_FAR_PLANE);
		}
	}

	/**
	* @brief view 행렬을 반환합니다.
	*/
	LV_FORCEINLINE const LvMat4f& GetViewMatrix() const { return _viewMatrix; }
	LV_FORCEINLINE void SetViewMatrix(const LvMat4f& viewMat)
	{
		if (_viewMatrix != viewMat)
		{
			_viewMatrix = viewMat;
			dirtyFlag.Add(DirtyFlag::TRANSFORM);
		}
	}

	/**
	* @brief projection - view 행렬을 반환합니다.
	*/
	LV_FORCEINLINE const LvMat4f& GetProjectionViewMatrix() const { return _projectionViewMatrix; }

	/**
	* @brief 카메라의 투영행렬을 반환합니다.
	*/
	LV_FORCEINLINE const LvMat4f& GetProjectionMatrix() const { return _projectionMatrix; }
	LV_FORCEINLINE void SetProjectionMatrix(const LvMat4f& projMat)
	{
		if (_projectionMatrix != projMat)
		{
			_projectionMatrix = projMat;
			dirtyFlag.Add(DirtyFlag::TRANSFORM);
		}
	}


	/**
	* @brief 절두체를 반환합니다.
	*/
	LV_FORCEINLINE const LvFrustum& GetFrustum() const { return _frustum; }
	LV_FORCEINLINE void SetFrustum(LvFrustum frustum)
	{
		if (_frustum != frustum)
		{
			_frustum = frustum;

			dirtyFlag.Add(DirtyFlag::FRUSTUM);
		}
	}

	//TODO: @yongsik-im Doxygen 주석 추가할 것.
	LV_REFLECT
	void SetCameraStack(LvCameraComponent* entity, size_t index);
	LV_REFLECT
	void ResizeCaemraStack(size_t length);
	LvCameraComponent* GetStackedCamera(size_t index) const;
	void RemoveStackedCamera(size_t index);
	LV_FORCEINLINE size_t GetStackedCameraCount() const { return _cameraStack.Count(); }

	LV_REFLECT
	void SetBaseCamera(LvCameraComponent* entity);
	LV_FORCEINLINE LvCameraComponent* GetBaseCamera() const { return _baseCamera; }

	/**
	* @brief View Rect 를 반환합니다.
	*/
	LV_FORCEINLINE LvRect GetRect() const { return _viewportRect; }
	LV_REFLECT
	LV_FORCEINLINE void SetRect(LvRect vpr)
	{
		if (_viewportRect != vpr)
		{
			_viewportRect = vpr;
			dirtyFlag.Add(DirtyFlag::VIEWPORT_RECT);
		}
	}

	/**
	* @brief 카메라간의 그려지는 순서를 반환합니다.
	*/
	LV_FORCEINLINE float GetDepth() const { return _depth; }
	LV_REFLECT
	void SetDepth(float p);


	/**
	* @brief 카메라 렌더타겟의 해상도를 반환합니다.
	*/
	LV_FORCEINLINE LvResolution GetRenderTargetResolution() const { return _renderTargetResolution; }
	LV_FORCEINLINE void SetRenderTargetResolution(LvResolution resolution)
	{
		if (_renderTargetResolution != resolution)
		{
			_renderTargetResolution = resolution;
			dirtyFlag.Add(DirtyFlag::RESOLUTION);
		}

	}

	/**
	* @brief 뷰의 바탕색상을 반환합니다.
	*/
	LV_FORCEINLINE LvVec4f GetBackgroundColor() const { return _backgroundColor; }

	LV_REFLECT
	void SetBackgroundColor(LvVec4f bgc);

	LV_FORCEINLINE LvCamera::Clear GetClearFlag() const { return _clearFlag; }

	LV_REFLECT
	LV_FORCEINLINE void SetClearFlag(LvCamera::Clear clearFlag)
	{
		if (_clearFlag != clearFlag)
		{
			_clearFlag = clearFlag;
			dirtyFlag.Add(DirtyFlag::CLEAR);
		}
	}

	/**
	* @brief World 좌료를 Screen 좌표로 변환합니다.
	*/
	LvVec2f WorldToScreenPoint(const LvVec3f& worldPos) const;

	/**
	* @brief World 좌료를 Viewport 좌표로 변환합니다.
	*/
	LvVec2f WorldToViewportPoint(const LvVec3f& worldPos) const;

	/**
	* @brief Screen 좌표를 World 좌료포 변환합니다.
	*/
	LvVec3f ScreenToWorldPoint(const LvVec2f& point) const;
	/**
	* @brief Screen 좌표를 Viewport 좌표로 변환합니다.
	*/
	LvVec2f ScreenToViewportPoint(const LvVec2f& pos) const;

	/**
	* @brief viewport 좌표를 screen 좌표로 변환합니다.
	*/
	LvVec2f ViewportPointToScreen(const LvVec2f& pos) const;

	/**
	* @brief viewport 좌표를 ray 로 변환합니다.
	*/
	LvRay ViewportPointToRay(const LvVec2f& pos) const;

	/**
	* @brief screen 좌표를 ray 로 변환합니다.
	*/
	LvRay ScreenPointToRay(const LvVec2f& pos) const;

	/**
	* @brief Screen 좌표를 UI ray 로 변환합니다.
	*/
	LvRay ScreenPointToUIRay(const LvVec2f& pos);

	void Serialize(LvArchive& archive) override;
	void Deserialize(LvArchive& archive) override;

	uint32 GetLookAtTargetId() const noexcept { return _lookAtTargetId; }

protected:

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Projection"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetProjectionType"))
	ProjectionType _projectionType;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Render Type"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetRenderType"))
	RenderType _renderType;

	LV_REFLECT
	// 해당 멤버는 인스펙터에 노출되지 않습니다.
	//LV_ATTRIBUTE(LvInspectorNameAttribute("Aspect"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetAspect"))
	float _aspect;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Culling Mask"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetCullingMask"))
	LvLayerMask _cullingMask = (LvLayerMask)((uint32)LvLayerMask::DEFAULT | (uint32)LvLayerMask::TRANSPARENTFX | (uint32)LvLayerMask::POST_EFFECT | (uint32)LvLayerMask::ICON | (uint32)LvLayerMask::RESERVED);
	//(uint32)LvLayerMask::ALL;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Anti-aliasing"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetAntiAliasingMode"))
	AntiAliasingMode _aaMode;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Near Plane"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetNearPlane"))
	float _nearP;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Far Plane"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetFarPlane"))
	float _farP;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Fov Radian"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetFOVRadian"))
	float _fovRadian;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Ortho Size"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetOrthoSize"))
	float _orthoSize;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Depth"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetDepth"))
	float _depth;	// rendering priority 

	// @brief 현재 찍고 있는 renderTarget의 resolution
	LvResolution _renderTargetResolution;

	//LV_REFLECT
	//	LV_ATTRIBUTE(LvInspectorNameAttribute("LookAt"))
	//bool _lookAtEnable;
	//  TODO : 인스펙터에 안 나오고 있는데 확인 필요!
	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("LookAtTarget"))
	uint32 _lookAtTargetId = -1;

	LvMat4f _viewMatrix;

	LvMat4f _projectionMatrix;

	LvMat4f _projectInverseMatrix;

	LvMat4f _projectionViewMatrix;	// Projection * View Matrix

	LvFrustum _frustum;

	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Viewport Rect"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetRect"))
	LvRect _viewportRect;

	// environment
	// TODO backGroundType -> skybox 구현필요
	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Background Color"))//
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetBackgroundColor"))
	LvColor _backgroundColor;

	// TODO: unity의 clear Flag와 같은 역할을 할 것이다. 우선 임시로 int으로 처리해둠.-> 수정 필요
	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Clear Flag"))
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetClearFlag"))
	LvCamera::Clear _clearFlag = LvCamera::Clear::SOLID_COLOR;


	LV_REFLECT
	LV_ATTRIBUTE(LvInspectorNameAttribute("Camera Stack"))
	LV_ATTRIBUTE(LvSetElementPropertyAttribute("SetCameraStack"))
	LV_ATTRIBUTE(LvResizePropertyAttribute("ResizeCameraStack"))
	LvList<LvCameraComponent*> _cameraStack;

	LV_REFLECT
	// 해당 멤버는 인스펙터에 노출되지 않습니다.
	LV_ATTRIBUTE(LvSetPropertyAttribute("SetBaseCamera"))
	LvCameraComponent* _baseCamera = nullptr;
};

using CameraComponent = LvCameraComponent;

LV_NS_ENGINE_END

#endif
