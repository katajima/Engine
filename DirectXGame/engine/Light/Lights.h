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

    Type GetType() const { return m_type; }

    // ライトデータをシェーダに送る
    virtual void SetLightData(void* data) const = 0;

    // GPU バッファから取得したデータを反映する
    virtual void UpdateFromData(const void* data) = 0;

    bool isLine = true;
protected:
    Type m_type;
};


class PointLight : public Lights {
public:
    PointLightData point;
  

    PointLight()
        : Lights(Type::Point), point() {}

    void SetLightData(void* data) const override {
        *static_cast<PointLightData*>(data) = point;
    }

    void UpdateFromData(const void* data) override {
        point = *static_cast<const PointLightData*>(data);
    }
};

class DirectionalLight : public Lights {
public:
    DirectionalLightData directional;
    DirectionalLight()
        : Lights(Type::Directional), directional() {}

    void SetLightData(void* data) const override {
        *static_cast<DirectionalLightData*>(data) = directional;
    }

    void UpdateFromData(const void* data) override {
        directional = *static_cast<const DirectionalLightData*>(data);
    }
};

class SpotLight : public Lights {
public:
    SpotLightData spot;
   
    SpotLight()
        : Lights(Type::Spot), spot() {}

    void SetLightData(void* data) const override {
        *static_cast<SpotLightData*>(data) = spot;
    }

    void UpdateFromData(const void* data) override {
        spot = *static_cast<const SpotLightData*>(data);
    }
};
