#include "CameraZoom.h"

void CameraZoom::Update(float& posZ,float dt) {
    if(isZoom) {
        // 加算
        t += data.zoomSpeed * dt;
        if(t >= 1.0f){
            t = 1.0f;
            data.duration -= dt;
            if(data.duration <= 0.0f){
                isZoom = false;
            } 
        }
    } else {
        // 減算
        t -= data.zoomSpeed * dt;

        if(t <= 0.0f) {
            t = 0.0f;
        }
    }

    posZ = Lerp(defultPosZ,data.targetDistance,t);
}

void CameraZoom::Request(const CameraZoomData &data) {
    // ズームする    
    isZoom = true;
    // データ設定
    this->data = data;
}
