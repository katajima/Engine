#pragma once
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Mesh/LineMesh.h"
#include "DirectXGame/engine/struct/Light.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/math/LineCurveMath.h"


namespace Engine {
	// 前方宣言
	struct OctreeColliderNode;
	class DirectXCommon;


	// ラインメッシュデータ
/// <summary>
/// LineMeshDataを管理・実装するクラス。
/// </summary>
	class LineMeshData
	{
	public:
		// 初期化
		void Initialize(DirectXCommon* dxCommon);

		// 更新
		void Update();

		// 描画
		void Draw();

		// メッシュ取得
		LineMesh* GetMesh() { return mesh_.get(); }

		// ライン使用料取得
		uint32_t GetCount() const { return lineCount_; }

		// ライン使用料加算
		void AddCount(uint32_t count) { lineCount_ += count; }

		// ラインクリア
		void Clear();

		/// <summary>
		/// ライン加算
		/// </summary>
		/// <param name="start">視点</param>
		/// <param name="end">終点</param>
		/// <param name="color">色</param>
		void AddLine(const Vector3& start, const Vector3& end, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// カメラの形状に沿ったライン加算
		/// </summary>
		/// <param name="camera"></param>
		/// <param name="color"></param>
		void AddCameraLine(const Camera& camera, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// ポイントライトのデータを追加します。
		/// </summary>
		/// <param name="data">追加するポイントライトのデータ。</param>
		void AddLightLine(const PointLightData& data);

		/// <summary>
		/// スポットライトデータを追加します。
		/// </summary>
		/// <param name="data">追加するスポットライトのデータ。</param>
		void AddLightLine(const SpotLightData& data);

		/// <summary>
		/// AABB状にライン加算
		/// </summary>
		/// <param name="aabb"></param>
		/// <param name="pos"></param>
		/// <param name="color"></param>
		void AddLineAABB(const AABB& aabb, const Vector3& pos, const Vector4& color = { 1,1,1,1 });


		/// <summary>
		/// OBB状にライン加算
		/// </summary>
		/// <param name="obb"></param>
		/// <param name="color"></param>
		void AddLineOBB(const OBB& obb, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// 球状にライン加算
		/// </summary>
		/// <param name="sphere"></param>
		/// <param name="color"></param>
		/// <param name="segmentW"></param>
		/// <param name="segmentH"></param>
		void AddLineSphere(const Sphere& sphere, const Vector4& color = { 1,1,1,1 }, int segmentW = 5, int segmentH = 5);

		/// <summary>
		/// コーナー状にライン加算
		/// </summary>
		/// <param name="corner"></param>
		/// <param name="pos"></param>
		void AddLineCorner(const CornerSegment& corner, const WorldTransform& pos, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// カプセル状にライン加算
		/// </summary>
		/// <param name="capsule"></param>
		/// <param name="color"></param>
		void AddLineCapsule(const Capsule& capsule, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// スプライン上にライン加算
		/// </summary>
		/// <param name="controlPoints"></param>
		/// <param name="pos"></param>
		/// <param name="color"></param>
		void AddSpline(const std::vector<Vector3>& controlPoints, const WorldTransform& pos, const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// スプライン状にライン加算
		/// </summary>
		/// <param name="controlPoints"></param>
		/// <param name="pos"></param>
		/// <param name="color"></param>
		void AddSpline(const std::vector<Vector3>& controlPoints, const Vector3& pos, const Vector4& color = { 1,1,1,1 });


		/// <summary>
		/// 三角形状にライン加算
		/// </summary>
		/// <param name="triangle"></param>
		/// <param name="pos"></param>
		void AddLineTriangle(const Triangle& triangle, const WorldTransform& pos,const Vector4& color = { 1,1,1,1 });

		/// <summary>
		/// オクツリー状にライン加算
		/// </summary>
		/// <param name="node"></param>
		void AddOctree(const OctreeColliderNode& node);

		// グリッド線
		void AddGrid(float xRange, float zRange, float interval, const Vector4& color);

	private:
		// ラインメッシュ
		std::unique_ptr<LineMesh> mesh_;
		// ライン使用量
		uint32_t lineCount_ = 0;
	};

	// ラインデータ
	struct LineData
	{
		Vector3 start_; // ライン開始座標
		Vector3 end_;   // ライン終了座標
		Vector4 color_; // ラインカラー
	};

	// ライン生成モジュール
	namespace CreateLine
	{
		// ライン生成
		void Line(LineMeshData& meshData, const LineData& lineData);


		// 法線ベクトルに対する垂直なベクトルを求める（円を作るため）
		static Vector3 GetPerpendicularVector(const Vector3& normal)
		{
			if (fabs(normal.x) < fabs(normal.y) && fabs(normal.x) < fabs(normal.z))
				return Normalize(Vector3(0, -normal.z, normal.y));
			else if (fabs(normal.y) < fabs(normal.z))
				return Normalize(Vector3(-normal.z, 0, normal.x));
			else
				return Normalize(Vector3(-normal.y, normal.x, 0));
		}

		// 法線ベクトルに対する「上方向」のベクトルを求める
		static Vector3 GetUpVector(const Vector3& normal)
		{
			return Normalize(Cross(normal, GetPerpendicularVector(normal)));
		}

	}
}