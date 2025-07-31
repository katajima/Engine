#include "VisionComponent.h"

#include "DirectXGame/engine/line/lineCommon.h"

void VisionComponent::DrawDireLine(Vector3 selfPos, Vector3 selfDir, Vector3 targetPos, float viewDistance)
{
    if (gLineDrawer) {
        Vector3 end = selfPos + Normalize(targetPos - selfPos) * viewDistance;
        gLineDrawer->AddLine(selfPos, end, { 1.0f, 1.0f, 0.0f,1.0f }); // 黄色線
    }
}
