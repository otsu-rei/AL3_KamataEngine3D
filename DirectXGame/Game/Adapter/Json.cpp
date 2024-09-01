#include "Json.h"

//=========================================================================================
// static variables
//=========================================================================================

const std::string JsonAdapter::directory_ = "./Resources/Json/";

////////////////////////////////////////////////////////////////////////////////////////////
// JsonAdapter methods
////////////////////////////////////////////////////////////////////////////////////////////

Json JsonAdapter::ReadJson(const std::string& path) {
	Json result;

	// ファイルパスの生成
	std::string filePath = directory_ + path;

	// ファイル
	std::ifstream ifs;
	ifs.open(filePath);

	assert(ifs.is_open());

	ifs >> result;

	ifs.close();

	return result;
}

void JsonAdapter::WriteJson(const std::string& path, const Json& root) {
	// ファイルパス生成
	std::string filePath = directory_ + path;

	std::ofstream ofs;
	ofs.open(filePath);

	/*assert(!ofs.fail());*/

	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void JsonAdapter::OverwriteJson(const std::string& path, const Json& root) {

	std::string filePath = directory_ + path;

	Json saveData;

	// 旧データの回収
	std::ifstream ifs(filePath);
	if (ifs.is_open()) {
		ifs >> saveData;
		ifs.close();
	}
	
	// 新データの書き込み
	saveData.update(root);

	std::ofstream ofs(filePath);

	ofs << std::setw(4) << saveData << std::endl;

	ofs.close();
}

Json JsonAdapter::ToJson(const Vector3f& v) { 
	return Json {
	    {"x", v.x},
        {"y", v.y},
        {"z", v.z},
	};
}

Vector3f JsonAdapter::JsonTo(const Json& object) {
	return { object["x"], object["y"], object["z"] };
}
