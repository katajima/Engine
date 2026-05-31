#pragma once

#include "DirectXGame/engine/3d/Object/Object3d.h"

namespace Engine {
	/// <summary>
	/// 通常モデル用Entity。既存Object3dの設定済み派生として扱う。
	/// </summary>
	class ModelEntity : public Object3d {
	public:
		void Initialize(EntityManager* entityManager, PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
		{
			Object3d::Initialize(entityManager, ObjectModelType::kNormal, rasterizerType);
		}
	};

	/// <summary>
	/// スカイボックス用Entity。
	/// </summary>
	class SkyBoxEntity : public Object3d {
	public:
		void Initialize(EntityManager* entityManager, PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
		{
			Object3d::Initialize(entityManager, ObjectModelType::kSkyBox, rasterizerType);
		}
	};

	/// <summary>
	/// Ocean用Entity。
	/// </summary>
	class OceanEntity : public Object3d {
	public:
		void Initialize(EntityManager* entityManager, PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
		{
			Object3d::Initialize(entityManager, ObjectModelType::kOcean, rasterizerType);
		}
	};
}
