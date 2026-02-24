#include "BulletData.h"

void BulletData::Initialize(HitBox::UseType useType, ProjectileType projectileType, const HitBox::CollData& hitBoxData){
	projectileType_ = projectileType;
	data_.hitBoxUseType_ = useType;
	hitBoxdata_ = hitBoxData;

}
