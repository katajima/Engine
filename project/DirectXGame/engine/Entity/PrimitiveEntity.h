#pragma once

#include "DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"

namespace Engine {
	class PrimitiveCommon;

	/// <summary>
	/// プリミティブ用Entity。形状Tを内部で生成してObject3dへ渡す。
	/// </summary>
	template<typename T>
	class PrimitiveEntity : public Object3d {
	public:
		void Initialize(EntityManager* entityManager, PrimitiveCommon* primitiveCommon, const std::string& texturePath,
			PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
		{
			Object3d::Initialize(entityManager, ObjectModelType::kPrimitive, rasterizerType);

			auto primitive = std::make_unique<T>();
			primitive->Initialize(primitiveCommon, texturePath);
			SetPrimitive(std::move(primitive));
		}
	};
}
