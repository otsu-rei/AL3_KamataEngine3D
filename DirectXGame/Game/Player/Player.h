#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <list>
#include <unordered_set>

// engine
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

// mylib
#include <MyMath.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class GameScene;

////////////////////////////////////////////////////////////////////////////////////////////
// Player class
////////////////////////////////////////////////////////////////////////////////////////////
class Player {
public:
	//=========================================================================================
	// public methods
	//=========================================================================================

	~Player() { Term(); }

	//! @brief 初期化処理
	void Init(Model* head, Model* body, Model* lArm, Model* rArm, const Vector3f& pos);

	//! @brief 更新処理
	void Update();

	//! @brief 描画処理
	void Draw(const ViewProjection& viewProj);

	//! @brief 終了処理
	void Term();

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	//! @brief ImGuiに設定
	void SetOnImGui();

	/* setter */

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	void SetParent(const WorldTransform* parent) {
		worldTransform_.parent_ = parent;
	}

	void SetViewProj(const ViewProjection* viewProj) {
		viewProj_ = viewProj;
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	//  structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parts {
		/* models */
		Model* head = nullptr;
		Model* body = nullptr;
		Model* lArm = nullptr;
		Model* rArm = nullptr;

		/* worldTransforms */
		WorldTransform headTransform;
		WorldTransform bodyTransform;
		WorldTransform lArmTransform;
		WorldTransform rArmTransform;

		void InitWorldTranform(const WorldTransform& parent) {
			// parent -> this
			bodyTransform.Initialize();
			bodyTransform.SetParent(&parent);

			// patent -> body -> this
			headTransform.Initialize();
			headTransform.SetParent(&bodyTransform);

			// patent -> body -> this
			lArmTransform.Initialize();
			lArmTransform.SetParent(&bodyTransform);
			lArmTransform.translation_ = {-1.4f, 2.5f, 0.0f};

			// patent -> body -> this
			rArmTransform.Initialize();
			rArmTransform.SetParent(&bodyTransform);
			rArmTransform.translation_ = {1.4f, 2.5f, 0.0f};
		}

		void UpdateMatrix() {
			bodyTransform.UpdateMatrix();
			headTransform.UpdateMatrix();
			lArmTransform.UpdateMatrix();
			rArmTransform.UpdateMatrix();
		}

		void Draw(const ViewProjection& viewProj) {
			head->Draw(headTransform, viewProj);
			body->Draw(bodyTransform, viewProj);
			lArm->Draw(lArmTransform, viewProj);
			rArm->Draw(rArmTransform, viewProj);
		}
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	/* externals */
	Input* input_ = Input::GetInstance();
	GameScene* gameScene_ = nullptr;
	const ViewProjection* viewProj_ = nullptr;

	/* parameters */
	const float kMoveSpeed_ = 0.2f;
	const Vector3f kMoveLimit_ = {100.0f, 100.0f, 100.0f};
	const float kDeadZone_ = 0.7f;
	const float kRotateRate_ = 0.12f;
	const uint16_t period_ = 60; //!< 今回は1秒(60frame)周期
	const float floatingStep_ = 2.0f * pi_v / period_;
	const float floatingRange_ = 0.2f;

	/* data */
	WorldTransform worldTransform_;
	Parts parts_;

	float targetAngle_ = 0.0f;
	float floatingParameter_ = 0.0f;



	//=========================================================================================
	// private methods
	//=========================================================================================

	void Move();

	void MoveController();

	void InitFloatingGimmick();

	void UpdateFloatingGimmick();

};