#pragma once
#include<cstdint>
#include "string"

//マテリアルデータ
struct MaterialData {
	// テクスチャ数
	uint32_t texDiffuseNum = 0;


	std::string textureFilePath;
	std::string textureNormalFilePath;
	std::string textureSpecularFilePath;

	//テクスチャ番号
	uint32_t textureIndex = 0;
	uint32_t textureNormalIndex = 0;
	uint32_t textureSpecularIndex = 0;

};





