#include "BulletData.h"

void BulletData::Initialize(HitBoxUseType useType, ProjectileType projectileType, const HitBoxCollData& hitBoxData){
	projectileType_ = projectileType;
	data_.hitBoxUseType_ = useType;
	hitBoxdata_ = hitBoxData;

}
