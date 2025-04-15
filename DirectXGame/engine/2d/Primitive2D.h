#pragma once
//#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Structs2D.h"
#include"DirectXGame/engine/struct/Material.h"

#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include"DirectXGame/engine/Material/Material.h"

#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
using namespace Microsoft::WRL;



namespace ShapeParameter2D {

	struct Cube
	{
		Vector2 size = { 1,1 };
	};
	struct  Sphere
	{
		float radius;
		int segment;
	};
	struct Star
	{
		float innerRadius_ = 2.0f;
		float outerRadius_ = 5.0f;
		int segments_ = 5;


		// == オペレーター
		bool operator==(const Star& other) const {
			return innerRadius_ == other.innerRadius_ && outerRadius_ == other.outerRadius_ && segments_ == other.segments_;
		}

		// != オペレーター
		bool operator!=(const Star& other) const {
			return !(*this == other);
		}
	};
	struct Ring {
		float innerRadius_;
		float outerRadius_;
		int segments_;

		// == オペレーター
		bool operator==(const Ring& other) const {
			return innerRadius_ == other.innerRadius_ && outerRadius_ == other.outerRadius_ && segments_ == other.segments_;
		}

		// != オペレーター
		bool operator!=(const Ring& other) const {
			return !(*this == other);
		}
	};

	struct Triangle {
		Vector2 vertices[3]; // !頂点

	};

};


class SpriteCommon;

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


	// 形
	void Initialize(SpriteCommon* spriteCommon,ShapeType type, const Color color = { 1,1,1,1 });

	void Update();

	void Draw();


	// 色
	const Color& GetColor() const { return material->color; }
	void SetColor(const Color& color) { material->color = color; }

	// アンカーポイント
	const Vector2& GetAnchorPoint() const { return anchorPoint; };
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }


	
	void SetParametar(float innerRadius, float outerRadius, int segments);
	void SetParametar(float radius, int segments);
	void SetParametar(Vector2 size);
	void SetParametar(Vector2 p0, Vector2 p1, Vector2 p2);
private:
	// 三角面
	void CreateTriangle(Vector2 p0,Vector2 p1,Vector2 p2);
	// 円
	void CreateCircle(float radius, int segments);
	// リング
	void CreateRing(float innerRadius, float outerRadius, int segments);
	// 星
	void CreateStar(float innerRadius, float outerRadius, int segments);
	// 四角
	void CreateCube(Vector2 size);



private:
	ShapeParameter2D::Cube cabe_;
	ShapeParameter2D::Sphere sphere_;
	ShapeParameter2D::Triangle triangle_;
	ShapeParameter2D::Star star_;
	ShapeParameter2D::Ring ring_;

public:
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 scale = { 1.0f,1.0f };
private:
	SpriteCommon* spriteCommon_ = nullptr;

	ShapeType type_;

	//頂点データ
	struct VertexData {

		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };


	

	
	// メッシュ
	std::unique_ptr<Mesh> mesh;
	// トランスフォーム
	std::unique_ptr<Transfomation>transfomation = nullptr;
	// マテリアル
	std::unique_ptr<Material> material = nullptr;


	// アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };

};

