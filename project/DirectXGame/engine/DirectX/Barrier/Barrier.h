#pragma once

#include <d3d12.h>
#include <unordered_map>


namespace Engine {
	// 前方宣言
	class Command;

	/// <summary>
	/// バリアクラス
	/// </summary>
	class Barrier
	{
	public:
		Barrier() = default;
		~Barrier() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command);

	public:

		/// <summary>
		/// トランジション
		/// </summary>
		void TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		/// <summary>
		/// トランジション
		/// </summary>
		void TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES newState);

		/// <summary>
		/// Uav依存
		/// </summary>
		void UavDependence(ID3D12Resource* res);

		/// <summary>
		/// 初期状態を登録する
		/// </summary>
		void RegisterInitialState(ID3D12Resource* res, D3D12_RESOURCE_STATES state);
	private:

		Command* command = nullptr;
	private:
		std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;
	};
}
