#pragma once
#include "SimpleShader.h"
#include <memory>
class Material
{
public:
	Material(std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, DirectX::XMFLOAT4 colorTint);
	~Material();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	DirectX::XMFLOAT4 GetColorTint();

	void SetPixelShader(std::shared_ptr<SimplePixelShader> pixelShader);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> vertexShader);
	void SetColorTint(DirectX::XMFLOAT4 colorTint);
private:
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	DirectX::XMFLOAT4 colorTint;
};

