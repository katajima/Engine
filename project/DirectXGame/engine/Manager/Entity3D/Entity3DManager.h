#pragma once

// C++
#include"memory"
#include <future>
#include <mutex>
// engine
#include"DirectXGame/engine/effect/Ocean/OceanManager.h"
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include "DirectXGame/engine/SkyBox/SkyBoxCommon.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/Skinning/Skinning.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Primitive/PrimitiveCommon.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"


#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"

struct SortResult {
	Object3d* ptr;
	ObjectDrawType drawType;
	float alpha;
	bool isSkin;
};

class DirectXCommon;
class Entity3DManager
{
public:



	// 初期化
	void Initialize(DirectXCommon* directXCommon);

	// ImGui
	void UpdateImgui();

	// 
	void Update();

	//void CheckAllCollisions();

	void ObjectClean();

	void ObjectDraw();

public: //セッター

	//void SetEntity3D(std::unique_ptr<Object3d> entity3D);

	// オブジェクト3D生成(名前、タグ、モデルタイプ、位置、カメラ)
	Object3d* CreateObject3D(const std::string& name, ObjectModelType type, const Vector3& pos, Camera* camera) {
		auto object = std::make_unique<Object3d>();
		object->Initialize(this, type);
		object->SetName(name);
		object->GetWorldTransform().translate_ = pos;
		object->SetCamera(camera);

		Object3d* raw = object.get();
		object3d.push_back(std::move(object));
		return raw;
	}

	template<typename T>
	Object3d* CreatePrimitiveObject3D(const std::string& name,
		const std::string& texturePath,
		Camera* camera,
		PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
	{
		// プリミティブ生成
		std::unique_ptr<BasePrimitive> primitive = std::make_unique<T>();
		primitive->Initialize(GetPrimitiveCommon(), texturePath);

		// Object3d 生成
		auto object = std::make_unique<Object3d>();
		object->Initialize(this, ObjectModelType::kPrimitive, rasterizerType);
		object->SetName(name);
		object->SetCamera(camera);
		object->SetPrimitive(std::move(primitive)); // 所有権を渡す

		Object3d* raw = object.get();
		object3d.push_back(std::move(object));
		return raw;
	}

	// タグでの削除
	void EraseObject3DByTag(const std::string& tag) {
		std::erase_if(object3d, [&](const std::unique_ptr<Object3d>& o) {
			return  o->GetNameTag() == tag;
			});
	}

public: //ゲッター

	OceanManager* GetOceanManager() { return oceanManager_.get(); };

	Object3dInstansManager* GetObject3dInstansManager() { return object3dInstansManager_.get(); }

	Object3dCommon* GetObject3dCommon() { return object3dCommon_.get(); }

	SkyBoxCommon* GetSkyBoxCommon() { return skyBoxCommon_.get(); }

	LightManager* GetLightManager() { return lightManager_.get(); }

	SkinningConmmon* GetSkinningConmmon() { return skinningCommon_.get(); }

	CameraCommon* GetCameraCommon() { return cameraCommon_.get(); }

	LineCommon* Get3DLineCommon() { return lineCommon_.get(); }

	PrimitiveCommon* GetPrimitiveCommon() { return primitiveCommon_.get(); }

	EffectManager* GetEffectManager() { return effectManager_.get(); }
public:
	//void SetCollisionManager(CollisionManager* collisionManager) {collisionManager_ = collisionManager;}


private:
	//CollisionManager* collisionManager_;


	std::vector<std::unique_ptr<Object3d>> object3d;

	std::vector<Object3d*> opaqueObjects;
	std::vector<Object3d*> transparentObjects01;
	std::vector<Object3d*> transparentObjects02;
	std::vector<Object3d*> transparentObjects03;

private:
	// DirectX
	DirectXCommon* directXCommon_;

	// オーシャンシェーダー
	std::unique_ptr<OceanManager> oceanManager_ = nullptr;

	// インスタンシング描画
	std::unique_ptr<Object3dInstansManager> object3dInstansManager_ = nullptr;

	// オブジェクト
	std::unique_ptr<Object3dCommon> object3dCommon_ = nullptr;

	// スカイボックス
	std::unique_ptr<SkyBoxCommon> skyBoxCommon_ = nullptr;

	// ライト
	std::unique_ptr<LightManager> lightManager_ = nullptr;

	// スキニング
	std::unique_ptr<SkinningConmmon> skinningCommon_ = nullptr;

	// カメラコモン
	std::unique_ptr <CameraCommon> cameraCommon_ = nullptr;

	// ラインコモン
	std::unique_ptr <LineCommon> lineCommon_ = nullptr;

	// プリミティブ
	std::unique_ptr<PrimitiveCommon> primitiveCommon_ = nullptr;

	// エフェクトマネージャー
	std::unique_ptr<EffectManager> effectManager_ = nullptr;

};

