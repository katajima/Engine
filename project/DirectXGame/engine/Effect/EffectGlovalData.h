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
	// 出現データ
	Engine::EffectEmitData emitData;
	// UVトランスフォーム
	Transform uvTransformVeloctiy{};
	// αクリッピング
	float alphaClipping = 0.5f; 
	// ライティング
	int enableLighting = false;
	// 発生頻度
	float frequency = 0.1f;		
	// エミッタ形状タイプ
	EmitterShapeType shapeType = EmitterShapeType::POINT; 


	// AABB用
	Vector3 rangeMin = {};
	Vector3 rangeMax = {};

	// 球用
	float radius = 0.0f;

	// 円用
	int segment = 3;

	// 線用
	Vector3 lineStart = {};
	Vector3 lineEnd = {};

	// スプライン用
	std::vector<Vector3> splinePoints = {};

	// 三角形用
	Triangle triangle = {};

};
