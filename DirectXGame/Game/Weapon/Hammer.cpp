#include "Hammer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include "GlobalVariables.h"
#include "MyMath.h"
#include "Enemy.h"
#include "HitEffect.h"

#include <stdexcept>

////////////////////////////////////////////////////////////////////////////////////////////
// Hammer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Hammer::Init(Model* model, WorldTransform* parent) {
	Collider::Init();
	Collider::SetTypeId(CollisionTypeIdDef::kPlayerWeapon);
	Collider::SetRadius(4.0f);

	assert(model);

	model_ = model;

	localTransform_.Initialize();

	localTransform_.parent_      = parent;
	localTransform_.translation_ = {0.0f, 2.4f, 0.0f};

	// グループの生成
	const std::string groupName = "Hammer";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "translation", localTransform_.translation_);
}

void Hammer::Term() {}

void Hammer::Update() {

	// globalVariablesの適用
	const std::string groupName = "Hammer";
	localTransform_.translation_ = globalVariables->GetValue<Vector3f>(groupName, "translation");
	// hack: rotationを適用する場合, 上書きされるので

	localTransform_.UpdateMatrix();
}

void Hammer::Draw(const ViewProjection& viewProj) {

	model_->Draw(localTransform_, viewProj); }

void Hammer::ClearRecord() {
	record_.ClearRecord();
}

void Hammer::OnCollision([[maybe_unused]]Collider* other) {
	
	if (other->GetTypeId() == static_cast<uint32_t>(CollisionTypeIdDef::kEnemy)) { //!< 当たった相手が敵だった場合
		Enemy* enemy = static_cast<Enemy*>(other);
		uint32_t serialId = enemy->GetSerialId();

		if (record_.CheckRecord(serialId)) { //!< すでにあったってるかの確認
			return;
		}

		record_.AddRecord(serialId);

		if (effectManager_ == nullptr && effectModel_ == nullptr) { //!< manager modelが設定されてないとき
			return;
		}
		
		// hitEffectの生成
		std::unique_ptr<HitEffect> newEffect = std::make_unique<HitEffect>();
		newEffect->Init(effectModel_, enemy->GetCenterPosition());

		std::unique_ptr<Effect> effect = std::move(newEffect);

		effectManager_->AddEffect(effect);
	}

}

void Hammer::SetEffect(EffectManager* effectManager, Model* hitEffectModel) {
	assert(effectManager && hitEffectModel);
	effectManager_ = effectManager;
	effectModel_ = hitEffectModel;
}

Vector3f Hammer::GetCenterPosition() const {
	const Vector3f offset = { 0.0f, 13.0f, 0.0f };

	return Matrix::Transform(offset, localTransform_.matWorld_);

}
