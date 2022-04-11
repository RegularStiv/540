#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Mesh.h"
#include "Camera.h"
#include <memory>
#include "Material.h"
#include "Lights.h"
#include "WICTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false),
	transform()
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	entities = {};
	materials = {};
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundSpecSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickSpecSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickSRV;

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/ground-diffuse.jpg").c_str(), nullptr, groundSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/ground-spec2.png").c_str(), nullptr, groundSpecSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/stone-brick-diffuse.jpg").c_str(), nullptr, brickSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/stone-brick-spec1.png").c_str(), nullptr, brickSpecSRV.GetAddressOf());

	ambiant = XMFLOAT3(.1,.1,.25);
	std::shared_ptr<Material> mat1 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	mat1->AddTextureSRV("SurfaceTexture",groundSRV); 
	mat1->AddTextureSRV("SpecularTexture", groundSpecSRV);
	mat1->AddSampler("BasicSampler",sampler);

	materials.push_back(mat1);
	
	mat2->AddTextureSRV("SurfaceTexture", brickSRV);
	mat2->AddTextureSRV("SpecularTexture", brickSpecSRV);
	mat2->AddSampler("BasicSampler", sampler);
	
	materials.push_back(mat2);

	
	Light light = {};
	light.Type = LIGHT_TYPE_DIRECTIONAL;
	light.Direction = XMFLOAT3(1, 0, 0);
	light.Color = XMFLOAT3(1, 0, 0);
	light.Intensity = 1;
	lights.push_back(light);
	light = {};
	light.Type = LIGHT_TYPE_DIRECTIONAL;
	light.Direction = XMFLOAT3(0, 0, 1);
	light.Color = XMFLOAT3(0, 0, 1);
	light.Intensity = 1;
	lights.push_back(light);
	light = {};
	light.Type = LIGHT_TYPE_DIRECTIONAL;
	light.Direction = XMFLOAT3(0, 1, 0);
	light.Color = XMFLOAT3(0, 1, 0);
	light.Intensity = 1;
	lights.push_back(light);
	light = {};
	light.Type = LIGHT_TYPE_POINT;
	light.Position = XMFLOAT3(5, -5, 10);
	light.Range = 10;
	light.Color = XMFLOAT3(1, 1, 1);
	light.Intensity = 1;
	lights.push_back(light);
	light = {};
	light.Type = LIGHT_TYPE_POINT;
	light.Position = XMFLOAT3(-5, -5, 10);
	light.Range = 10;
	light.Color = XMFLOAT3(1, 1, 1);
	light.Intensity = 1;
	lights.push_back(light);
	CreateBasicGeometry();
	
	

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	camera = std::make_shared<Camera>(DirectX::XM_1DIV2PI, 0.0f, 0.0f, -5.0f, (float)width / height);

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, 
		GetFullPathTo_Wide(L"VertexShader.cso").c_str());    

	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"CustomPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device, context), materials[0]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context), materials[1]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device, context), materials[0]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context), materials[1]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device, context), materials[0]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad_double_sided.obj").c_str(), device, context), materials[1]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device, context), materials[0]));
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if (camera != 0)
	{
		camera->UpdateProjectionMatrix(DirectX::XM_1DIV2PI, (float)this->width / this->height, .1, 10);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//manually changed different values to show off different combinations of scaling translations and rotations
#pragma region entity drawing different transforms
	for (int i = 0; i < entities.size(); i++)
	{
		entities.at(i)->GetTransform()->SetRotation(0,totalTime/2,0);
	}
	for (int i = 0; i < entities.size(); i++) {
		float offset = -10 + (i * 3);
		entities.at(i)->GetTransform()->SetPosition(offset, -3, 10);
	}

	camera->Update(deltaTime);
#pragma endregion
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	

	//draws each entity meshes
	for (int i = 0; i < entities.size(); i++)
	{
		pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambiant);
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat("roughness", entities.at(i)->GetMaterial()->GetRoughness());
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());
		entities.at(i)->GetMaterial()->PrepareMaterial();
		entities.at(i)->Draw(context, camera);
	}


	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}