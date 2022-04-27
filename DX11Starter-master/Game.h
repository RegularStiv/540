#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "Mesh.h"
#include "GameEntity.h"
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> customPixelShader;
	std::shared_ptr<SimpleVertexShader> skyVertexShader;
	std::shared_ptr<SimplePixelShader> skyPixelShader;
	std::shared_ptr<SimplePixelShader> celShader;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> celSampler;
	std::shared_ptr<Sky> sky;
	std::shared_ptr<Mesh> triangle;
	std::shared_ptr<Mesh> rect;
	std::shared_ptr<Mesh> pentagon;
	std::vector<std::shared_ptr<GameEntity>> entities;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<Light> lights;

	std::shared_ptr<Camera> camera;
	DirectX::XMFLOAT3 ambiant;
	Transform transform;
};

