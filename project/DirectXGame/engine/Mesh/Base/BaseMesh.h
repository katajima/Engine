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
		// 通常用
		virtual void Initialize(DirectXCommon* dxcommon) = 0;
	public:
		// 最小位置
		Vector3 GetMin() const { return min_; }

		// 最大位置
		Vector3 GetMax() const { return max_; }

		// 中心位置
		Vector3 GetMiddle() const { return Vector3{ (max_.x + min_.x) / 2, (max_.y + min_.y) / 2, (max_.z + min_.z) / 2 }; }

		// 最小位置設定
		void SetMin(const Vector3& min) { min_ = min; }

		// 最大位置設定
		void SetMax(const Vector3& max) { max_ = max; }
		// 三角面たち取得
		const std::vector<Triangle>& GetTriangles() { return triangles; }
		// 三角面取得
		const Triangle& GetTriangle(int num) { return triangles[num]; }
		// インデックス取得
		const std::vector<uint32_t>& GetIndices() { return indices; }
		// 頂点時間取得
		const std::vector<float>& GetVerticesTimer() { return verticesTimer; }
		// インデックス時間取得
		const std::vector<float>& GetIndicesTimer() { return indicesTimer; }
		// 三角面追加
		void SetTriangle(const Triangle& triangle) { triangles.push_back(triangle); }
		// インデックス追加
		void SetIndex(uint32_t index) { indices.push_back(index); }
		// インデックスクリア
		void ClearIndices() { indices.clear(); }
		// 頂点時間設定
		void SetVerticesTimer(float time) { verticesTimer.push_back(time); };
		// 時間を進める
		void AddVertexTimer(int num, float time) { verticesTimer[num] += time; }
		// 頂点時間削除
		void EraseVertexTimer() { verticesTimer.erase(verticesTimer.begin()); };
		// インデックス時間設定
		void SetIndicesTimer(float time) { indicesTimer.push_back(time); }
		// メッシュインデックス取得
		uint32_t GetMeshIndex() const { return meshIndex; }
		// メッシュインデックス設定
		void SetMeshIndex(uint32_t index) { meshIndex = index; }
		// 最大時間設定
		void SetMaxTime(float time) { maxTime = time; }
		// 最大時間取得
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
