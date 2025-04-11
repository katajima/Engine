#include "Mesh.h"

#include "DirectXGame/engine/base/ImGuiManager.h"
#include "DirectXGame/engine/3d/Model/ModelCommon.h"

// 頂点を比較するためのオペレーター
bool operator==(const Mesh::VertexData& v1, const Mesh::VertexData& v2) {
	return v1.position == v2.position &&
		v1.normal == v2.normal &&
		v1.texcoord == v2.texcoord;
}

void Mesh::Initialize(ModelCommon* modelCommon)
{
	modelCommon_ = modelCommon;

	//if (vertices.size() != 0) {
	vertexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(VertexData) * vertices.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());
	//}
	//if (indices.size() != 0) {
		// インデクスリソース
	indexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
	
}

void Mesh::InitializeLine(ModelCommon* modelCommon)
{
	modelCommon_ = modelCommon;

	vertexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(LineVertexData) * verticesline.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(LineVertexData) * verticesline.size());
	vertexBufferView.StrideInBytes = sizeof(LineVertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&lineVertexData));
	std::memcpy(lineVertexData, verticesline.data(), vertexBufferView.SizeInBytes);


	// インデクスリソース
	indexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), indexBufferView.SizeInBytes);

}

void Mesh::InitializeSkyBox(ModelCommon* modelCommon)
{
	modelCommon_ = modelCommon;

	vertexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(SkyBoxVertexData) * verticesskyBox.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(SkyBoxVertexData) * verticesskyBox.size());
	vertexBufferView.StrideInBytes = sizeof(SkyBoxVertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&skyBoxVertexData));
	std::memcpy(skyBoxVertexData, verticesskyBox.data(), vertexBufferView.SizeInBytes);


	// インデクスリソース
	indexResource = modelCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), indexBufferView.SizeInBytes);
}

void Mesh::UpdateVertexBuffer() {
	if (vertexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(VertexData) * vertices.size();
		D3D12_RESOURCE_DESC desc = vertexResource->GetDesc();
		if (requiredSize > desc.Width) {
			// バッファが不足している場合、再割り当て
			vertexResource.Reset();

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
			HRESULT hr = modelCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));

			if (FAILED(hr)) {
				// エラー処理
				return;
			}

			// バッファビューの更新
			vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
			vertexBufferView.SizeInBytes = UINT(requiredSize);
			vertexBufferView.StrideInBytes = sizeof(VertexData);
		}

		// データのコピー
		VertexData* data;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, vertices.data(), requiredSize);
		vertexResource->Unmap(0, nullptr);
	}
}

void Mesh::UpdateLineVertexBuffer()
{
	if (vertexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(LineVertexData) * verticesline.size();
		D3D12_RESOURCE_DESC desc = vertexResource->GetDesc();
		if (requiredSize > desc.Width) {
			// バッファが不足している場合、再割り当て
			vertexResource.Reset();

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
			HRESULT hr = modelCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));

			if (FAILED(hr)) {
				// エラー処理
				return;
			}

			// バッファビューの更新
			vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
			vertexBufferView.SizeInBytes = UINT(requiredSize);
			vertexBufferView.StrideInBytes = sizeof(LineVertexData);
		}

		// データのコピー
		LineVertexData* data;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, verticesline.data(), requiredSize);
		vertexResource->Unmap(0, nullptr);
	}
}

void Mesh::UpdateIndexBuffer()
{
	if (indexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(uint32_t) * indices.size();
		D3D12_RESOURCE_DESC desc = indexResource->GetDesc();
		if (requiredSize > desc.Width) {
			// バッファが不足している場合、再割り当て
			indexResource.Reset();

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
			HRESULT hr = modelCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexResource));

			if (FAILED(hr)) {
				// エラー処理
				return;
			}

			// バッファビューの更新
			indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
			indexBufferView.SizeInBytes = UINT(requiredSize);

		}
		// データのコピー
		uint32_t* data;
		indexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, indices.data(), requiredSize);
		indexResource->Unmap(0, nullptr);
	}

}

void Mesh::GetCommandList()
{
	// 頂点バッファの設定
	modelCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// インデックスバッファの設定
	modelCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}

void Mesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		vertexBufferView,
		vbv
	};

	// 頂点バッファの設定
	modelCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
	// インデックスバッファの設定
	modelCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}

void Mesh::Clear()
{
	indices.clear();
	vertices.clear();
	verticesline.clear();
}

void Mesh::GenerateIndices2()
{

	indices.clear();


	// ハッシュマップで頂点の重複を管理
	std::unordered_map<Mesh::VertexData, uint32_t, VertexHash> vertexMap;

	for (size_t i = 0; i < vertices.size(); ++i) {
		const Mesh::VertexData& vertex = vertices[i];

		// 既に同じ頂点が登録されているかチェック
		auto it = vertexMap.find(vertex);
		if (it != vertexMap.end()) {
			// 既存のインデックスを使用
			indices.push_back(it->second);
		}
		else {
			// 新しいインデックスを追加
			vertexMap[vertex] = static_cast<uint32_t>(i);
			indices.push_back(static_cast<uint32_t>(i));
		}
	}
}

bool Mesh::IsSegmentCollision(const Mesh& mesh, const Vector3& worldPos, const Segment& segment)
{
	for (size_t i = 0; i < mesh.triangle.size(); i++) {
		const Triangle& tria = mesh.triangle[i];
		Triangle offsetTri = tria.OffsetVector3(worldPos);

		if (IsCollision(offsetTri, segment)) {
			return true;
		}
	}
	return false;
}

bool Mesh::IsCapsuleCollision(const Mesh& mesh, const Vector3& worldPos, const Capsule& capsule)
{
	for (size_t i = 0; i < mesh.triangle.size(); i++) {
		const Triangle& tria = mesh.triangle[i];
		Triangle offsetTri = tria.OffsetVector3(worldPos);

		if (IsCollision(offsetTri, capsule)) {
			return true;
		}
	}
	return false;
}

void Mesh::SetTriangleImGui(const Mesh& mesh, const std::string name, const Vector3& worldPos)
{

	ImGui::Begin(name.c_str());
	std::string str;
	for (size_t i = 0; i < mesh.triangle.size(); i++) {
		str = "triangle" + std::to_string(i);
		std::string str2;
		for (size_t j = 0; j < 3;j++) {
			str2 = str + "_" + std::to_string(j);
			Vector3 tri = mesh.triangle[i].vertices[j] + worldPos;
			ImGui::InputFloat3(str2.c_str(), &tri.x);
		}
	}
	
	ImGui::End();

}

#include <unordered_map>
#include <vector>

void Mesh::MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum)
{
	//lineIndices.clear();
	//lineIndices.reserve(indices.size() * 2); // 事前にメモリ確保（最大2倍のライン数を想定）

	// エッジの重複管理用マップ
	std::unordered_map<uint64_t, bool> edgeMap;

	for (size_t i = 0; i < indices.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			uint32_t v0 = indices[i + j] + lineNum;
			uint32_t v1 = indices[i + (j + 1) % 3] + lineNum;

			// 一意なキーを作成（小さい方を前にする）
			if (v0 > v1) std::swap(v0, v1);
			uint64_t edgeKey = (static_cast<uint64_t>(v0) << 32) | v1;

			// まだ登録されていないエッジなら追加
			if (edgeMap.find(edgeKey) == edgeMap.end()) {
				edgeMap[edgeKey] = true;
				lineIndices.push_back(v0);
				lineIndices.push_back(v1);
			}
		}
	}
}




