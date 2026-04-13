#pragma once  
#include <array>  
#include <cstdint>  
#include <d3d12.h>  
#include <memory>  
#include <string>  
#include <wrl.h>  
#include "DirectXGame/engine/PSO/PSOManager.h"  
#include "CreateLine.h" 
/// <summary>  
/// ライン共通クラス  
/// </summary>  
/// 
namespace Engine {
    class LineCommon
    {
    public:

        // 初期化  
        void Initialize(DirectXCommon* dxCommon);
        // 更新  
        void Update();
        // 描画  
        void Draw();
        // 描画前準備  
        void DrawCommonSetting();
        // 描画前準備2  
        void DrawCommonSetting2();
        // DitectXCommonの取得  
        DirectXCommon* GetDxCommon() const { return dxCommon; }
        // カメラ設定  
        void SetDefaltCamera(Camera* camera) { this->camera = camera; }

        // デバッグラインメッシュデータの取得  
        LineMeshData& GetDebugLineMeshData() { return lineDebugMeshData_; }

        // ラインメッシュデータの取得  
        LineMeshData& GetLineMeshData() { return lineMeshData_; }
    public:
        void LineClear();

        // GPUデータ  
        struct LineGPU {
            Vector4 color;
            Vector3 strPos;
            float pad;
            Vector3 endPos;
            float pad2;
        };
    private:
        // ルートシグネチャの作成  
        void CreateRootSignature();
        // グラフィックスパイプラインの作成  
        void CreateGraphicsPipeline();
    private:
        std::unique_ptr<PSOManager> psoManager_ = nullptr;
        
        // ルートシグネチャ  
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
        // パイプラインステートオブジェクト  
        Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
        // ルートシグネチャ  
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature2;
        // パイプラインステートオブジェクト  
        Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState2;

        const uint32_t kNumMaxInstance = 300000;

        //マテリアルデータ  
        struct MaterialData {
            Vector4 color;
        };
        Microsoft::WRL::ComPtr < ID3D12Resource> materialResource;
        MaterialData* materialData;
        Microsoft::WRL::ComPtr<ID3D12Resource> viewResource;
        Matrix4x4* cameraWVP;


        // デバッグ用ラインメッシュデータ  
        LineMeshData lineDebugMeshData_;

        // ラインメッシュデータ  
        LineMeshData lineMeshData_;
    private:
        DirectXCommon* dxCommon = nullptr;
        Camera* camera = nullptr;
    };
}
