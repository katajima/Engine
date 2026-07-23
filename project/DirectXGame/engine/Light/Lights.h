#pragma once
#include "DirectXGame/engine/struct/Light.h"


namespace Engine {
    /// <summary>
    /// ライトクラス
    /// </summary>
    class Lights
    {
    public:
        enum class Type {
            Point,
            Directional,
            Spot
        };

        Lights(Type type)
            : m_type(type) {
        }

        virtual ~Lights() {}

        /// <summary>
        /// ライト種類取得
        /// </summary>
        Type GetType() const { return m_type; }
        /// <summary>
        /// ライト番号取得
        /// </summary>
        int GetNumber() const { return num; }
        /// <summary>
        /// ライト番号設定
        /// </summary>
        void SetNumber(int num) { this->num = num; }

        /// <summary>
        /// ライトデータをシェーダに送る
        /// </summary>
        virtual void SetLightData(void* data) const = 0;

        /// <summary>
        /// GPU バッファから取得したデータを反映する
        /// </summary>
        virtual void UpdateFromData(const void* data) = 0;

        bool isLine = true;
    protected:
        Type m_type;
        int num = 0; //番号
    };

    /// <summary>
    /// ポイントライト
    /// </summary>
    class PointLight : public Lights {
    public:
        PointLightData point;


        PointLight()
            : Lights(Type::Point), point() {
        }

        /// <summary>
        /// データ設定
        /// </summary>
        void SetLightData(void* data) const override {
            *static_cast<PointLightData*>(data) = point;
        }

        /// <summary>
        /// データ更新
        /// </summary>
        void UpdateFromData(const void* data) override {
            point = *static_cast<const PointLightData*>(data);
        }
    };

    /// <summary>
    /// ディレクショナルライト
    /// </summary>
    class DirectionalLight : public Lights {
    public:
        DirectionalLightData directional;
        DirectionalLight()
            : Lights(Type::Directional), directional() {
        }
        /// <summary>
        /// データ設定
        /// </summary>
        void SetLightData(void* data) const override {
            *static_cast<DirectionalLightData*>(data) = directional;
        }
        /// <summary>
        /// データ更新
        /// </summary>
        void UpdateFromData(const void* data) override {
            directional = *static_cast<const DirectionalLightData*>(data);
        }
    };

    /// <summary>
    /// スポットライト
    /// </summary>
    class SpotLight : public Lights {
    public:
        SpotLightData spot;

        SpotLight()
            : Lights(Type::Spot), spot() {
        }
        /// <summary>
        /// データ設定
        /// </summary>
        void SetLightData(void* data) const override {
            *static_cast<SpotLightData*>(data) = spot;
        }
        /// <summary>
        /// データ更新
        /// </summary>
        void UpdateFromData(const void* data) override {
            spot = *static_cast<const SpotLightData*>(data);
        }
    };
}