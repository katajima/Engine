#pragma once
#include<cstdint>
#include "string"

//マテリアルデータ
struct MaterialData {
	// テクスチャ数
	uint32_t texDiffuseNum = 0;


	std::string textuerFilePath;
	std::string textuerNormalFilePath;
	std::string textuerSpeculerFilePath;

	//テクスチャ番号
	uint32_t textureIndex = 0;
	uint32_t textureNormalIndex = 0;
	uint32_t textuerSpeculerIndex = 0;

};





