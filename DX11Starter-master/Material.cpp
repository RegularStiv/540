#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 colorTint)
{
    this->vertexShader = vertexShader;
    this->pixelShader = pixelShader;
    this->colorTint = colorTint;
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


