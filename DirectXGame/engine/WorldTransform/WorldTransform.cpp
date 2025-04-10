#include "WorldTransform.h"

void WorldTransform::Initialize()
{
	scale_ = { 1,1,1 };
	rotate_ = {};
	translate_ = { 0,0,0 };



	worldMat_ = MakeIdentity4x4();
	worldPreMat_ = MakeIdentity4x4();
}

void WorldTransform::Update()
{
	isChange_ = false;
	worldPreMat_ = worldMat_;

	if (o_scale_ != scale_) {
		o_scale_ = scale_;
		isChange_ = true;
	}
	if (o_rotate_ != rotate_) {
		o_rotate_ = rotate_;
		isChange_ = true;
	}
	if (o_translate_ != translate_) {
		o_translate_ = translate_;
		isChange_ = true;
	}
	if (isChange_) {
		worldMat_ = MakeAffineMatrix(scale_, rotate_, translate_);
	}
	// 親がいれば
	if (parent_) {
		worldMat_ = worldMat_ * parent_->worldMat_;
	}
}
