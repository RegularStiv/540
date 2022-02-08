#pragma once
#include "Transform.h"
#include <memory>
#include "Mesh.h"
#include <DirectXMath.h>
#include "BufferStructs.h"
class GameEntity
{
public:
	
	GameEntity(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
		Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout);
private:
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Mesh> mesh;
};
