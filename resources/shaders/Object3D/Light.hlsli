static const int kMaxLight = 3;

// ���s����
struct DirectionalLight
{
    float4 color; //!< ���C�g�̐F
    float3 direction; //!< ���C�g�̌���
    float intensity; //!< �P�x
    float ilg; // ���O
    int enableLighting;
    float3 groundColor; // �n�ʐF
    float3 skyColor; // �V���F
    float3 groundNormal; // �n�ʖ@������
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

// �|�C���g���C�g
struct PointLight
{
    float4 color; //���C�g�F
    float3 position; // ���C�g�ʒu
    float intensity; //�P�x
    float radius; // !< ���C�g�̓͂��ő勗��
    float decay; //!< ������ 
    float ilg; // ���O
    int enableLighting;
};


// �����|�C���g���C�g
struct PointLights
{
    PointLight pointLights[kMaxLight];
};
ConstantBuffer<PointLights> gPointLight : register(b3);

// �X�|�b�g���C�g
struct SpotLight
{
    float4 color; //���C�g�F
    float3 position; // ���C�g�ʒu
    float intensity; //�P�x
    float3 direction; //!< ���C�g�̌���
    float distance; //!< ���C�g�͂�����
    float decay; //!< ������ 
    float cosAngle; //!< �X�|�b�g���C�g�̗]��
    float cosFalloffStart;
    int enableLighting;
};

// �����|�C���g���C�g
struct SpotLights
{
    SpotLight spotLights[kMaxLight];
};
ConstantBuffer<SpotLights> gSpotLight : register(b4);


struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION0;
    
    float3 tangent : TANGENT0;
    float3 biNormal : BINORMAL0;
    float3 transformedNormal : NORMAL1; // ���_�V�F�[�_����n���ꂽ�@�� 
};


//�F�ȂǎO�p�`�̕\�ʂ̍ގ������肷�����Material
struct Material
{
    
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
    int useLig;
    int useHem;
    int useNormalMap;
    int useSpeculerMap;
};
ConstantBuffer<Material> gMaterial : register(b0);

float Cos(float3 dire, float3 toEye, float3 normal)
{
    float3 halfVectorv = normalize((-dire) + toEye);
    float NdotL = dot(normalize(normal), halfVectorv);

    return pow(NdotL * 0.5f + 0.5f, 2.0f);
}

float SpecularPow(float3 dire, float3 toEye, float3 normal, float shininess)
{
    float3 reflectLight = reflect(dire, normalize(normal));
    float RdotE = dot(reflectLight, toEye);
   
    return pow(saturate(RdotE), shininess);
}

float SpecularPow2(float3 dire, float3 toEye, float3 normal, float shininess)
{
    float3 reflectLight = reflect(dire, normalize(normal)); // ���˃x�N�g��
    float RdotE = dot(reflectLight, toEye); // �����x�N�g���Ɣ��˃x�N�g���̃h�b�g��

    return pow(saturate(RdotE), shininess);
}

float3 DirectionalLightFunc(PixelShaderInput input,float4 textureColor, float3 toEye, float3 normal)
{
    float3 diffuse = { 0, 0, 0 };
    float3 specular = { 0, 0, 0 };
        
    if (gDirectionalLight.enableLighting)
    {
       
              // ���s�����̏���
              // �g�U����
        float cos = Cos(gDirectionalLight.direction, toEye, normal);

            
              // ���ʔ���
        float specularPow = 0.0f;
        if (gMaterial.shininess >= 1.0f)
        {
            specularPow = SpecularPow(gDirectionalLight.direction, toEye, normal, gMaterial.shininess);
        }
            
        if (gMaterial.useSpeculerMap)
        {
              //  float specPower = g_Specularmap.Sample(sSampler, input.texcoord).r;
            
              //  specularPow *= specPower * 10.0f;
                
              //  amdientPower = g_aoMap.Sample(sSampler, input.texcoord).r;
        }
                  
            
            
            
            
            // �������C�g
        float3 limColor = { 0, 0, 0 };
            //float power1 = 1.0f - max(0.0f, dot(gDirectionalLight.direction, input.normal));
            //float power2 = 1.0f - max(0.0f, input.normal.z * -1.0f);
            //float limPower = power1 * power2;
            //limPower = pow(limPower, 1.3f);
            
            //limColor = limPower * gDirectionalLight.color.rgb;
            
            
            
            //   // �������C�g
        float3 hemiLight = { 0, 0, 0 };
            //if (gMaterial.useHem != 0)
            //{
            //    float t = dot(normalize(input.normal), normalize(gDirectionalLight.groundNormal));
            //    t = (t + 1.0f) / 2.0f;
            //    hemiLight = lerp(gDirectionalLight.groundColor, gDirectionalLight.skyColor, t);
            //}

               // �g�U����
        diffuse = gMaterial.color.rgb * textureColor.rgb * cos * gDirectionalLight.intensity;

            //diffuseDirectionalLight /= 3.1415926f;
            
               // ���ʔ���
        specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float3(1.0f, 1.0f, 1.0f);

            
            
            
             // ����
            //directionalLig = diffuseDirectionalLight + specularDirectionalLight + limColor;
            
        if (gMaterial.useNormalMap)
        {
               // directionalLig *= float3(amdientPower, amdientPower, amdientPower);
        }
            //directionalLig.x += gDirectionalLight.ilg;
            //directionalLig.y += gDirectionalLight.ilg;
            //directionalLig.z += gDirectionalLight.ilg;
    
            //directionalLig += hemiLight;
    }
    
    return diffuse + specular;
};

