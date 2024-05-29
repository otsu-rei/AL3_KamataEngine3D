#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"

#include <memory>
#include <sstream>

// Game
#include "RailCamera.h"
#include "Skydome.h"
#include "Player.h"
#include "Enemy.h"
#include "CollisionManager.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//! @brief 敵弾の追加
	void AddEnemyBullet(std::unique_ptr<EnemyBullet>& enemyBullet);

	//! @brief 自弾の追加
	void AddPlayerBullet(std::unique_ptr<PlayerBullet>& playerBullet);

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// camera
	ViewProjection viewProjection_;

	bool isDebugCameraActive_ = false;
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<RailCamera> railCamera_;

	/// Game
	// model
	std::unique_ptr<Model> cubeModel_;
	std::unique_ptr<Model> skydomeModel_;

	// skydome
	std::unique_ptr<Skydome> skydome_;

	// player
	std::unique_ptr<Player> player_;
	uint32_t playerTextureHandle_;

	// enemy
	std::list<std::unique_ptr<Enemy>> enemies_;
	uint32_t enemyTextureHandle_;

	// bullets
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	std::list<std::unique_ptr<PlayerBullet>> playerBullets_;

	// collisionManager
	std::unique_ptr<CollisionManager> collisionManager_;

	// commands
	std::stringstream enemyPopCommands_;
	bool isWait_ = false;
	int32_t waitTime_ = 0;

	void UpdatePlayerBullet();
	void UpdateEnemyBullet();

	void DrawPlayerBullet(const ViewProjection& viewProj);
	void DrawEnemyBullet(const ViewProjection& viewProj);

	void CheckAllCollision();

	void LoadEnemyPopData();
	void UpdateEnemyPopCommands();

	void DrawRail(const std::vector<Vector3f>& controllPoints, const Vector4f& color);
};
