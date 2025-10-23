#pragma once
#include "DirectXGame/engine/struct/Light.h"

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
        : m_type(type) {}

    virtual ~Lights() {}

    // ライト種類取得
    Type GetType() const { return m_type; }
    // ライト番号取得
    int GetNumber() const { return num; }
    // ライト番号設定
    void SetNumber(int num) { this->num = num; }

    // ライトデータをシェーダに送る
    virtual void SetLightData(void* data) const = 0;

    // GPU バッファから取得したデータを反映する
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
        : Lights(Type::Point), point() {}

    // データ設定
    void SetLightData(void* data) const override {
        *static_cast<PointLightData*>(data) = point;
    }

    // データ更新
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
        : Lights(Type::Directional), directional() {}
    // データ設定
    void SetLightData(void* data) const override {
        *static_cast<DirectionalLightData*>(data) = directional;
    }
    // データ更新
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
        : Lights(Type::Spot), spot() {}
    // データ設定
    void SetLightData(void* data) const override {
        *static_cast<SpotLightData*>(data) = spot;
    }
    // データ更新
    void UpdateFromData(const void* data) override {
        spot = *static_cast<const SpotLightData*>(data);
    }
};
