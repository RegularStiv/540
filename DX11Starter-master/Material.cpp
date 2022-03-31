#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 colorTint, float roughness)
{
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
    this->colorTint = colorTint;
    if (roughness > 1.0f) roughness = 1.0f;
    if (roughness < 1.0f) roughness = 0.0f;
    this->roughness = roughness;
}
Material::~Material() {
}
std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
    return colorTint;
}

float Material::GetRoughness()
{
    return this->roughness;
}



void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader)
{
    this->pixelShader = pixelShader;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader)
{
    this->vertexShader = vertexShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 colorTint)
{
    this->colorTint = colorTint;
}

void Material::AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs.insert({ shaderName, srv });
}

void Material::AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ shaderName, sampler });
}

void Material::PrepareMaterial(std::shared_ptr<SimplePixelShader> ps)
{
    for (auto& t : textureSRVs) { ps->SetShaderResourceView(t.first.c_str(), t.second); }
    for (auto& s : samplers) { ps->SetSamplerState(s.first.c_str(), s.second); }
}
