#pragma once

// C++
#include"memory"
#include <future>
#include <mutex>
// engine
#include"DirectXGame/engine/2d/SpriteCommon.h"
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


#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include <DirectXGame/engine/Effect/Primitive/Primitive.h>
#include "DirectXGame/engine/Entity/Entity.h"
#include "DirectXGame/engine/Entity/ModelEntity.h"
#include "DirectXGame/engine/Entity/PrimitiveEntity.h"
#include "DirectXGame/engine/3d/Object/Object3d.h"


#include"DirectXGame/engine/collider/3d/CollisionManager.h"

namespace Engine {
	// 前方宣言
	class DirectXCommon;
	class ShadowMap;
	class TrailEffect;
	class AudioManager;

	/// <summary>
	/// エンティティマネージャー
	/// </summary>
	class EntityManager
	{
	public:



		// 初期化
		void Initialize(DirectXCommon* directXCommon);

		// ImGui
		void UpdateImgui();

		// 更新
		void Update();

		// オブジェクトクリーン
		void ObjectClean();
		// オブジェクト描画
		void ObjectDraw();
		// シャドウマップ用の深度描画
		void DrawShadowMap(ShadowMap* shadowMap);

	public: //セッター
		// ゲーム全体で共有するオーディオマネージャーを設定する。
		void SetAudioManager(AudioManager* audioManager) { audioManager_ = audioManager; }
		// オブジェクト3D生成(名前、タグ、モデルタイプ、位置、カメラ)
		Object3d* CreateObject3D(const std::string& name, ObjectModelType type, const Vector3& pos, Engine::Camera* camera) {
			std::unique_ptr<Object3d> object;
			switch (type)
			{
			case ObjectModelType::kNormal:	// モデル描画オブジェクト
			{
				auto modelEntity = std::make_unique<ModelEntity>();
				modelEntity->Initialize(this);
				object = std::move(modelEntity);
				break;
			}
			case ObjectModelType::kSkyBox:	// スカイボックス描画オブジェクト
			{
				auto skyBoxEntity = std::make_unique<SkyBoxEntity>();
				skyBoxEntity->Initialize(this);
				object = std::move(skyBoxEntity);
				break;
			}
			case ObjectModelType::kOcean:	// 波描画オブジェクト
			{
				auto oceanEntity = std::make_unique<OceanEntity>();
				oceanEntity->Initialize(this);
				object = std::move(oceanEntity);
				break;
			}
			default:
				object = std::make_unique<Object3d>();
				object->Initialize(this, type);
				break;
			}
			object->SetName(name);
			object->GetWorldTransform().translate_ = pos;
			object->SetCamera(camera);

			Object3d* raw = object.get();
			entities_.push_back(std::move(object));
			return raw;
		}
		// トレイル生成。Object3dには所有させず、独立したEntityとして管理する。
		TrailEffect* CreateTrailEffect(const std::string& name,
			const std::string& tex,
			float maxTime,
			WorldTransform& parent,
			Camera* camera,
			Color color = { 1,1,1,1 },
			Vector3 offsetStr = { 0,0.5f,0 },
			Vector3 offsetEnd = { 0,-0.5f,0 });
		// オブジェクト3D生成(プリミティブ)
		template<typename T>
		Object3d* CreatePrimitiveObject3D(const std::string& name,
			const std::string& texturePath,
			Camera* camera,
			PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK)
		{
			auto object = std::make_unique<PrimitiveEntity<T>>();
			object->Initialize(this, GetPrimitiveCommon(), texturePath, rasterizerType);
			object->SetName(name);
			object->SetCamera(camera);

			Object3d* raw = object.get();
			entities_.push_back(std::move(object));
			return raw;
		}

		// タグでの削除
		void EraseObject3DByTag(const std::string& tag) {
			std::erase_if(entities_, [&](const std::unique_ptr<Entity>& o) {
				return  o->GetNameTag() == tag;
				});
		}

	public: //ゲッター
		// 海マネージャー取得
		OceanManager* GetOceanManager() { return oceanManager_.get(); };
		// オブジェクトインスタンスマネージャー取得
		Object3dInstansManager* GetObject3dInstansManager() { return object3dInstansManager_.get(); }
		// オブジェクト共通クラス取得
		Object3dCommon* GetObject3dCommon() { return object3dCommon_.get(); }
		// スカイボックス共通クラス取得
		SkyBoxCommon* GetSkyBoxCommon() { return skyBoxCommon_.get(); }
		// ライトマネージャー取得
		LightManager* GetLightManager() { return lightManager_.get(); }
		// スキニング共通クラス取得
		SkinningConmmon* GetSkinningConmmon() { return skinningCommon_.get(); }
		// カメラ共通クラス取得
		CameraCommon* GetCameraCommon() { return cameraCommon_.get(); }
		// ライン共通クラス取得
		LineCommon* Get3DLineCommon() { return lineCommon_.get(); }
		// プリミティブ共通クラス取得
		PrimitiveCommon* GetPrimitiveCommon() { return primitiveCommon_.get(); }
		// エフェクトマネージャークラス取得
		EffectManager* GetEffectManager() { return effectManager_.get(); }
		// コンボやゲームオブジェクトから共有音源へアクセスする。
		AudioManager* GetAudioManager() { return audioManager_; }
	public:
		// スプライト共通クラス取得
		SpriteCommon* GetSpriteCommon() { return spriteCommon_.get(); }


	private:
		std::vector<std::unique_ptr<Entity>> entities_;

		// Update時に毎フレーム作り直す描画キュー。順番はカメラ奥行きから自動で決める。
		std::vector<Object3d*> opaqueObjects;
		std::vector<Object3d*> transparentObjects;

	private:
		// DirectX
		DirectXCommon* dxCommon = nullptr;
		// Frameworkが所有するオーディオマネージャーへの非所有ポインター。
		AudioManager* audioManager_ = nullptr;

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

	private:
		// スプライト共通クラス
		std::unique_ptr<SpriteCommon> spriteCommon_ = nullptr;

	};
}
