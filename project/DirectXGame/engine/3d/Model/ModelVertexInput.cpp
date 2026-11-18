#include "ModelData.h"

Vector2 Engine::LoadModel::ReadTexcoord(const aiMesh& mesh, uint32_t vertexIndex)
        {
            if (!mesh.HasTextureCoords(0)) return {0.5f, 0.5f};
            const auto& texcoord = mesh.mTextureCoords[0][vertexIndex];
            return {texcoord.x, texcoord.y};
        }
