#include "GlobalVariables.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalVariables class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GlobalVariables::Update() {

	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar()) {
		return;
	}
	
	//!< 各gruopの取得
	for (auto& itGroup : datas_) {

		// グループ名の取得
		const std::string& groupName = itGroup.first;

		// グループの参照取得
		Group& group = itGroup.second;

		if (!ImGui::BeginMenu(groupName.c_str())) {
			continue;
		}

		//!< 各itemの取得
		for (auto& itItem : group.items) {
			
			// 項目名の取得
			const std::string& itemName = itItem.first;

			// valueの取得
			Item& item = itItem.second;

			
			if (std::holds_alternative<int32_t>(item)) { //!< int32_tの場合
				int32_t* ptr = std::get_if<int32_t>(&item);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);

			} else if (std::holds_alternative<float>(item)) { //!< floatの場合
				float* ptr = std::get_if<float>(&item);
				ImGui::SliderFloat(itemName.c_str(), ptr, 0.0f, 100.0f); //!< 仮で設定

			} else if (std::holds_alternative<Vector3f>(item)) { //!< Vector3fの場合
				Vector3f* ptr = std::get_if<Vector3f>(&item);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.02f); //!< 仮で設定
				
			}
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();
}

void GlobalVariables::CreateGroup(const std::string& groupName) {
	datas_[groupName]; //!< 要素の追加
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, int value) {
	// datasの中身の参照
	Group& group = datas_[groupName];

	// 項目のデータ設定
	group.items[key] = value;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	// datasの中身の参照
	Group& group = datas_[groupName];

	// 項目のデータ設定
	group.items[key] = value;
}

void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const Vector3f& value) {
	// datasの中身の参照
	Group& group = datas_[groupName];

	// 項目のデータ設定
	group.items[key] = value;
}

GlobalVariables* GlobalVariables::GetInstance() { 
	static GlobalVariables instance;
	return &instance;
}
