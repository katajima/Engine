#pragma once
#include "DirectXGame/engine/Effect/EffectComponent.h"

/// <summary>
/// エフェクト外部保存用データ
/// </summary>
struct EffectGlobalData {
	// パーティクル名
	std::string particleName; 
	// フラグ系データ
	EmitData::IsFlag isFlag;
	// 拡縮データ
	EmitData::TopBottom topBottom;
	// エミッタ形状内のどこから出すか
	EmitData::EmitType emitType = EmitData::EmitType::kRandom;
	// エミット位置に応じた初速方向
	EmitData::DirectionType directionType = EmitData::DirectionType::kRandom;
	// 出現データ
	Engine::EffectEmitData emitData;
	// UVトランスフォーム
	Transform uvTransformVeloctiy{ {},{},{0.0f,0.0f,0.0f} };
	// αクリッピング
	float alphaClipping = 0.5f; 
	// ライティング
	int enableLighting = false;
	// 発生頻度
	float frequency = 0.1f;		
	// エミッタ形状タイプ
	EmitterShapeType shapeType = EmitterShapeType::POINT; 


	// AABB用
	Vector3 rangeMin = { -1.0f,-1.0f,-1.0f };
	Vector3 rangeMax = { 1.0f,1.0f,1.0f };

	// 球用
	float radius = 1.0f;

	// 円用
	int segment = 3;

	// 線用
	Vector3 lineStart = {};
	Vector3 lineEnd = { 1.0f,0.0f,0.0f };

	// スプライン用
	std::vector<Vector3> splinePoints = {};

	// 三角形用
	Triangle triangle = {};

};
