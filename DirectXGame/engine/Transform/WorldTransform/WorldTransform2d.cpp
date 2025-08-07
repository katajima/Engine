#include "WorldTransform2d.h"

void WorldTransform2d::Initialize()
{
	scale_ = { 1,1};
	rotate_ = {};
	translate_ = { 0,0};

	isPearent = false;

	worldMat_ = MakeIdentity3x3();
	worldPreMat_ = MakeIdentity3x3();
}


void WorldTransform2d::Update() {
	worldPreMat_ = worldMat_;


	worldMat_ = MakeAffineMatrix(scale_, rotate_, translate_);

	// 親がいれば
	if (parent_) {
		worldMat_ = worldMat_ * parent_->worldMat_;
	}
	if (isPearent) {
		worldMat_ = Multiply(worldMat_, parentMatrix_);
	}
};