#include "ShapePlane.h"
#include "DirectXGame/engine/Effect/Primitive/PrimitiveCommon.h"


void ShapePlane::Initialize(PrimitiveCommon* primitiveCommon)
{
    primitiveCommon_ = primitiveCommon;
    // プレーン
   

    mesh_ = std::make_unique<Mesh>();
    
    mesh_->Initialize(primitiveCommon_->GetDxCommon());
    


}

void ShapePlane::Update()
{
    UpdateImGui();
}

void ShapePlane::Draw()
{

    mesh_->GetCommandList();


    primitiveCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);

}

void ShapePlane::UpdateImGui()
{
#ifdef _DEBUG
   /* ImGui::Begin("Plane");
    ImGui::DragFloat3("center", &paramerter_.center.x,0.1f);
    ImGui::DragFloat("width", &paramerter_.width,0.1f);
    ImGui::DragFloat("height", &paramerter_.height,0.1f);
    ImGui::DragFloat2("uvScale", &paramerter_.uvScale.x,0.1f);
    ImGui::DragInt("resolutionX", &paramerter_.resolutionX);
    ImGui::DragInt("resolutionZ", &paramerter_.resolutionZ);
    ImGui::End();*/
#endif // _DEBUG
    //if (o_paramerter_ != paramerter_) {
    //    CreatePlane(paramerter_);
    //    mesh_->UpdateIndexBuffer();
    //    mesh_->UpdateVertexBuffer();
    //}

    //o_paramerter_ = paramerter_;
}



void ShapePlane::UpdateMesh()
{

}







