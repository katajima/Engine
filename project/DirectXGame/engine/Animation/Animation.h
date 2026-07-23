#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/VertexData.h"

#include"map"
#include"optional"
#include <array>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include<span>


#include"imgui.h"
#include"DirectXGame/engine/Line/LineCommon.h"


namespace Engine {
	// キーフレーム
	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};

	// アニメーションカーブ
	template <typename tValue>
	struct AnimationCurve
	{
		std::vector<Keyframe<tValue>> keyframes;
	};

	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	/// <summary>
	/// ノードアニメーション
	/// </summary>
	struct NodeAnimation
	{
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};

	/// <summary>
	/// アニメーションデータ
	/// </summary>
	struct Animation
	{
		float duration; // アニメーション全体の尺 (単位は秒)
		// NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
		bool isLoop = true;	 // 基本的にはループさせる
		bool flag = false;   // アニメーションがあるかないか
	};

	/// <summary>
	/// Vector3トランスフォーム情報(オイラー回転)
	/// </summary>
	struct EulerTransform {
		Vector3 scale;
		Vector3 ratate; // Eulerでの回転
		Vector3 translate;
	};



	/// <summary>
	/// アニメーションを行うジョイント(Node)データ
	/// </summary>
	struct Joint {
		QuaternionTransform transform; // Transform情報
		Matrix4x4 localMatrix; // localMatrix
		Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
		std::string name; // 名前
		std::vector<int32_t> children; // 子JointのIndexのリスト。いなければ空
		int32_t index; // 自身のIndex
		std::optional<int32_t> parent; // 親JointのIndex。いなければnull
	};

	/// <summary>
	/// ノード
	/// </summary>
	struct  Node
	{
		QuaternionTransform transform;
		Matrix4x4 localMatrix;
		std::string name;
		std::vector<Node> children;
		std::vector<uint32_t> meshIndices; // ★追加：このノードが所有するメッシュのインデックス
	};

	/// <summary>
	/// スケルトン
	/// </summary>
	struct Skeleton {
		int32_t root; // RootJointのIndex
		std::map<std::string, int32_t> jointMap; // Joint名とIndexとの辞書
		std::vector<Joint> joints; // 所属しているジョイント
	};

	/// <summary>
	/// ウェイトデータ
	/// </summary>
	struct VertexWeightData {
		float weight;
		uint32_t vertexIndex;
	};

	/// <summary>
	/// ジョイントウェイトデータ
	/// </summary>
	struct JointWeightData {
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	/// <summary>
	/// インフルエンスデータ
	/// </summary>
	const uint32_t kNumMaxInfluence = 4; // 最大4ジョイントの影響を受ける
	struct VertexInfluence {
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	/// <summary>
	/// マトリックスパレット
	/// </summary>
	struct WellForGPU {
		Matrix4x4 skeletonSpaceMatrix; // 位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
	};

	/// <summary>
	/// スキニングの頂点数記録用
	/// </summary>
	struct SkinningInformation {
		uint32_t numVertices;
	};

	/// <summary>
	/// スキニング用SRVUAV
	/// </summary>
	struct SkinningSRVUAV {
		uint32_t wellSrvIndex; // Well
		uint32_t inputVerticesIndex; // 入力頂点
		uint32_t influencesIndex; // 入力インフルエンス
		uint32_t outputVerticesUavIndex; // 計算後の頂点データ
	};
	/// <summary>
	/// スキンクラスター
	/// </summary>
	struct SkinCluster {
		/// <summary>
		/// 生成時はGPUリソースを持っていない空の状態にしておく
		/// </summary>
		SkinCluster() = default;
		/// <summary>
		/// GPUリソースを二重解放しないようコピーは禁止する
		/// </summary>
		SkinCluster(const SkinCluster&) = delete;
		SkinCluster& operator=(const SkinCluster&) = delete;
		/// <summary>
		/// LoadBone内の一時オブジェクトから所有権を移せるようムーブは許可する
		/// </summary>
		SkinCluster(SkinCluster&&) noexcept = default;
		SkinCluster& operator=(SkinCluster&&) noexcept = default;
		/// <summary>
		/// Mapしたリソースを確実にUnmapしてからComPtrを解放する
		/// </summary>
		~SkinCluster() { Finalize(); }
		/// <summary>
		/// スキニング用GPUリソースの終了処理
		/// </summary>
		void Finalize();

		SkinningSRVUAV srvUavIndices;

		std::vector<Matrix4x4> inverseBindPoseMatrices;
		std::map<std::string, JointWeightData> skinClusterData;	// ← 全体で1つだけ
		/// <summary>
		///
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> influenceSrvHandle;

		/// <summary>
		///
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> paletteResource;
		std::span<WellForGPU> mappedPalette;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;


		/// <summary>
		///
		/// </summary>
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> inputVertexSrvHandle;


		/// <summary>
		///
		/// </summary>



		Microsoft::WRL::ComPtr < ID3D12Resource> outputVertexResource;
		D3D12_VERTEX_BUFFER_VIEW outputBufferView;
		std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> outputVertexUavHandle;

		/// <summary>
		///
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> skinningInformation;
		SkinningInformation* skinningInformationData = nullptr;

	};

	struct ModelData;
	namespace AnimationFunction {

		/// <summary>
		/// スケルトンに対してアニメーションを適用させる関数
		/// </summary>
		void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);

		/// <summary>
		/// 任意の時刻の値を取得する(Vector3)
		/// </summary>
		Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

		/// <summary>
		/// 任意の時刻の値を取得する(Quaternion)
		/// </summary>
		Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

		/// <summary>
		/// スケルトン更新
		/// </summary>
		void UpdateSkeleton(Skeleton& skeleton);

		/// <summary>
		/// スケルトンをブレンド
		/// </summary>
		void BlendSkeletons(Skeleton& outSkeleton, const Skeleton& from, const Skeleton& to, float t);

		/// <summary>
		/// スキンクラスター更新
		/// </summary>
		void UpdateSkinCluster(SkinCluster& skinCluster, const Skeleton& skeleton, std::vector<Matrix4x4>& cachedSkeletonMatrices);

		/// <summary>
		/// スケルトンの描画
		/// </summary>
		void DrawSkeleton(LineCommon* lineCommo, const std::vector<Joint>& joints, const Vector3& pos, const Vector3& scale, const Matrix4x4& rotationMatrix);

		/// <summary>
		/// アニメーション適応
		/// </summary>
		void SetAnimation(ModelData& modelData, const std::string& newAnimName, float blendDuration = 0.3f);



		/// モディファイ可能な Joint が欲しい場合
		Joint* FindJointByName(Skeleton& skeleton, const std::string& name);
		/// 名前からJointを取得する（存在しなければnullptr）
		const Joint* FindJointByNameConst(const Skeleton& skeleton, const std::string& name);
		/// <summary>
		/// ジョイントの行列取得
		/// </summary>
		Matrix4x4 GetWorldMatrixOfJoint(const Skeleton& skeleton, const std::string& jointName, const Matrix4x4& modelWorldMatrix);

		/// <summary>
		/// スケール調整
		/// </summary>
		void ValidateTransform(Joint& joint);
		/// <summary>
		/// ジョイントのImGui表示
		/// </summary>
		void ImGuiJoint(const std::vector<Joint>& joints);
		/// <summary>
		/// ノードのImGui表示
		/// </summary>
		void ImGuiNode(const std::vector<Node>& nodes);

		/// <summary>
		/// ジョイントの深さを計算する関数
		/// </summary>
		int CalculateDepth(const std::vector<Joint>& joints, int index);

	}
}





