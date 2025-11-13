#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	// SRT
	scale_ = { 1,1,1 };
	rotate_ = {};
	translate_ = { 0,0,0 };

	isPearent = false;

	// 単位行列
	worldMat_ = MakeIdentity4x4();
	worldPreMat_ = MakeIdentity4x4();
}

void WorldTransform::Update()
{
	// 一個前の行列
	worldPreMat_ = worldMat_;


	// アフィン変換
	worldMat_ = MakeAffineMatrix(scale_, rotate_, translate_);

	// 親がいれば
	if (parent_) {
		worldMat_ = worldMat_ * parent_->worldMat_;
	}
	if (isPearent) {
		worldMat_ = Multiply(worldMat_,parentMatrix_);
	}
}
