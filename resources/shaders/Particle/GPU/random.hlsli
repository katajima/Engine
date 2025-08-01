

float3 rand3dTo3d(float3 p)
{
    p = frac(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return frac((p.xxy + p.yzz) * p.zyx);
}

float rand3dTo1d(float3 p)
{
    p = frac(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return frac(dot((p.xxy + p.yzz) * p.zyx, float3(1.0, 1.0, 1.0)));
}

float3 hash33(float3 p)
{
    const float3x3 m =
    {
        0.00, 0.80, 0.60,
        0.60, 0.00, 0.80,
        0.80, 0.60, 0.00
    };

    p = frac(mul(p, m));
    p += dot(p, p.yzx + 19.19);
    return frac((p.xxy + p.yzz) * p.zyx);
}

float hash31(float3 p)
{
    return frac(dot(hash33(p), float3(1.0, 1.0, 1.0)));
}

float rand(float3 vec)
{
    float random = dot(vec, float3(12.9898, 78.233, 37.719));
    random = frac(random * 143758.5453);
    return random;
}



class RandomGeneratetor
{
    float3 seed;
       
    float3 Generate3d_1()
    {
        float x, y, z;
        
        seed = frac(sin(seed * 12.9898) * 43758.5453);
        x = frac(sin(seed.x + 1.0) * 43758.5453) * 2.0f - 1.0f;
        y = frac(sin(seed.y + 2.0) * 43758.5453) * 2.0f - 1.0f;
        z = frac(sin(seed.z + 3.0) * 43758.5453) * 2.0f - 1.0f;
        return float3(x, y, z);
    }
    
    uint counter;

    float3 Generate3d_2()
    {
        float3 offset = float3(counter, counter * 31, counter * 59);
        ++counter;
        seed = frac(sin(seed * 12.9898) * 43758.5453);
        return rand3dTo3d(seed + offset) * 2.0f - 1.0f;
    }

    float Generate1d_2()
    {
        float3 offset = float3(counter, counter * 31, counter * 59);
        ++counter;
        seed = frac(sin(seed * 12.9898) * 43758.5453);
        return rand3dTo1d(seed + offset) * 2.0f - 1.0f;
    }
    
    float3 Generate3d_3()
    {
        seed = hash33(seed) * 2.0f - 1.0f;
        return seed;
    }

    
    
    float Generate1d_4(float3 dotDir = float3(12.9898, 78.233, 37.719))
    {
        seed = sin(seed);
        float random = dot(seed, dotDir);
        random = frac(sin(random) * 143758.5453) * 2.0f - 1.0f;
        return random;
    }
    
    float3 Generate3d_4(float3 dotDir = float3(12.9898, 78.233, 37.719))
    {
        return float3(
        Generate1d_4(float3(12.989, 78.233, 37.719)),
        Generate1d_4(float3(39.346, 11.135, 83.155)),
        Generate1d_4(float3(73.156, 52.235, 09.151))
    );
    }
    
};