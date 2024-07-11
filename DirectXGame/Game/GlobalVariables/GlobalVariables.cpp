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
		for (auto& itItem : group) {
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
				ImGui::DragFloat3(itemName.c_str(), &ptr->x, 0.02f); //!< 仮で設定
				
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

void GlobalVariables::SaveFile(const std::string& groupName) {
	
	auto it = datas_.find(groupName);
	assert(it != datas_.end()); //!< グループの未登録

	json root;
	root = json::object();

	// jsonオブジェクトの登録
	root[groupName] = json::object();

	for (auto& itItem : it->second) {
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

void GlobalVariables::LoadFiles() {
	// ディレクトリの確認
	std::filesystem::path dir(kDirectoryPath_);
	if (!std::filesystem::exists(kDirectoryPath_)) { //!< ディレクトリが無ければ早期リターン
		return;
	}

	// ディレクトリの中にある全ファイルの読み込み
	std::filesystem::directory_iterator dirIt(kDirectoryPath_);
	for (const auto& entry : dirIt) {
		// ファイルパスの取得
		const std::filesystem::path& filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();

		if (extension.compare(".json") != 0) { //!< .jsonファイル以外はスキップ
			continue;
		}

		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	// 読み込むjsonファイルのフルパスの設定
	std::string filePath = kDirectoryPath_ + groupName + ".json";

	// ファイルを読み込み用に開く
	std::ifstream ifs;
	ifs.open(filePath);

	if (ifs.fail()) { //!< ファイルが開けなかった場合
		// ログwindowを出す
		std::string msg = "Failed: open data file for read.";
		MessageBoxA(nullptr, msg.c_str(), "GlobalVariables class", 0);
		assert(false);
		return;
	}

	json root;

	// jsonデータ構造体に展開
	ifs >> root;

	ifs.close();

	// グループの検索
	json::iterator itGroup = root.find(groupName);
	assert(itGroup != root.end()); //!< 未登録の確認

	for (auto itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名の取得
		const std::string& itemName = itItem.key();

		if (itItem->is_number_integer()) { //!< int32_tの場合
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);

		} else if (itItem->is_number_float()) { //!< floatの場合
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));

		} else if (itItem->is_array() && itItem->size() == 3) { //!< 配列 && 配列サイズが3の場合
			// Vector3fとして扱う
			Vector3f value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}
	}
}

GlobalVariables* GlobalVariables::GetInstance() { 
	static GlobalVariables instance;
	return &instance;
}
