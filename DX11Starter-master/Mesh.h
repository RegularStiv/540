#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
class Mesh
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	int indiceNumber;
public:
	Mesh(Vertex* vertexArray, unsigned long long vertexNum, unsigned int* indices, unsigned long long indiceNum, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	Mesh(const char* file, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~Mesh();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	void Draw();
	void BufferCreation(Vertex* vertexArray, unsigned long long vertexNum, unsigned int* indices, unsigned long long indiceNum, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

