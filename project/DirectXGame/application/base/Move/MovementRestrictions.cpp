#include "MovementRestrictions.h"

void MovementRestrictions::Initialize(Vector3 min, Vector3 max)
{
	min_ = min;
	max_ = max;
}

void MovementRestrictions::Update(WorldTransform& world)
{
	if (world.translate_.x > max_.x) {
		world.translate_.x = max_.x;
	}
	if (world.translate_.x < min_.x) {
		world.translate_.x = min_.x;
	}
	if (world.translate_.z > max_.z) {
		world.translate_.z = max_.z;
	}
	if (world.translate_.z < min_.z) {
		world.translate_.z = min_.z;
	}
}
