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
#include "FollowCamera.h"
#include "Skydome.h"
#include "Ground.h"
#include "Player.h"

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

	std::unique_ptr<FollowCamera> followCamera_;

	/// Game
	// model
	std::unique_ptr<Model> cubeModel_;
	std::unique_ptr<Model> skydomeModel_;
	std::unique_ptr<Model> groundModel_;

	// chara
	std::unique_ptr<Model> bodyModel_;
	std::unique_ptr<Model> headModel_;
	std::unique_ptr<Model> lArmModel_;
	std::unique_ptr<Model> rArmModel_;

	// player
	std::unique_ptr<Player> player_;
	uint32_t playerTextureHandle_;

	// skydome
	std::unique_ptr<Skydome> skydome_;

	// ground
	std::unique_ptr<Ground> ground_;

};
