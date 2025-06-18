

class Collider
{
    bool CollAABBPoint(float3 range,float3 aabbPos,float3 pointPos)
    {
        float3 min = aabbPos - range;
        float3 max = aabbPos + range;

        return all(pointPos >= min) && all(pointPos <= max);
    }
};
