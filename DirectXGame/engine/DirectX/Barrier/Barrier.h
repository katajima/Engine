#pragma once

#include <d3d12.h>
#include <unordered_map>

class Command;
class Barrier
{
public:
	Barrier() = default;
	~Barrier() = default;

	// 
	void Initialize(Command* command);

public:

	void TransitionResource(ID3D12Resource* res,D3D12_RESOURCE_STATES before,D3D12_RESOURCE_STATES after);

	void TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES newState);


	void RegisterInitialState(ID3D12Resource* res, D3D12_RESOURCE_STATES state);
private:

	Command* command_;
private:
	std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates_;
};

