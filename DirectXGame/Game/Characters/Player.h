#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <list>
#include <optional>

// engine
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

// mylib
#include <MyMath.h>

// character
#include "BaseCharacter.h"

// weapon
#include "Hammer.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameScene;
class LockOn;
class Player;

////////////////////////////////////////////////////////////////////////////////////////////
// Behavior enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class Behavior {
	kRoot,   //!< 通常状態
	kAttack, //!< 攻撃状態
	kJump,   //!< ジャンプ状態
	kDash,   //!< ダッシュ状態
};

////////////////////////////////////////////////////////////////////////////////////////////
// BasePlayerBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class BasePlayerBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BasePlayerBehavior() = delete;

	BasePlayerBehavior(Player* player) : player_(player) {}

	virtual ~BasePlayerBehavior() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	Behavior GetBehavior() const noexcept { return behavior_; }


protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	Player* player_ = nullptr;

	//* member *//

	Behavior behavior_;
	const std::string groupName_ = "Player";

};

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBehaviorRoot class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBehaviorRoot
	: public BasePlayerBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerBehaviorRoot(Player* player) : BasePlayerBehavior(player) { Init(); }

	void Init() override;

	void Update() override;

private:
	
	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	float moveSpeed_ = 0.2f;

	const uint16_t period_ = 60; //!< 今回は1秒(60frame)周期
	const float floatingStep_ = 2.0f * pi_v / period_;
	const float floatingRange_ = 0.2f;

	const float kDeadZone_ = 0.7f;

	//* member *//

	float floatingParameter_ = 0.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void ApplyGlobalVariables();

	void InitFloatingGimmick();
	void UpdateFloatingGimmick();

	void Action();

};

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBehaviorAttack class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBehaviorAttack
	: public BasePlayerBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerBehaviorAttack(Player* player) : BasePlayerBehavior(player) { Init(); }

	void Init() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	const uint16_t kAttackTime_ = 60 /*frame*/ * 1 /*sec*/;

	//* member *//

	float attackParameter_ = 0;
	float attackMoveSpeed_ = 0.0f;


};

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBehaviorJump class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBehaviorJump
	: public BasePlayerBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerBehaviorJump(Player* player) : BasePlayerBehavior(player) { Init(); }

	void Init() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// PlayerBehaviorDash class
////////////////////////////////////////////////////////////////////////////////////////////
class PlayerBehaviorDash
	: public BasePlayerBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PlayerBehaviorDash(Player* player) : BasePlayerBehavior(player) { Init(); }

	void Init() override;

	void Update() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	const uint32_t kDashTime_ = 10 /* frame */;
	const float kDashSpeed_ = 1.0f;
	
	//* member *//

	uint32_t dashParameter_ = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player
	: public BaseCharacter {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Player() = default;

	~Player() { Term(); }

	void Init(const std::vector<Model*>& models) override;

	void Term() override;

	void Update() override;

	void Draw(const ViewProjection& viewProj) override;

	void OnCollision([[maybe_unused]]Collider* other) override;

	//* Setter *//

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

	void SetViewProj(const ViewProjection* viewProj) { viewProj_ = viewProj; }

	void SetHammer(Model* hammerModel, EffectManager* effectManager, Model* hitEffectModel) {
		hammer_->Init(hammerModel, &worldTransform_);
		hammer_->SetEffect(effectManager, hitEffectModel);
	}

	//* Getter *//

	Behavior GetBehavior() const noexcept { return behavior_->GetBehavior(); }

	Vector3f GetCenterPosition() const;

	Hammer* GetHammer() const { return hammer_.get(); }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ModelType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum ModelType {
		MODEL_BODY,
		MODEL_HEAD,
		MODEL_LARM,
		MODEL_RARM,

		kCountOfModelType
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	Input* input_ = Input::GetInstance();
	GameScene* gameScene_ = nullptr;
	const ViewProjection* viewProj_ = nullptr;
	const LockOn* lockOn_ = nullptr;

	//* parameter *//

	const float kRotateRate_ = 0.12f;
	const Vector3f kMoveLimit_ = {100.0f, 100.0f, 100.0f};

	//* member *//
	
	Vector3f velocity_ = {0.0f};
	Vector3f direction_ = {0.0f};
	float targetAngle_ = 0.0f;

	// transforms
	WorldTransform modelTransforms_[kCountOfModelType];

	// weapon
	std::unique_ptr<Hammer> hammer_;
	
	// behavior
	std::unique_ptr<BasePlayerBehavior> behavior_ = nullptr;
	std::optional<Behavior>             behaviorRequest_ = Behavior::kRoot;

	//=========================================================================================
	// private methods
	//=========================================================================================
	
	void UpdateBehavior();

	void ApplyGlobalVariables();
	
	//=========================================================================================
	// friend behavior
	//=========================================================================================

	friend PlayerBehaviorRoot;
	friend PlayerBehaviorAttack;
	friend PlayerBehaviorJump;
	friend PlayerBehaviorDash;

};

