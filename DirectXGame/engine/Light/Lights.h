#pragma once
#include "DirectXGame/engine/struct/Light.h"

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

    // ライトのタイプに基づいて異なるデータ
    Type GetType() const { return m_type; }

    // ライトデータをシェーダに送るための仮想関数
    virtual void SetLightData(void* data) const = 0;

protected:
    Type m_type;

};


class PointLight : public Lights {
public:
    PointLightData point;

   
    PointLight()
        : Lights(Type::Point), point() {}

    void SetLightData(void* data) const override {
        // PointLightDataをデータにコピーする処理
        *static_cast<PointLightData*>(data) = point;
    }
};

class DirectionalLight : public Lights {
public:
    DirectionalLightData directional;

    DirectionalLight()
        : Lights(Type::Directional), directional() {}

    void SetLightData(void* data) const override {
        // DirectionalLightDataをデータにコピーする処理
        *static_cast<DirectionalLightData*>(data) = directional;
    }
};

class SpotLight : public Lights {
public:
    SpotLightData spot;

    SpotLight()
        : Lights(Type::Spot), spot() {}

    void SetLightData(void* data) const override {
        // SpotLightDataをデータにコピーする処理
        *static_cast<SpotLightData*>(data) = spot;
    }
};

