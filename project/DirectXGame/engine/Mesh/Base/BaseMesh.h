#pragma once

#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Material.h"
#include "DirectXGame/engine/struct/VertexData.h"


#include "vector"
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<d3d12.h>
#include<dxgi1_6.h>
#include "set"
#include <unordered_map>
#include <vector>

namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// メッシュの基底クラス
	/// </summary>
	class BaseMesh {
	public:
		/// <summary>
		/// 通常用
		/// </summary>
		virtual void Initialize(DirectXCommon* dxcommon) = 0;
	public:
		/// <summary>
		/// 最小位置
		/// </summary>
		Vector3 GetMin() const { return min_; }

		/// <summary>
		/// 最大位置
		/// </summary>
		Vector3 GetMax() const { return max_; }

		/// <summary>
		/// 中心位置
		/// </summary>
		Vector3 GetMiddle() const { return Vector3{ (max_.x + min_.x) / 2, (max_.y + min_.y) / 2, (max_.z + min_.z) / 2 }; }

		/// <summary>
		/// 最小位置設定
		/// </summary>
		void SetMin(const Vector3& min) { min_ = min; }

		/// <summary>
		/// 最大位置設定
		/// </summary>
		void SetMax(const Vector3& max) { max_ = max; }
		/// <summary>
		/// 三角面たち取得
		/// </summary>
		const std::vector<Triangle>& GetTriangles() { return triangles; }
		/// <summary>
		/// 三角面取得
		/// </summary>
		const Triangle& GetTriangle(int num) { return triangles[num]; }
		/// <summary>
		/// インデックス取得
		/// </summary>
		const std::vector<uint32_t>& GetIndices() { return indices; }
		/// <summary>
		/// 頂点時間取得
		/// </summary>
		const std::vector<float>& GetVerticesTimer() { return verticesTimer; }
		/// <summary>
		/// インデックス時間取得
		/// </summary>
		const std::vector<float>& GetIndicesTimer() { return indicesTimer; }
		/// <summary>
		/// 三角面追加
		/// </summary>
		void SetTriangle(const Triangle& triangle) { triangles.push_back(triangle); }
		/// <summary>
		/// インデックス追加
		/// </summary>
		void SetIndex(uint32_t index) { indices.push_back(index); }
		/// <summary>
		/// インデックスクリア
		/// </summary>
		void ClearIndices() { indices.clear(); }
		/// <summary>
		/// 頂点時間設定
		/// </summary>
		void SetVerticesTimer(float time) { verticesTimer.push_back(time); };
		/// <summary>
		/// 時間を進める
		/// </summary>
		void AddVertexTimer(int num, float time) { verticesTimer[num] += time; }
		/// <summary>
		/// 頂点時間削除
		/// </summary>
		void EraseVertexTimer() { verticesTimer.erase(verticesTimer.begin()); };
		/// <summary>
		/// インデックス時間設定
		/// </summary>
		void SetIndicesTimer(float time) { indicesTimer.push_back(time); }
		/// <summary>
		/// メッシュインデックス取得
		/// </summary>
		uint32_t GetMeshIndex() const { return meshIndex; }
		/// <summary>
		/// メッシュインデックス設定
		/// </summary>
		void SetMeshIndex(uint32_t index) { meshIndex = index; }
		/// <summary>
		/// 最大時間設定
		/// </summary>
		void SetMaxTime(float time) { maxTime = time; }
		/// <summary>
		/// 最大時間取得
		/// </summary>
		float GetMaxTime() const { return maxTime; }
	protected:
		DirectXCommon* dxCommon = nullptr;


		Vector3 min_;
		Vector3 max_;
	protected:
		std::vector<Triangle> triangles;
		std::vector<uint32_t> indices;
		std::vector<float> verticesTimer;
		std::vector<float> indicesTimer;
		float maxTime = 0.2f;
		uint32_t meshIndex = 0;
	};
}
