#pragma once
//#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Structs2D.h"
#include"DirectXGame/engine/struct/Material.h"

#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/Transform/Transformation/Transformation.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform2d.h"

#include"DirectXGame/engine/Material/Material.h"

#include"DirectXGame/engine/Mesh/ModelMesh.h"

#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
using namespace Microsoft::WRL;

// シェイプパラメータ
namespace ShapeParameter2D {

	// 四角
	struct Cube
	{
		Vector2 size = { 1,1 };
	};
	// 球
	struct  Sphere
	{
		float radius;
		int segment;
	};
	// 星
	struct Star
	{
		float innerRadius_ = 2.0f;
		float outerRadius_ = 5.0f;
		int segments_ = 5;


		/// <summary>
		/// == オペレーター
		/// </summary>
		bool operator==(const Star& other) const {
			return innerRadius_ == other.innerRadius_ && outerRadius_ == other.outerRadius_ && segments_ == other.segments_;
		}

		/// <summary>
		/// != オペレーター
		/// </summary>
		bool operator!=(const Star& other) const {
			return !(*this == other);
		}
	};
	// リング
	struct Ring {
		float innerRadius_;
		float outerRadius_;
		int segments_;

		/// <summary>
		/// == オペレーター
		/// </summary>
		bool operator==(const Ring& other) const {
			return innerRadius_ == other.innerRadius_ && outerRadius_ == other.outerRadius_ && segments_ == other.segments_;
		}

		/// <summary>
		/// != オペレーター
		/// </summary>
		bool operator!=(const Ring& other) const {
			return !(*this == other);
		}
	};
	// 三角
	struct Triangle {
		Vector2 vertices[3]; // !頂点

	};

};

// 前方宣言
namespace Engine {
	class SpriteCommon;
}
// エンジンネームスペース
namespace Engine {

	/// <summary>
	/// 2dでのプリミティブクラス
	/// </summary>
	class Primitive2D
	{
	public:
		enum class ShapeType
		{
			Cube,			// 四角
			Triangle,		// 三角形
			Circle,			// 円
			Star,			// 星
			Ring,			// リング
		};

		/// <summary>
		/// スプライトの共通設定を初期化します。
		/// </summary>
		/// <param name="spriteCommon">初期化するスプライトの共通設定を指すポインタ。</param>
		/// <param name="type">スプライトの形状タイプ。</param>
		/// <param name="color">スプライトの初期色（デフォルト値は {1, 1, 1, 1} ）。</param>
		void Initialize(SpriteCommon* spriteCommon, ShapeType type, const Color color = { 1,1,1,1 });
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();


		/// <summary>
		/// 色取得
		/// </summary>
		const Color& GetColor() const { return material->GetMaterialInstance().color; }
		/// <summary>
		/// 色設定
		/// </summary>
		void SetColor(const Color& color) { material->GetMaterialInstance().color = color; }

		/// <summary>
		/// アンカーポイント取得
		/// </summary>
		const Vector2& GetAnchorPoint() const { return anchorPoint; };
		/// <summary>
		/// アンカーポイント設定
		/// </summary>
		void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }


		/// <summary>
		/// パラメーター設定(内半径、外半径、セグメント数)(星やリング)
		/// </summary>
		void SetParametar(float innerRadius, float outerRadius, int segments);
		/// <summary>
		/// パラメーター設定(半径、セグメント数)(円)
		/// </summary>
		void SetParametar(float radius, int segments);
		/// <summary>
		/// パラメーター設定(サイズ)(四角)
		/// </summary>
		void SetParametar(Vector2 size);
		/// <summary>
		/// パラメーター設定(各点)(三角)
		/// </summary>
		void SetParametar(Vector2 p0, Vector2 p1, Vector2 p2);
	private:
		/// <summary>
		/// 三角面生成
		/// </summary>
		void CreateTriangle(Vector2 p0, Vector2 p1, Vector2 p2);
		/// <summary>
		/// 円生成
		/// </summary>
		void CreateCircle(float radius, int segments);
		/// <summary>
		/// リング生成
		/// </summary>
		void CreateRing(float innerRadius, float outerRadius, int segments);
		/// <summary>
		/// 星生成
		/// </summary>
		void CreateStar(float innerRadius, float outerRadius, int segments);
		/// <summary>
		/// 四角生成
		/// </summary>
		void CreateCube(Vector2 size);



	private:
		ShapeParameter2D::Cube cabe_;
		ShapeParameter2D::Sphere sphere_;
		ShapeParameter2D::Triangle triangle_;
		ShapeParameter2D::Star star_;
		ShapeParameter2D::Ring ring_;

	public:
		WorldTransform2d worldTransform;
	private:
		Engine::SpriteCommon* spriteCommon = nullptr;

		ShapeType type_;

		//頂点データ
		struct VertexData {

			Vector4 position;
			Vector2 texcoord;
			Vector3 normal;
		};
		Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };





		// メッシュ
		std::unique_ptr<ModelMesh> mesh;
		// トランスフォーム
		std::unique_ptr<Transformation>transformation = nullptr;
		// マテリアル
		std::unique_ptr<Material> material = nullptr;


		// アンカーポイント
		Vector2 anchorPoint = { 0.0f,0.0f };

	};

}