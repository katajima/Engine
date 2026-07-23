#pragma once
#include"DirectXGame/engine/struct/Light.h"
#include "DirectXGame/engine/Entity/Entity.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/3d/Model/RenderComponent.h"
#include <DirectXGame/engine/Collider/ColliderData.h>
#include <future>
#include <cstdint>

// 前方宣言

namespace Engine {
	class EntityManager;
	class Object3dCommon;
	class SkinningCommon;
	class ImGuiManager;
	class SkyBoxCommon;
	class OceanManager;
	class RigidBodyComponent;
	class AnimationComponent;
	class ColliderComponent;
	class ContactRecord;
	class PrimitiveCommon;
	class EffectManager;
	class TransformComponent;
	class RenderComponent;
	class ShadowMap;

	class Camera;
	class Ocean;
	class SkyBox;
	class BasePrimitive;

	/// <summary>
	/// オブジェクトクラス
	/// </summary>
	class Object3d : public Entity
	{
	public:
		Object3d();
		~Object3d();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EntityManager* entity3DManager, ObjectModelType objectType = ObjectModelType::kNormal, PSOType rasterizerType = PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;

		/// <summary>
		/// 物理更新
		/// </summary>
		void RigidBodyUpdate() override;

		/// <summary>
		/// 描画通常
		/// </summary>
		void Draw() override;
		/// <summary>
		/// シャドウマップ用の深度描画
		/// </summary>
		void DrawShadowMap(ShadowMap* shadowMap) override;
	public:// セッター
		/// <summary>
		/// モデル設定(モデル)
		/// </summary>
		void SetModel(Model* model);
		/// <summary>
		/// モデル設定(モデル名での)
		/// </summary>
		void SetModel(const std::string& filePath);
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera);
		/// <summary>
		/// プリミティブ形状
		/// </summary>
		void SetPrimitive(std::unique_ptr<BasePrimitive> primitive);
		/// <summary>
		/// スカイボックス
		/// </summary>
		void SetSkyBox(SkyBox* skyBox);
		/// <summary>
		/// 波セット
		/// </summary>
		void SetOcean(Ocean* ocean);
		/// <summary>
		/// オブジェクト固有に映すカメラを使用するか設定
		/// </summary>
		void SetIsIndividualCamera(bool isIndividualCamera);
		/// <summary>
		/// メッシュ取得
		/// </summary>
		ModelMesh* GetMesh(int index);
		/// <summary>
		/// マテリアル取得
		/// </summary>
		Material* GetMaterial(int index);
		/// <summary>
		/// モデル取得
		/// </summary>
		Model* GetModel() const;
		/// <summary>
		/// プリミティブ取得
		/// </summary>
		BasePrimitive* GetPrimitive() const;
		/// <summary>
		/// 波取得
		/// </summary>
		Ocean* GetOcean() const;
		/// <summary>
		/// スカイボックス取得
		/// </summary>
		SkyBox* GetSkyBox() const;
		/// <summary>
		/// マテリアルインスタンス取得
		/// </summary>
		std::vector<MaterialInstance>& GetMaterialInstance();

		/// <summary>
		/// 描画するか設定
		/// </summary>
		void SetIsDraw(bool is);
		/// <summary>
		/// モデルのデバッグ用ImGui
		/// </summary>
		void DebugImguiModel();
		/// <summary>
		/// スキンモデルのデバッグ用
		/// </summary>
		void DebugImguiSkin();
		/// <summary>
		/// オブジェクトに使われているモデルの透明度取得
		/// </summary>
		float GetAlpha();
		/// <summary>
		/// 描画順自動ソート用に、使用中カメラの前方向へ投影した奥行きを取得
		/// </summary>
		float GetCameraSortDepth() const;

	private:
		// カメラ
		Camera* defaultCamera = nullptr;
		Camera* individualCamera = nullptr;
		// 個人的にカメラを使用するか
		bool isIndividualCamera_ = false;

		// トランスフォームデータ
		std::unique_ptr<Transformation> transformation = nullptr;



