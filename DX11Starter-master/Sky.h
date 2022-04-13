#pragma once
#include <wrl/client.h>
#include <memory>
#include "Game.h"
class Sky
{
public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device);
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerOptions;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthBufferComparison;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr < SimplePixelShader> skyPixelShader;
	std::shared_ptr < SimpleVertexShader> skyVertexShader;
};

