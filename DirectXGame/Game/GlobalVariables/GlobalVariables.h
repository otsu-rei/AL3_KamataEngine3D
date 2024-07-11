#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <variant>
#include <unordered_map>
#include <string>
#include <cassert>

// math
#include "Vector3.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using Item = std::variant<int32_t, float, Vector3f>;
using Group = std::unordered_map<std::string, Item>;

//-----------------------------------------------------------------------------------------
// concept
//-----------------------------------------------------------------------------------------
template <typename T>
concept ItemType = std::same_as<T, int32_t> || std::same_as<T, float> || std::same_as<T, Vector3f>;

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalVariables class
////////////////////////////////////////////////////////////////////////////////////////////
class GlobalVariables {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Update();

	//! @brief グループの生成
	void CreateGroup(const std::string& groupName);

	//* setters *//
	//! @brief 値の設定
	template<ItemType T>
	void SetValue(const std::string& groupName, const std::string& key, const T& value);

	//! @brief 項目の追加
	template <ItemType T>
	void AddItem(const std::string& groupName, const std::string& key, const T& value);

	//* getter *//
	//! @brief 値の取得
	template<ItemType T>
	T GetValue(const std::string& groupName, const std::string& key) const;

	//* file operator *//
	//! @brief ファイルに書き出し
	void SaveFile(const std::string& groupName);

	//! @brief ディレクトリの全ファイルの読み込み
	void LoadFiles();

	//! @brief ファイルの読み込み
	void LoadFile(const std::string& groupName);

	//* instance *//
	static GlobalVariables* GetInstance();

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	std::unordered_map<std::string, Group> datas_;
	//!< mapをhash mapに変更
	
	const std::string kDirectoryPath_ = "Resources/GlobalVariables/";

	//=========================================================================================
	// private methods
	//=========================================================================================

	GlobalVariables() = default;
	~GlobalVariables() = default;

	// コピーの禁止
	GlobalVariables(const GlobalVariables&) = delete;
	void operator=(const GlobalVariables&) = delete;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GlobalVariables class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<ItemType T>
void GlobalVariables::SetValue(const std::string& groupName, const std::string& key, const T& value) {
	// datasの中身の参照
	Group& group = datas_[groupName];

	// 項目のデータ設定
	group[key] = value;
}

template<ItemType T>
void GlobalVariables::AddItem(const std::string& groupName, const std::string& key, const T& value) {

	auto it = datas_[groupName].find(key);
	if (it != datas_[groupName].end()) { //!< 項目が登録済みの場合
		return;
	}

	// 登録されてない場合, 新しく登録
	SetValue(groupName, key, value);
}

template<ItemType T>
T GlobalVariables::GetValue(const std::string& groupName, const std::string& key) const {

	auto itGroup = datas_.find(groupName);
	assert(itGroup != datas_.end()); //!< 未登録の確認

	// グループの参照の取得
	const Group& group = itGroup->second;

	auto itItem = group.find(key);
	assert(itItem != group.end()); //!< 未登録の確認

	// 項目の参照
	const Item& item = itItem->second;

	if (!std::holds_alternative<T>(item)) {
		assert(false); //!< itemと取得型が一致しない
	}

	return std::get<T>(item);
}

// 頻繁に使うのでInstanceを確保しておく
static GlobalVariables* globalVariables = GlobalVariables::GetInstance();