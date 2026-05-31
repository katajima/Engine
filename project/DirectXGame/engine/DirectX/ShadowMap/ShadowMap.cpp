#include "ShadowMap.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include "externals/imgui/imgui.h"
#include <json.hpp>
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"
#include "DirectXGame/engine/Light/Lights.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Math/MathFunctions.h"

Engine::ShadowMap::~ShadowMap() = default;

void Engine::ShadowMap::Initialize(DirectXCommon* dxCommon)
{
	dxgiDevice_ = dxCommon->GetDXGIDevice();
	command_ = dxCommon->GetCommand();
	srvManager_ = dxCommon->GetSrvManager();
	barrier_ = dxCommon->GetBarrier();

	shadowData_ = std::make_unique<ConstantBuffer<ShadowData>>();
	shadowData_->CreateBuffer(dxCommon);
	LoadSettings();
	shadowData_->Data()->lightDirection = Normalize(Vector3{ 0.0f, -1.0f, 0.0f });
	ApplySettingsToBuffer();
	shadowData_->Data()->lightViewProjection = CreateLightViewProjection(shadowData_->Data()->lightDirection);

	CreateResource();
	CreateDescriptors();
	barrier_->RegisterInitialState(resource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Engine::ShadowMap::Update(EntityManager* entityManager)
{
	Vector3 direction = { 0.0f, -1.0f, 0.0f };
	if (entityManager && entityManager->GetLightManager()) {
		for (const auto& light : entityManager->GetLightManager()->GetLights()) {
			if (light && light->GetType() == Lights::Type::Directional) {
				const auto* directional = static_cast<const DirectionalLight*>(light.get());
				if (directional->directional.isLight) {
					direction = directional->directional.direction;
					break;
				}
			}
		}
	}

	// ライト方向がゼロに近い場合はビュー行列を作れないので、真下方向をフォールバックにする。
	if (direction.LengthSq() <= 0.0001f) {
		direction = { 0.0f, -1.0f, 0.0f };
	}

	shadowData_->Data()->lightDirection = Normalize(direction);
	ApplySettingsToBuffer();
	shadowData_->Data()->lightViewProjection = CreateLightViewProjection(shadowData_->Data()->lightDirection);
}

void Engine::ShadowMap::PreDraw()
{
	barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

	D3D12_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(kShadowMapWidth);
	viewport.Height = static_cast<float>(kShadowMapHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	command_->GetList()->RSSetViewports(1, &viewport);

	D3D12_RECT scissor{};
	scissor.right = static_cast<LONG>(kShadowMapWidth);
	scissor.bottom = static_cast<LONG>(kShadowMapHeight);
	command_->GetList()->RSSetScissorRects(1, &scissor);

	command_->GetList()->OMSetRenderTargets(0, nullptr, false, &dsvHandle_);
	command_->GetList()->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void Engine::ShadowMap::PostDraw()
{
	barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Engine::ShadowMap::SetGraphicsRootDescriptorTable(UINT rootParameterIndex)
{
	command_->GetList()->SetGraphicsRootDescriptorTable(rootParameterIndex, srvHandleGPU_);
}

void Engine::ShadowMap::SetGraphicsRootConstantBufferView(UINT rootParameterIndex)
{
	shadowData_->SetGraphicsRootConstantBufferView(rootParameterIndex);
}

const Matrix4x4& Engine::ShadowMap::GetLightViewProjection() const
{
	return shadowData_->Data()->lightViewProjection;
}

void Engine::ShadowMap::RenderDebugImGui()
{
#ifdef _DEBUG
	ImGui::Begin("ShadowMap Debug");

	bool isChanged = false;
	static float previewSize = 320.0f;
	ImGui::SliderFloat("Preview Size", &previewSize, 128.0f, 768.0f);
	ImGui::Text("Depth: black = near, white = far");
	ImGui::Text("Size: %u x %u", kShadowMapWidth, kShadowMapHeight);

	// シャドウの見た目を実行中に調整する。変更は次のシャドウパスから反映される。
	isChanged |= ImGui::DragFloat("Bias", &settings_.bias, 0.0001f, 0.0f, 0.05f, "%.5f");
	isChanged |= ImGui::SliderFloat("Shadow Min", &settings_.shadowMin, 0.0f, 1.0f);
	isChanged |= ImGui::SliderFloat("PCF Radius", &settings_.pcfRadius, 0.0f, 4.0f);
	isChanged |= ImGui::DragFloat("Ortho Half Size", &settings_.orthographicHalfSize, 0.5f, 5.0f, 300.0f);
	isChanged |= ImGui::DragFloat("Light Distance", &settings_.lightDistance, 0.5f, 5.0f, 300.0f);
	isChanged |= ImGui::DragFloat("Near Clip", &settings_.nearClip, 0.01f, 0.01f, 50.0f);
	isChanged |= ImGui::DragFloat("Far Clip", &settings_.farClip, 0.5f, 10.0f, 500.0f);

	if (isChanged) {
		ClampSettings();
		ApplySettingsToBuffer();
		shadowData_->Data()->lightViewProjection = CreateLightViewProjection(shadowData_->Data()->lightDirection);
	}

	if (ImGui::Button("Save Shadow Settings")) {
		SaveSettings();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Shadow Settings")) {
		LoadSettings();
		ApplySettingsToBuffer();
		shadowData_->Data()->lightViewProjection = CreateLightViewProjection(shadowData_->Data()->lightDirection);
	}

	ImTextureID shadowTexture = (ImTextureID)(srvHandleGPU_.ptr);
	ImGui::Image(shadowTexture, ImVec2(previewSize, previewSize));

	ImGui::End();
#endif // _DEBUG
}

void Engine::ShadowMap::CreateResource()
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = kShadowMapWidth;
	resourceDesc.Height = kShadowMapHeight;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;

	HRESULT hr = dxgiDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));
}

void Engine::ShadowMap::CreateDescriptors()
{
	dsvHeap_ = dxgiDevice_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	dsvHandle_ = dsvHeap_->GetCPUDescriptorHandleForHeapStart();

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dxgiDevice_->GetDevice()->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);

	srvIndex_ = srvManager_->Allocate();
	srvHandleCPU_ = srvManager_->GetCPUDescriptorHandle(srvIndex_);
	srvHandleGPU_ = srvManager_->GetGPUDescriptorHandle(srvIndex_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// ImGuiで表示したときにも深度値がグレースケールに見えるよう、R成分をRGBへ複製する。
	srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
		D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
		D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
		D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0,
		D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1);
	srvDesc.Texture2D.MipLevels = 1;
	dxgiDevice_->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandleCPU_);
}

