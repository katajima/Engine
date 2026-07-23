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
/// <summary>
/// LineCommonを管理・実装するクラス。
/// </summary>
    class LineCommon
    {
    public:
        ~LineCommon();

        /// <summary>
        /// 初期化
        /// </summary>
        void Initialize(DirectXCommon* dxCommon);
        /// <summary>
        /// 終了処理
        /// </summary>
        void Finalize();
        /// <summary>
        /// 更新
        /// </summary>
        void Update();
        /// <summary>
        /// 描画
        /// </summary>
        void Draw();
        /// <summary>
        /// 描画前準備
        /// </summary>
        void DrawCommonSetting();
        /// <summary>
        /// 描画前準備2
        /// </summary>
        void DrawCommonSetting2();
        /// <summary>
        /// DitectXCommonの取得
        /// </summary>
        DirectXCommon* GetDxCommon() const { return dxCommon; }
        /// <summary>
        /// カメラ設定
        /// </summary>
        void SetDefaultCamera(Camera* camera) { this->camera = camera; }

        /// <summary>
        /// デバッグラインメッシュデータの取得
        /// </summary>
        LineMeshData& GetDebugLineMeshData() { return lineDebugMeshData_; }

        /// <summary>
        /// ラインメッシュデータの取得
        /// </summary>
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
        /// <summary>
        /// ルートシグネチャの作成
        /// </summary>
        void CreateRootSignature();
        /// <summary>
        /// グラフィックスパイプラインの作成
        /// </summary>
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
