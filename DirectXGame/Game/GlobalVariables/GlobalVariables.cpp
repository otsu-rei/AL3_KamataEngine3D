#include "GlobalVariables.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <imgui.h>
#include <json.hpp>
#include <fstream>
#include <windows.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using json = nlohmann::json;

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

		ImGui::Separator();

		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string msg = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, msg.c_str(), "GrobalVariables class", 0);
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

void GlobalVariables::SaveFile(const std::string& groupName) {
	
	auto it = datas_.find(groupName);
	assert(it != datas_.end()); //!< グループの未登録

	json root;
	root = json::object();

	// jsonオブジェクトの登録
	root[groupName] = json::object();

	for (auto& itItem : it->second.items) {

		// 項目名を取得
		const std::string& itemName = itItem.first;

		// 項目の参照
		Item& item = itItem.second;

		if (std::holds_alternative<int32_t>(item)) { //!< int32_tの場合
			root[groupName][itemName] = std::get<int32_t>(item);

		} else if (std::holds_alternative<float>(item)) { //!< floatの場合
			root[groupName][itemName] = std::get<float>(item);

		} else if (std::holds_alternative<Vector3f>(item)) { //!< Vector3fの場合
			Vector3f value = std::get<Vector3f>(item);
			root[groupName][itemName] = json::array({value.x, value.y, value.z});
		}
	}

	// ファイルに書き込む
	std::filesystem::path dir(kDirectoryPath_);

	if (!std::filesystem::exists(kDirectoryPath_)) { //!< ディレクトリが無ければ作成
		std::filesystem::create_directories(kDirectoryPath_);
	}

	// 書き込むjsonの生成
	std::string filePath = kDirectoryPath_ + groupName + ".json";
	
	std::ofstream ofs;
	ofs.open(filePath);

	if (ofs.fail()) { //!< ファイルが開けなかった場合
		// ログwindowを出す
		std::string msg = "Failed: open data file for write.";
		MessageBoxA(nullptr, msg.c_str(), "GlobalVariables class", 0);
		assert(false);
		return;
	}

	// ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

GlobalVariables* GlobalVariables::GetInstance() { 
	static GlobalVariables instance;
	return &instance;
}
