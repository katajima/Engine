#include "GPUParticleEmitter.h"
#include "DirectXGame/engine/Utility/EmissionTimer.h"
#include <cmath>

void Engine::BaseGpuParticleEmitter::Update(float deltaTime) {
    deltaTime_ = std::isfinite(deltaTime) && deltaTime > 0.0f ? deltaTime : 0.0f;
    // GPUへ周期の消費回数を渡し、一回あたりの設定個数は変更しない。
    cbEmitterCommon_.emit = ConsumeEmissionBursts(cbEmitterCommon_.frequencyTime, deltaTime_, cbEmitterCommon_.frequency);
    cbEmitterCommon_.translate = worldTransform_.translate_;
    if (!isEmitte_) {
        cbEmitterCommon_.emit = 0;
    }
    if (group) {
        UpdateUnique(deltaTime_);
    }
    worldTransform_.Update();
}
