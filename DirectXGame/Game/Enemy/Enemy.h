#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <list>

// engine
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

// Game
#include "EnemyBullet.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Enemy;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseEnemyState base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseEnemyState(Enemy* enemy) : enemy_(enemy) {}

	virtual ~BaseEnemyState() {}

	virtual void Update() = 0;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Enemy* enemy_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateApproach class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateApproach
	: public BaseEnemyState {
public:

	EnemyStateApproach(Enemy* enemy);

	void Init();

	void Update();

private:

	static const int kFireInterval = 60;
	int32_t fireTimer_ = 0;

};

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateLeave class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateLeave
	: public BaseEnemyState {
public:

	EnemyStateLeave(Enemy* enemy);

	void Update();
};

////////////////////////////////////////////////////////////////////////////////////////////
// Enemy class
////////////////////////////////////////////////////////////////////////////////////////////
class Enemy {
public:

	//=========================================================================================
	// public variables
	//=========================================================================================

	// parameters //
	const float kMoveSpeed_ = 0.1f;

	//=========================================================================================
	// public methods
	//=========================================================================================

	~Enemy() { Term(); }

	//! @brief 初期化処理
	void Init(Model* model, uint32_t textureHandle);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	void Term();

	//! @brief ImGuiに設定
	void SetOnImGui();

	//! @brief stateの変更
	//! 
	//! @param[in] state 次のstate
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	const Vector3f& GetPos() const { return worldTransform_.translation_; }

	void SetPos(const Vector3f& pos) { worldTransform_.translation_ = pos; }

	void Fire();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Phase enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Phase {
		Approach, //!< 接近する
		Leave,    //!< 離脱する
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	// parameter //
	const float kBulletSpeed_ = 1.0f;

	// info //
	std::unique_ptr<BaseEnemyState> state_;
	WorldTransform worldTransform_;

	// Bullet
	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Action();

	

};