#pragma once
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<vector>
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/Effect/Trail/TrailEffect.h"
class DirectXCommon;
class EffectComponent
{
public:
	// 初期化
	void Init(DirectXCommon* dxCommon);
	// エミッター追加
	void AddEmitter(const std::string& name);
	// プリミティブ追加
	void AddPrimitive(const std::string& name);
	// トレイル追加
	void AddTrail(const std::string& name);

private:



	// トレイルエフェクト
	std::map<std::string,std::unique_ptr<TrailEffect>> trailEffects_;


private:
	DirectXCommon* dxCommon_ = nullptr;
};

