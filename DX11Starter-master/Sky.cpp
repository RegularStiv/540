#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV, std::shared_ptr<SimpleVertexShader> skyVertexShader, std::shared_ptr<SimplePixelShader> skyPixelShader)
{
	this->camera = camera;
	this->mesh = mesh;
	samplerOptions = sampler;
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rasterDesc, &rasterizerState);

	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&stencilDesc, &depthBufferComparison);
	this->cubeSRV = cubeSRV;
	this->skyPixelShader = skyPixelShader;
	this->skyVertexShader = skyVertexShader;
}

Sky::~Sky()
{
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	skyPixelShader->SetShader();
	skyVertexShader->SetShader();
	skyPixelShader->SetSamplerState("BasicSampler",samplerOptions);
	skyPixelShader->SetShaderResourceView("CubeTexture",cubeSRV);

	skyVertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
	skyVertexShader->SetMatrix4x4("projection",camera->GetProjectionMatrix());
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthBufferComparison.Get(),0);

	
	
	skyVertexShader->CopyAllBufferData();

	mesh->Draw();
	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(0,0);
}
