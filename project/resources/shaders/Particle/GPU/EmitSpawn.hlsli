#include "GpuParticle.hlsli"
#include "random.hlsli"

// [0,1] の乱数を generator から得る補助関数
float Random01(inout RandomGeneratetor gen)
{
    return gen.Generate1d_4() * 0.5f + 0.5f;
}

// 球体の内部に一様分布で乱数を出す（ループなし）
float3 RandomInSphere(inout RandomGeneratetor gen, float radius)
{
    // 方向を球面一様分布で作成
    float theta = Random01(gen) * 6.2831853f; // 0～2π
    float phi = acos(2.0f * Random01(gen) - 1); // 0～π

    float x = sin(phi) * cos(theta);
    float y = cos(phi);
    float z = sin(phi) * sin(theta);

    // 半径補正 pow(rand, 1/3)
    float r = radius * pow(Random01(gen), 1.0f / 3.0f);

    return float3(x, y, z) * r;
}


// 放出位置を決定する
class EmitSpawns
{
    // 球体の内部に一様分布で乱数を出す
    float3 EmitSphere(inout RandomGeneratetor gen, uint spawnShape, float3 pos, float rad)
    {
        if (spawnShape == 0) // ランダム
        {
            return pos + RandomInSphere(gen, rad);
        }
        else if (spawnShape == 1) // 表面
        {
            return pos + normalize(gen.Generate3d_4()) * rad;
        }
    
        return float3(0, 0, 0);
    }
    float3 EmitPoint(inout RandomGeneratetor gen, ConstantBuffer<EmitterCommon> emitCommon, uint interpolation, uint countIndex, uint globalIndex, uint totalThreadCount)
    {
        if (interpolation == 0) // 補間なし
        {
            return emitCommon.translate;
        }
        else if (interpolation == 1) // 補間あり(ランダムに)
        {
            float t = gen.Generate1d_4(); // -1~1
            if (t < 0)
            {
                t *= -1.0f;
            }
            
            return lerp(emitCommon.prevTranslate, emitCommon.translate, t);
        }else if (interpolation == 2) // 補間あり(順番に)
        {
            uint emitIndex = (countIndex * totalThreadCount) + globalIndex;
            float t = (emitCommon.count > 1) ?
          ((float) emitIndex / (float) (emitCommon.count - 1)) : 0.0f;
            
            if(t > 1.0f)
            {
                uint t2 = uint(t);  
                t -= float(t2);
            }
            
            
            return lerp(emitCommon.prevTranslate, emitCommon.translate, t);
        }
        
        return emitCommon.translate;
    }
    
    
    // AABBの内部に一様分布で乱数を出す
    float3 EmitAABB(inout RandomGeneratetor gen, uint spawnShape, float3 pos, float3 size, uint count)
    {
        if (spawnShape == 0) // ランダム
        {
            return pos + gen.Generate3d_4() * size;
        }
        else if (spawnShape == 1) // 表面
        {
            // 6 面のうちランダムに1つ選択
            uint face = count % 6;
            float3 rnd = gen.Generate3d_4();

            float3 p = pos;
            if (face == 0) // +X面
                p += float3(size.x, rnd.y * size.y, rnd.z * size.z);
            else if (face == 1) // -X面
                p += float3(-size.x, rnd.y * size.y, rnd.z * size.z);
            else if (face == 2) // +Y面
                p += float3(rnd.x * size.x, size.y, rnd.z * size.z);
            else if (face == 3) // -Y面
                p += float3(rnd.x * size.x, -size.y, rnd.z * size.z);
            else if (face == 4) // +Z面
                p += float3(rnd.x * size.x, rnd.y * size.y, size.z);
            else if (face == 5) // -Z面
                p += float3(rnd.x * size.x, rnd.y * size.y, -size.z);

            return p;
        }
        else if (spawnShape == 2) // 辺
        {
            // AABBの12辺のうちランダムに1本を選択
            uint edge = count % 12;
            float t = gen.Generate1d_4(); // 0~1
            if (t < 0)
            {
                t *= -1.0f;
            }
            
            
            float3 v0, v1;
            float3 minP = pos - size;
            float3 maxP = pos + size;

        // 8頂点を用意
            float3 c[8] =
            {
                float3(minP.x, minP.y, minP.z),
            float3(maxP.x, minP.y, minP.z),
            float3(minP.x, maxP.y, minP.z),
            float3(maxP.x, maxP.y, minP.z),
            float3(minP.x, minP.y, maxP.z),
            float3(maxP.x, minP.y, maxP.z),
            float3(minP.x, maxP.y, maxP.z),
            float3(maxP.x, maxP.y, maxP.z)
            };

        // 12本の辺 (v0→v1)
            uint2 edges[12] =
            {
                uint2(0, 1), uint2(0, 2), uint2(0, 4),
            uint2(1, 3), uint2(1, 5),
            uint2(2, 3), uint2(2, 6),
            uint2(3, 7),
            uint2(4, 5), uint2(4, 6),
            uint2(5, 7),
            uint2(6, 7)
            };

            v0 = c[edges[edge].x];
            v1 = c[edges[edge].y];

            return lerp(v0, v1, t);
        }
        
        return float3(0, 0, 0);
    }
    
};

// 放出方向を決定する
class EmitDirections
{
    float3 EmitDirection(inout RandomGeneratetor gen, uint type, float3 pPos, float3 ePos, float3 velo, float3 veloRange, float force)
    {
        if (type == 0) // 全方向
        {
            return velo + gen.Generate3d_4() * veloRange;
        }
        else if (type == 1) // 外側
        {
            return normalize(pPos - ePos) * force;
        }
        else if (type == 2) // 内側
        {
            return -normalize(pPos - ePos) * force;
        }
        return float3(0, 0, 0);
    }
    
    
};


void EmitSetting_Set(
    inout RandomGeneratetor gen,
    inout Particle particle,
    ConstantBuffer<EmitterCommon> emitCommon,
    ConstantBuffer<EmitterTrail> emitTrail)
{
    particle.prevTranslate = particle.translate;
    particle.currentTime = 0;

    particle.lifeTime = emitCommon.lifeTime + gen.Generate1d_4() * emitCommon.lifeTimeRange;
    particle.scale = emitCommon.scale + gen.Generate3d_4() * emitCommon.scaleRange;
    particle.rotation = emitCommon.rotate + gen.Generate3d_4() * emitCommon.rotateRange;
    particle.color.rgb = emitCommon.color + gen.Generate3d_4() * emitCommon.colorRange;
    particle.color.a = 1.0f;

    particle.isAlpha = emitCommon.isAlpha;
    particle.isScaling = emitCommon.isScaling;
    particle.scaleAmount = emitCommon.scaleAmount;
    particle.acceleration = float3(0.0f, 0.0f, 0.0f);
    particle.trailHeadIndex = 0;
    particle.hasPrevQuad = false;
    particle.isAlive = true;
    particle.isGravity = emitCommon.isGravity;
    particle.useBillboard = emitCommon.useBillboard;

    particle.trailWidth = emitTrail.trailWidth;
    particle.trailColor = emitTrail.trailColor;
    particle.trailLifeTime = emitTrail.trailLifeTime;
    particle.isTrail = emitTrail.isTrail;
}