		// ImGuiを表示するか
		bool imguiFlag_ = false;

		// 物理更新するか
		bool isRigidUpdate_ = true;
	private: // コンポネント

		/// <summary>
		/// コライダー
		/// </summary>

		// コライダーコンポーネント
		std::unique_ptr<ColliderComponent> colliderComponent_;
		// コライダーコンポーネントをObject3d内で更新するかのフラグ
		bool isColliderComponenyUpdate_ = false;

		/// <summary>
		///  トランスフォーム
		/// </summary>

		std::unique_ptr<TransformComponent> transformComponent_ = nullptr;

		/// <summary>
		///  物理
		/// </summary>

		std::unique_ptr<RigidBodyComponent> rigidBodyComponent_ = nullptr;

		/// <summary>
		/// アニメーション
		/// </summary>

		std::unique_ptr<AnimationComponent> animationComponent_ = nullptr;

		/// <summary>
		/// 描画
		/// </summary>

		std::unique_ptr<RenderComponent> renderComponent_ = nullptr;

	public:
		/// <summary>
		/// コライダーコンポーネントを初期化
		/// </summary>
		void InitColliderComponent();
		/// <summary>
		/// Object3d内でコライダーコンポーネントを更新するか
		/// </summary>
		void SetIsUpdateColliderComponent(bool is);
		/// <summary>
		/// コライダーコンポーネントを取得
		/// </summary>
		ColliderComponent* GetColliderComponent();
		/// <summary>
		/// コライダーコンポーネントの接触情報を取得
		/// </summary>
		ContactRecord& GetContactRecord();

		/// <summary>
		/// トランスフォームコンポーネント
		/// </summary>
		TransformComponent* GetTransformComponent();
		/// <summary>
		/// ワールド座標
		/// </summary>
		Vector3 GetWorldPosition() const;
		/// <summary>
		/// １フレーム前のワールド座標
		/// </summary>
		Vector3 GetPreWorldPosition() const;
		/// <summary>
		/// ワールド座標
		/// </summary>
		WorldTransform& GetWorldTransform();
		WorldTransform* GetWorldTransformPtr() override;
		const WorldTransform* GetWorldTransformPtr() const override;
		/// <summary>
		/// 座標更新
		/// </summary>
		void UpdateWorldTransform();
		/// <summary>
		/// 向いている方向
		/// </summary>
		Vector3 ObjectDirection() const;
		/// <summary>
		/// スクリーン位置取得
		/// </summary>
		Vector2 GetScreenPosition();

		/// <summary>
		/// 物理
		/// </summary>

		/// <summary>
		/// 初期化
		/// </summary>
		void InitRigidBodyComponent();
		/// <summary>
		/// 物理取得
		/// </summary>
		RigidBodyComponent* GetRigidBodyComponent();
		//
		void SetIsRigidUpdate(bool isRigidUpdate);

		/// <summary>
		/// アニメーション
		/// </summary>

		/// <summary>
		/// アニメーションコンポーネント初期化
		/// </summary>

		void InitAnimationComponent();
		/// <summary>
		/// アニメーションコンポーネント取得
		/// </summary>
		AnimationComponent* GetAnimationComponent();

		/// <summary>
		/// 描画
		/// </summary>
		/// <returns></returns>
		/// <summary>
		/// レンダーコンポーネント取得
		/// </summary>
		RenderComponent* GetRenderComponent();

	private:
		Object3dCommon*		object3dCommon = nullptr;
		SkinningCommon*	skinningCommon = nullptr;
		ImGuiManager*		imGuiManager = nullptr;
		EntityManager*		entityManager = nullptr;
		LineCommon*			lineCommon = nullptr;
		SkyBoxCommon*		skyBoxCommon = nullptr;
		OceanManager *		oceanManager = nullptr;
	};

	/// <summary>
	/// スクリーン座標計算取得
	/// </summary>
	Vector2 ScreenPosition(const Engine::WorldTransform world, Engine::Camera* camera);
}

