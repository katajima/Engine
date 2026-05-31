#pragma once
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/3d/Model/Model.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


namespace Engine {
	// オブジェクトのタイプを指定する
	enum class ObjectModelType { // オブジェクト種類
		kNormal,	// モデルを描画するオブジェクト
		kAnimation,	// モデルをアニメーション描画するオブジェクト
		kSkinning,	// モデルをスキニング描画するオブジェクト
		kPrimitive, // プリミティブを描画するオブジェクト
		kSkyBox,	// スカイボックスを描画するオブジェクト
		kOcean,		// 波を描画するオブジェクト
	};

	// 描画する順番
	enum class ObjectDrawType {
		kOpaque,		// 不透明
		kTranslucent01,	// 半透明最前
		kTranslucent02,	// 半透明中
		kTranslucent03,	// 半透明最後
	};

	// 前方宣言
	class BasePrimitive;
	class SkyBox;
	class Ocean;
	class Transfomation;
	class EntityManager;
	class ShadowMap;

	/// <summary>
	/// 描画に関するコンポーネントクラス
	/// </summary>
	class RenderComponent
	{
	public:
		~RenderComponent();
		RenderComponent();

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="entityManager">オブジェクトに関係するマネージャー</param>
		/// <param name="objectType">オブジェクトのモデルタイプ(スキニング、アニメーション、プリミティブ、スカイボックスなど)</param>
		/// <param name="rasterizerType">モデルのうつり方を設定</param>
		void Init(EntityManager* entityManager, ObjectModelType objectType, PSOType rasterizerType);

		// 更新
		void Update();

		// 描画
		void Draw();
		// シャドウマップ用の深度描画
		void DrawShadowMap(ShadowMap* shadowMap);

		// カメラ設定
		void SetCamera(Camera* camera) { this->camera = camera; }
		// トランスフォーム設定
		void SetTransfomation(Transfomation* transfomation) { this->transfomation = transfomation; }

		// モデル設定
		void SetModel(Model* model);
		// スカイボックス設定
		void SetSkyBox(SkyBox* skyBox);
		// 波セット設定
		void SetOcean(Ocean* ocean);
		// プリミティブ設定
		void SetPrimitive(std::unique_ptr<BasePrimitive> primitive);

		

	public:
		// モデル取得
		Model* GetModel() const;
		// 波取得
		Ocean* GetOcean();
		// スカイボックス取得
		SkyBox* GetSkyBox();
		// プリミティブ形状取得
		BasePrimitive* GetPrimitive();
	public:
		// 何かしらの見た目があるか
		bool GetIsSkin() const { return isSkin_; }
		// 描画するかの設定
		void SetIsDraw(bool is) { isDraw = is; }
		// オブジェクトタイプ取得
		ObjectModelType GetObjectType() const { return objectType_; }
		// オブジェクトの描画順取得
		ObjectDrawType GetObjectDrawType() const { return objectDrawType_; }
		// 描画順の設定
		void SetObjectDrawType(ObjectDrawType type) { objectDrawType_ = type; };
		// 映り方タイプ設定
		void SetObjectRasterizerType(PSOType type) { rasterizerType_ = type; }
		// オブジェクト型名前取得
		std::string GetObjectTypeName() const { return objectTypeName; }
		// マテリアルインスタンス
		std::vector<MaterialInstance>& GetMaterialInstance() { return materialInstances_; }

	public:
		// 透明度取得
		float GetAlpha();


	private:
		// 通常オブジェクトのうつり方設定
		void ObjectNormalTypeDiscrimination(PSOType type);
		// スキニングオブジェクトのうつり方設定
		void ObjectSkinningTypeDiscrimination(PSOType type);

		// 描画セッティング(デフォルト)
		void DrawSetting();
		// 描画セッティング(スキニング)
		void DrawSettingSkin();
		// 描画セッティング(海)
		void DrawSettingOcean();


	private:
		// オブジェクトの描画順
		ObjectDrawType objectDrawType_ = ObjectDrawType::kOpaque;
		// オブジェクトの映り方タイプ
		PSOType rasterizerType_ = PSOType::NoUvInterpolation_MODE_SOLID_BACK;
		// オブジェクトのタイプ
		ObjectModelType objectType_ = ObjectModelType::kNormal;
		// 何かしらの見た目があるか
		bool isSkin_ = false;
		// 描画するかのフラグ
		bool isDraw = true;
		// オブジェクトタイプ名前
		std::string objectTypeName = "";

		// マテリアルインスタンス
		std::vector<MaterialInstance> materialInstances_{};
		// マテリアル用定数バッファ
		std::vector<std::unique_ptr<ConstantBuffer<Material::DataGPU>>> cbResources_;
	private: // 貰いもの
		// 3Dエンティティマネージャー
		EntityManager* entityManager = nullptr;
		// モデル
		Model* model = nullptr;
		// プリミティブ
		std::unique_ptr<BasePrimitive> primitive = nullptr;
		// スカイボックス
		SkyBox* skyBox = nullptr;
		// 波
		Ocean* ocean = nullptr;
		// トランスフォーム
		Transfomation* transfomation = nullptr;
		// カメラ
		Camera* camera = nullptr;
	};
}
