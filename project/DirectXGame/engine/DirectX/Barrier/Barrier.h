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

		// 初期化
		void Initialize(Command* command);

	public:

		// トランジション
		void TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

		// トランジション
		void TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES newState);

		// Uav依存
		void UavDependence(ID3D12Resource* res);

		// 初期状態を登録する
		void RegisterInitialState(ID3D12Resource* res, D3D12_RESOURCE_STATES state);
	private:

		Command* command = nullptr;
	private:
		std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;
	};
}
