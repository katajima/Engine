#include "WorldTransform2d.h"

void WorldTransform2d::Initialize()
{
	// SRT
	scale_ = { 1,1};
	rotate_ = {};
	translate_ = { 0,0};

	isPearent = false;

	// 単位行列
	worldMat_ = MakeIdentity3x3();
	worldPreMat_ = MakeIdentity3x3();
}


void WorldTransform2d::Update() {
	// 前の行列設定
	worldPreMat_ = worldMat_;

	// アフィン変換
	worldMat_ = MakeAffineMatrix(scale_, rotate_, translate_);

	// 親がいれば
	if (parent_) {
		worldMat_ = worldMat_ * parent_->worldMat_;
	}
	if (isPearent) {
		worldMat_ = Multiply(worldMat_, parentMatrix_);
	}
};