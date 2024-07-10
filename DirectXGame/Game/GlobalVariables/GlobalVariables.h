#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <cstdint>
#include <variant>
#include <map>
#include <string>

// math
#include "Vector3.h"

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
	//! @brief 値の設定(int)
	void SetValue(const std::string& groupName, const std::string& key, int value);

	//! @brief 値の設定(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);

	//! @brief 値の設定(Vector3f)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3f& value);

	static GlobalVariables* GetInstance();

private:
	
	//-----------------------------------------------------------------------------------------
	// using
	//-----------------------------------------------------------------------------------------
	using Item = std::variant<int32_t, float, Vector3f>; //!< 必要であればpublicにする

	////////////////////////////////////////////////////////////////////////////////////////////
	// Group structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Group {
		std::map<std::string, Item> items;
	};

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	std::map<std::string, Group> datas_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	GlobalVariables() = default;
	~GlobalVariables() = default;

	// コピーの禁止
	GlobalVariables(const GlobalVariables&) = delete;
	void operator=(const GlobalVariables&) = delete;

};

// 頻繁に使うのでInstanceを確保しておく
static GlobalVariables* globalVariables = GlobalVariables::GetInstance();