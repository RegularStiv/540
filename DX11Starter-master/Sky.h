#pragma once
#include <wrl/client.h>
#include <d3d11.h>
#include "Mesh.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include "Camera.h"
#include <memory>
class Sky
{
public:
	Sky(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera,Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV, std::shared_ptr < SimpleVertexShader> skyVertexShader, std::shared_ptr < SimplePixelShader> skyPixelShader);
	~Sky();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
private:
	std::shared_ptr<Camera> camera;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBufferComparison;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr < SimplePixelShader> skyPixelShader;
	std::shared_ptr < SimpleVertexShader> skyVertexShader;
};