float3 PointLightFunc(PixelShaderInput input, float4 textureColor, float3 toEye, float3 normal)
{
    float3 diffusePointLight = { 0.0f, 0.0f, 0.0f };
    float3 specularPointLight = { 0.0f, 0.0f, 0.0f };       
    for (int point_i = 0; point_i < kMaxLight; point_i++)
    {
        if (gPointLight.pointLights[point_i].enableLighting == 0)
        {
            continue;
        }
            
            
            
        float3 pointLightDirection = normalize(input.worldPosition - gPointLight.pointLights[point_i].position);
    
            
        float cosP = Cos(pointLightDirection, toEye, normal);
            //���ʔ���
        float specularPowP = SpecularPow2(pointLightDirection, toEye, normal, gMaterial.shininess);
        
            
        float distanceP = length(gPointLight.pointLights[point_i].position - input.worldPosition);
        float attenuationFactorP = saturate(1.0 - (distanceP / gPointLight.pointLights[point_i].radius));
 
            
        float intensity = gPointLight.pointLights[point_i].intensity;
        if (gMaterial.useLig)
        {
            intensity *= gPointLight.pointLights[point_i].ilg;

        }
            
            
        diffusePointLight += gPointLight.pointLights[point_i].color.rgb * cosP * intensity * attenuationFactorP;
        specularPointLight += gPointLight.pointLights[point_i].color.rgb * gPointLight.pointLights[point_i].intensity * specularPowP * attenuationFactorP * float3(1.0f, 1.0f, 1.0f);
            
            
    }
        
    diffusePointLight *= gMaterial.color.rgb * textureColor.rgb;
    
    
    return (diffusePointLight + specularPointLight);
}

float3 SpotLightFunc(PixelShaderInput input, float4 textureColor, float3 toEye, float3 normal)
{
    float3 diffuseSpotLight = { 0.0f, 0.0f, 0.0f };
    float3 specularSpotLight = { 0.0f, 0.0f, 0.0f };
    for (uint32_t spot_i = 0; spot_i < kMaxLight; spot_i++)
    {
        if (gSpotLight.spotLights[spot_i].enableLighting == 0)
        {
            continue;
        }
            
        float3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.spotLights[spot_i].position);
     
           
        
        
        // ���������̌v�Z
        float distanceS = length(gSpotLight.spotLights[spot_i].position - input.worldPosition);
        float attenuationFactorS = pow(saturate(-distanceS / gSpotLight.spotLights[spot_i].distance + 1.0), gSpotLight.spotLights[spot_i].decay);
        
        // �t�H�[���I�t�̌v�Z
        float cosAngle = dot(spotLightDirectionOnSurface, normalize(gSpotLight.spotLights[spot_i].direction));
        float falloffFactor = saturate((cosAngle - gSpotLight.spotLights[spot_i].cosAngle) / (gSpotLight.spotLights[spot_i].cosFalloffStart - gSpotLight.spotLights[spot_i].cosAngle));
       
            
            
        float cosS = Cos(spotLightDirectionOnSurface, toEye, normal);
        float specularPowS = SpecularPow2(spotLightDirectionOnSurface, toEye, normal, gMaterial.shininess);
            
             // �X�|�b�g���C�g�̊g�U���˂Ƌ��ʔ��˂̌v�Z
        diffuseSpotLight += gSpotLight.spotLights[spot_i].color.rgb * cosS * gSpotLight.spotLights[spot_i].intensity * attenuationFactorS * falloffFactor;
        specularSpotLight += gSpotLight.spotLights[spot_i].color.rgb * gSpotLight.spotLights[spot_i].intensity * specularPowS * attenuationFactorS * falloffFactor;
    }
        
    diffuseSpotLight *= gMaterial.color.rgb * textureColor.rgb;
    
    
    return (diffuseSpotLight + specularSpotLight);
}