#pragma once

// C++
#include"memory"
#include <future>
#include <mutex>
// engine
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/effect/Ocean/OceanManager.h"
#include"DirectXGame/engine/3d/Object/Object3dInstanceManager.h"
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

		~EntityManager();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* directXCommon);
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGui
		/// </summary>
		void UpdateImgui();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// オブジェクトクリーン
		/// </summary>
		void ObjectClean();
		/// <summary>
		/// オブジェクト描画
		/// </summary>
		void ObjectDraw();
		/// <summary>
		/// シャドウマップ用の深度描画
		/// </summary>
		void DrawShadowMap(ShadowMap* shadowMap);

	public: //セッター
		/// <summary>
		/// ゲーム全体で共有するオーディオマネージャーを設定する。
		/// </summary>
		void SetAudioManager(AudioManager* audioManager) { audioManager_ = audioManager; }
		/// <summary>
		/// オブジェクト3D生成(名前、タグ、モデルタイプ、位置、カメラ)
		/// </summary>
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

		/// <summary>
		/// タグでの削除
		/// </summary>
		void EraseObject3DByTag(const std::string& tag) {
			std::erase_if(entities_, [&](const std::unique_ptr<Entity>& o) {
				return  o->GetNameTag() == tag;
				});
		}

	public: //ゲッター
		/// <summary>
		/// 海マネージャー取得
		/// </summary>
		OceanManager* GetOceanManager() { return oceanManager_.get(); };
		/// <summary>
		/// オブジェクトインスタンスマネージャー取得
		/// </summary>
		Object3dInstanceManager* GetObject3dInstanceManager() { return object3dInstanceManager_.get(); }
		/// <summary>
		/// オブジェクト共通クラス取得
		/// </summary>
		Object3dCommon* GetObject3dCommon() { return object3dCommon_.get(); }
		/// <summary>
		/// スカイボックス共通クラス取得
		/// </summary>
		SkyBoxCommon* GetSkyBoxCommon() { return skyBoxCommon_.get(); }
		/// <summary>
		/// ライトマネージャー取得
		/// </summary>
		LightManager* GetLightManager() { return lightManager_.get(); }
		/// <summary>
		/// スキニング共通クラス取得
		/// </summary>
		SkinningCommon* GetSkinningCommon() { return skinningCommon_.get(); }
		/// <summary>
		/// カメラ共通クラス取得
		/// </summary>
		CameraCommon* GetCameraCommon() { return cameraCommon_.get(); }
		/// <summary>
		/// ライン共通クラス取得
		/// </summary>
		LineCommon* Get3DLineCommon() { return lineCommon_.get(); }
		/// <summary>
		/// プリミティブ共通クラス取得
		/// </summary>
		PrimitiveCommon* GetPrimitiveCommon() { return primitiveCommon_.get(); }
		/// <summary>
		/// エフェクトマネージャークラス取得
		/// </summary>
		EffectManager* GetEffectManager() { return effectManager_.get(); }
		/// <summary>
		/// コンボやゲームオブジェクトから共有音源へアクセスする。
		/// </summary>
		AudioManager* GetAudioManager() { return audioManager_; }
	public:
		/// <summary>
		/// スプライト共通クラス取得
		/// </summary>
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
		std::unique_ptr<Object3dInstanceManager> object3dInstanceManager_ = nullptr;

		// オブジェクト
		std::unique_ptr<Object3dCommon> object3dCommon_ = nullptr;

		// スカイボックス
		std::unique_ptr<SkyBoxCommon> skyBoxCommon_ = nullptr;

		// ライト
		std::unique_ptr<LightManager> lightManager_ = nullptr;

		// スキニング
		std::unique_ptr<SkinningCommon> skinningCommon_ = nullptr;

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
