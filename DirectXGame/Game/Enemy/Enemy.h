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
#include "TimedCall.h"

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Enemy;
class Player;
class GameScene;

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

	//=========================================================================================
	// public methods
	//=========================================================================================

	EnemyStateApproach(Enemy* enemy);

	~EnemyStateApproach();

	void Init();

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const int kFireInterval = 50;
	std::list<std::unique_ptr<TimedCall>> timeCalls_;

	int32_t stateChangeTime_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void SetFire();

};

////////////////////////////////////////////////////////////////////////////////////////////
// EnemyStateLeave class
////////////////////////////////////////////////////////////////////////////////////////////
class EnemyStateLeave
	: public BaseEnemyState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

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
	void Init(Model* model, uint32_t textureHandle, const Vector3f& pos);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	void Term();

	//! @brief ImGuiに設定
	void SetOnImGui(const std::string& id = "##");

	//! @brief stateの変更
	//! 
	//! @param[in] state 次のstate
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	//! @brief 衝突したら呼び出される関数
	void OnCollision();

	bool IsDead() const { return isDead_; }

	Vector3f GetWorldPosition() const;

	const float GetCollisionRadius() const { return kCollisionRadius_; }

	void SetPos(const Vector3f& pos) { worldTransform_.translation_ = pos; }

	void SetPlayer(Player* player) { player_ = player; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	//! @brief 弾を発射
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

	Player* player_ = nullptr;

	GameScene* gameScene_;

	// parameter //
	const float kBulletSpeed_ = 0.6f;
	const float kCollisionRadius_ = 1.0f;

	// info //
	std::unique_ptr<BaseEnemyState> state_;
	WorldTransform worldTransform_;

	bool isDead_ = false;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void Action();

	

};