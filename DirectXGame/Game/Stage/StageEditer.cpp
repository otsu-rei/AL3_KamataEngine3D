#include "StageEditer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const Vector3f StageEditer::kGrabitys_[kCountOfGrabityType] = {
	{-1.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
	{0.0f, -1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 0.0f, -1.0f},
	{0.0f, 0.0f, 1.0f},
};

const LPCSTR StageEditer::kGrabityNames_[kCountOfGrabityType] = {
	"mX", "pX",
	"mY", "pY",
	"mZ", "pZ",
};

////////////////////////////////////////////////////////////////////////////////////////////
// StageEditer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void StageEditer::Init(GameCamera* camera) {

	goal_ = std::make_unique<Goal>();
	goal_->Init({0.0f, 0.0f, 0.0f}, kGrabitys_[goalGrabityType_]);

	player_ = std::make_unique<Player>();
	player_->Init();
	player_->SetPosition(playerStartPosition_);
	player_->SetGameCamera(camera);

}

void StageEditer::Term() {}

void StageEditer::Update() {

	player_->Update();

	boxes_.remove_if([](auto& box) {
		return box->IsDelete();
	});
}

void StageEditer::ReactionUpdate() {

	goal_->Update();

}

void StageEditer::Draw(const ViewProjection& viewProj) {

	player_->Draw(viewProj);
	goal_->Draw(viewProj);

	for (auto& box : boxes_) {
		box->Draw(viewProj);
	}
}

void StageEditer::UpdateImGui() {

	if (ImGui::TreeNode("StageEditor")) {

		ImGui::SeparatorText("files");

		static CHAR buf[256] = "stage1.json";

		ImGui::InputTextWithHint("filename", "test hint", buf, 256);

		if (ImGui::Button("output")) {
			// TODO
			OutputJson(buf);
		}

		ImGui::SameLine();

		if (ImGui::Button("load")) {
			LoadJson(buf);
		}

		//* objects
		ImGui::SeparatorText("objects");

		if (ImGui::Button("create")) {
			std::unique_ptr<Box> newBox = std::make_unique<Box>();
			newBox->Init({4.0f, 4.0f, 4.0f}, {0.0f, -4.0f, 0.0f});

			boxes_.emplace_back(std::move(newBox));
		}

		ImGui::SameLine();

		if (ImGui::Button("delete all")) {
			boxes_.clear();
		}

		ImGui::Spacing();

		if (ImGui::TreeNode("player")) {
			ImGui::DragFloat3("startPosition", &playerStartPosition_.x, 0.01f);
			const Vector3f& position = player_->GetColliderPosition();

			ImGui::Text("position x: %f, y: %f, z: %f", position.x, position.y, position.z);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Goal")) {

			ImGui::SliderInt("Grabity", reinterpret_cast<int*>(&goalGrabityType_), mX, pZ, kGrabityNames_[goalGrabityType_]);
			goal_->SetGravity(kGrabitys_[goalGrabityType_]);

			goal_->UpdateImGui();

			ImGui::TreePop();
		}

		ImGui::SeparatorText("boxes");

		for (auto& box : boxes_) {

			std::string label = "box##" + std::format("{:p}", reinterpret_cast<void*>(box.get()));

			if (ImGui::TreeNode(label.c_str())) {
				box->SetImGuiCommand();

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

}

void StageEditer::OutputJson(const std::string& path) {
	//!< path指定で

	Json root = {};
	root = Json::object();

	//* player情報の登録
	root["Player"] = Json::object();
	root["Player"]["start"] = JsonAdapter::ToJson(playerStartPosition_);
	
	//* goalの登録
	root["Goal"] = Json::object();

	root["Goal"]["position"] = JsonAdapter::ToJson(goal_->GetColliderPosition());
	root["Goal"]["gravity"]  = static_cast<uint32_t>(goalGrabityType_);

	
	//* Box情報の登録
	root["Boxes"] = Json::array();

	// boxDataを登録
	for (auto& box : boxes_) {
		BoxData data = {
			.scale    = box->GetScale(),
			.position = box->GetColliderPosition()
		};

		root["Boxes"].emplace_back(data.ToJson());
	}


	JsonAdapter::WriteJson(path, root);

}

void StageEditer::LoadJson(const std::string& path) {

	Json root = JsonAdapter::ReadJson(path);

	//* Player情報の取得
	auto playerIt = root.find("Player");
	assert(playerIt != root.end());

	playerStartPosition_ = JsonAdapter::JsonTo((*playerIt)["start"]);
	player_->SetPosition(playerStartPosition_);
	player_->SetGrabity({0.0f, -1.0f, 0.0f});

	//* Goal情報の取得
	auto goalIt = root.find("Goal");
	assert(goalIt != root.end()); //!< Goalが見つからなかった

	goalGrabityType_ = static_cast<GravityType>((*goalIt)["gravity"]);

	goal_->SetGravity(kGrabitys_[goalGrabityType_]);
	goal_->SetPosition(JsonAdapter::JsonTo((*goalIt)["position"]));
	goal_->ResetFlag();

	//* box情報の取得

	auto boxesIt = root.find("Boxes");
	assert(boxesIt != root.end()); //!< Boxesが見つからなかった

	boxes_.clear(); //!< 一度要素の全削除

	for (const auto& boxData : (*boxesIt)) {
		std::unique_ptr<Box> newBox = std::make_unique<Box>();
		newBox->Init(JsonAdapter::JsonTo(boxData["scale"]), JsonAdapter::JsonTo(boxData["position"]));

		boxes_.emplace_back(std::move(newBox));
	}
}