Matrix4x4 Engine::ShadowMap::CreateLightViewProjection(const Vector3& direction) const
{
	Vector3 lightDirection = Normalize(direction);
	Vector3 lightPosition = lightDirection * -settings_.lightDistance;
	Vector3 zAxis = Normalize(Vector3{ 0.0f, 0.0f, 0.0f } - lightPosition);
	Vector3 up = std::abs(zAxis.y) > 0.98f ? Vector3{ 0.0f, 0.0f, 1.0f } : Vector3{ 0.0f, 1.0f, 0.0f };
	Vector3 xAxis = Normalize(Cross(up, zAxis));
	Vector3 yAxis = Cross(zAxis, xAxis);

	Matrix4x4 lightWorld = MakeIdentity4x4();
	lightWorld.m[0][0] = xAxis.x;
	lightWorld.m[0][1] = xAxis.y;
	lightWorld.m[0][2] = xAxis.z;
	lightWorld.m[1][0] = yAxis.x;
	lightWorld.m[1][1] = yAxis.y;
	lightWorld.m[1][2] = yAxis.z;
	lightWorld.m[2][0] = zAxis.x;
	lightWorld.m[2][1] = zAxis.y;
	lightWorld.m[2][2] = zAxis.z;
	lightWorld.m[3][0] = lightPosition.x;
	lightWorld.m[3][1] = lightPosition.y;
	lightWorld.m[3][2] = lightPosition.z;

	Matrix4x4 view = Inverse(lightWorld);
	// 深度シャドウは通常のD3D深度(near=0, far=1)で比較するため、Zが反転しない正射影行列を使う。
	const float halfSize = settings_.orthographicHalfSize;
	Matrix4x4 projection = MakeOrthographicMatrix(-halfSize, halfSize, halfSize, -halfSize, settings_.nearClip, settings_.farClip);
	return Multiply(view, projection);
}

void Engine::ShadowMap::ApplySettingsToBuffer()
{
	ClampSettings();
	shadowData_->Data()->bias = settings_.bias;
	shadowData_->Data()->shadowMin = settings_.shadowMin;
	shadowData_->Data()->pcfRadius = settings_.pcfRadius;
	shadowData_->Data()->padding0 = 0.0f;
	shadowData_->Data()->padding1 = 0.0f;
}

void Engine::ShadowMap::ClampSettings()
{
	settings_.bias = std::clamp(settings_.bias, 0.0f, 0.05f);
	settings_.shadowMin = std::clamp(settings_.shadowMin, 0.0f, 1.0f);
	settings_.pcfRadius = std::clamp(settings_.pcfRadius, 0.0f, 4.0f);
	settings_.orthographicHalfSize = std::clamp(settings_.orthographicHalfSize, 5.0f, 300.0f);
	settings_.lightDistance = std::clamp(settings_.lightDistance, 5.0f, 300.0f);
	settings_.nearClip = std::clamp(settings_.nearClip, 0.01f, 50.0f);
	settings_.farClip = (std::max)(settings_.farClip, settings_.nearClip + 1.0f);
}

void Engine::ShadowMap::LoadSettings()
{
	std::ifstream ifs(kSettingsFilePath);
	if (!ifs) {
		ClampSettings();
		return;
	}

	nlohmann::json root{};
	ifs >> root;
	const auto& group = root.value(kSettingsGroupName, nlohmann::json::object());
	settings_.bias = group.value("Bias", settings_.bias);
	settings_.shadowMin = group.value("ShadowMin", settings_.shadowMin);
	settings_.pcfRadius = group.value("PcfRadius", settings_.pcfRadius);
	settings_.orthographicHalfSize = group.value("OrthographicHalfSize", settings_.orthographicHalfSize);
	settings_.lightDistance = group.value("LightDistance", settings_.lightDistance);
	settings_.nearClip = group.value("NearClip", settings_.nearClip);
	settings_.farClip = group.value("FarClip", settings_.farClip);
	ClampSettings();
}

void Engine::ShadowMap::SaveSettings() const
{
	std::filesystem::create_directories(std::filesystem::path(kSettingsFilePath).parent_path());

	nlohmann::json root{};
	root[kSettingsGroupName]["Bias"] = settings_.bias;
	root[kSettingsGroupName]["ShadowMin"] = settings_.shadowMin;
	root[kSettingsGroupName]["PcfRadius"] = settings_.pcfRadius;
	root[kSettingsGroupName]["OrthographicHalfSize"] = settings_.orthographicHalfSize;
	root[kSettingsGroupName]["LightDistance"] = settings_.lightDistance;
	root[kSettingsGroupName]["NearClip"] = settings_.nearClip;
	root[kSettingsGroupName]["FarClip"] = settings_.farClip;

	std::ofstream ofs(kSettingsFilePath);
	ofs << std::setw(4) << root << std::endl;
}
