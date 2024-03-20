#pragma once
#ifndef __LV_SCRIPT_COMPONENT_H__
#define __LV_SCRIPT_COMPONENT_H__

#include "engine/LvGameEntity.h"
#include "engine/component/LvComponent.h"
#include "engine/LvCoroutine.h"
	
/* #include "engine/LvScene.h" */ namespace Lv { namespace Engine { class LvScene; } }
/* #include "engine/component/LvColliderComponent.h" */ namespace Lv { namespace Engine { class LvColliderComponent; } }

LV_NS_ENGINE_BEGIN

class LV_API LV_REFLECT LvScriptComponent : public LvComponent
{
public:
	LvScene* scene = nullptr;

	virtual ~LvScriptComponent();

	/**
	* @brief Component의 enable이 true가 될 때 마다 호출 새로 추가시에도 제일 먼저 호출됨
	*/
	virtual void OnEnable() {}

	/**
	* @brief Component가 Update loop가 수행될때 제일 먼저 최초 1번 호출됨 
	*/
	virtual void Awake() {}
	
	/**
	* @brief Component가 Update loop가 수행될때 모든 Component의 Awake가 호출된 다음 1번 호출됨
	*/
	virtual void Start() {}

	/**
	* @brief Component가 Update loop중 호출되는 함수
	*/
	virtual void Update(float deltaTime) {}

	/**
	* @brief Component가 Update loop중 모든 ScriptComponent의 Update 호출 이후에 호출되는 함수
	*/
	virtual void LateUpdate(float deltaTime) {}

	/**
	* @brief Component의 enable이 false가 될 때 마다 호출 삭제시에 OnDestroy전에 호출됨
	*/
	virtual void OnDisable() {}

	/**
	* @brief Component가 삭제될 때 제일 마지막에 호출되는 함수
	*/
	virtual void OnDestroy() {}

	/**
	* @brief OnCollisionEnter는 이 충돌체/강체가 다른 강체/충돌체에 닿기 시작하면 호출됩니다.
	*/
	virtual void OnCollisionEnter(LvColliderComponent& collider) {}

	/**
	* @brief OnCollisionStay는 다른 Collider 또는 Rigidbody에 닿는 모든 Collider 또는 Rigidbody에 대해 프레임당 한 번씩 호출됩니다.
	*/
	virtual void OnCollisionStay(LvColliderComponent& collider) {}

	/**
	* @brief OnCollisionExit는 이 충돌체/강체가 다른 강체/충돌체 접촉을 중지하면 호출됩니다.
	*/
	virtual void OnCollisionExit(LvColliderComponent& collider) {}


	void Serialize(LvArchive& archive) override;

	void Deserialize(LvArchive& archive) override;

	/**
	* @brief 외부에서 생성한 코루틴을 시작합니다. 이는 Coroutine 의 ScriptComponent 소유와 연관이 없습니다. 
	*		 따라서 ScriptComponent 가 삭제되어도 코루틴은 계속 실행될 수 있습니다.
	*/
	LV_DEPRECATED
	LvCoroutineHandle StartCoroutine(LvCoroutine& coroutine);

	/**
	* @brief Coroutine 을 호출합니다.
	*/
	template<typename T, typename... TArgs, typename std::enable_if<sizeof...(TArgs) == 0>::type* = nullptr>
	LvCoroutineHandle StartCoroutine()
	{
		auto coroutine = new T();
		return registCoroutine(*coroutine);
	}

	/**
	* @brief Coroutine 을 호출합니다.
	*/
	template<typename T, typename... TArgs, typename std::enable_if<sizeof...(TArgs) != 0>::type* = nullptr>
	LvCoroutineHandle StartCoroutine(TArgs&&... args)
	{
		auto coroutine = new T(std::forward<TArgs>(args)...);
		return registCoroutine(*coroutine);
	}

	/**
	* @brief Coroutine 을 멈춥니다.
	*/
	void StopCoroutine(LvCoroutineHandle& handle);

	LvGameEntity GetGameEntity() const;

private :

	LvCoroutineHandle registCoroutine(LvCoroutine& coroutine);
};

using ScriptComponent = LvScriptComponent;

LV_NS_ENGINE_END

#endif	