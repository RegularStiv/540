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
	Microsoft::WRL::ComPtr<ID3D11SamplerState> celSampler;
	D3D11_SAMPLER_DESC sampDesc = {};

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, sampler.GetAddressOf());
	
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	device->CreateSamplerState(&sampDesc, celSampler.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundSpecSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> groundNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickMetalSRV; 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickRoughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brickNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeRoughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeNormalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> celFourBandTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> celTwoBandTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> celSpecTexture;


	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/ground-diffuse.jpg").c_str(), nullptr, groundSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/ground-spec2.png").c_str(), nullptr, groundSpecSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/ground-normal.png").c_str(), nullptr, groundNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/cobblestone_albedo.png").c_str(), nullptr, brickSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/cobblestone_metal.png").c_str(), nullptr, brickMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/cobblestone_roughness.png").c_str(), nullptr, brickRoughnessSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/cobblestone_normals.png").c_str(), nullptr, brickNormalSRV.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/bronze_albedo.png").c_str(), nullptr, bronzeSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/bronze_metal.png").c_str(), nullptr, bronzeMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/bronze_roughness.png").c_str(), nullptr, bronzeRoughnessSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/bronze_normals.png").c_str(), nullptr, bronzeNormalSRV.GetAddressOf());


	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/toonRamp1.png").c_str(), nullptr, celFourBandTexture.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/toonRamp3.png").c_str(), nullptr, celTwoBandTexture.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/textures/toonRampSpecular.png").c_str(), nullptr, celSpecTexture.GetAddressOf());


	skySRV = CreateCubemap(GetFullPathTo_Wide(L"../../Assets/textures/right.png").c_str(), GetFullPathTo_Wide(L"../../Assets/textures/left.png").c_str(), GetFullPathTo_Wide(L"../../Assets/textures/up.png").c_str(), GetFullPathTo_Wide(L"../../Assets/textures/down.png").c_str(), GetFullPathTo_Wide(L"../../Assets/textures/front.png").c_str(), GetFullPathTo_Wide(L"../../Assets/textures/back.png").c_str());


	ambiant = XMFLOAT3(.1,.1,.25);
	std::shared_ptr<Material> mat1 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat2 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat3 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat4 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat5 = std::make_shared<Material>(vertexShader, pixelShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat6 = std::make_shared<Material>(vertexShader, celShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);
	std::shared_ptr<Material> mat7 = std::make_shared<Material>(vertexShader, celShader, DirectX::XMFLOAT4(1, 1, 1, 1), .2);


	mat1->AddTextureSRV("SpecularTexture", groundSpecSRV);
	mat1->AddTextureSRV("NormalTexture", groundNormalSRV);
	mat1->AddSampler("BasicSampler",sampler);

	materials.push_back(mat1);
	
	mat2->AddTextureSRV("SurfaceTexture", brickSRV);
	mat2->AddTextureSRV("MetalTexture", brickMetalSRV);
	mat2->AddTextureSRV("NormalTexture", brickNormalSRV);
	mat2->AddTextureSRV("RoughnessTexture", brickRoughnessSRV);
	mat2->AddSampler("BasicSampler", sampler);
	
	materials.push_back(mat2);

	mat3->AddTextureSRV("SurfaceTexture", rockSRV);
	mat3->AddTextureSRV("NormalTexture", rockNormalSRV);
	mat3->AddSampler("BasicSampler", sampler);
	materials.push_back(mat3);

	mat4->AddTextureSRV("SurfaceTexture", cushionSRV);
	mat4->AddTextureSRV("NormalTexture", cushionNormalSRV);
	mat4->AddSampler("BasicSampler", sampler);
	materials.push_back(mat4);
	
	mat5->AddTextureSRV("SurfaceTexture", bronzeSRV);
	mat5->AddTextureSRV("MetalTexture", bronzeMetalSRV);
	mat5->AddTextureSRV("NormalTexture", bronzeNormalSRV);
	mat5->AddTextureSRV("RoughnessTexture", bronzeRoughnessSRV);
	mat5->AddSampler("BasicSampler", sampler);

	materials.push_back(mat5);

	mat6->AddTextureSRV("SurfaceTexture", brickSRV);
	mat6->AddTextureSRV("NormalTexture", brickNormalSRV);
	mat6->AddTextureSRV("CelTexture", celFourBandTexture);
	mat6->AddTextureSRV("MetalMap", brickMetalSRV);
	mat6->AddTextureSRV("CelSpecularTexture", celSpecTexture);
	mat6->AddTextureSRV("RoughnessTexture", brickRoughnessSRV);
	mat6->AddSampler("BasicSampler", sampler);
	mat6->AddSampler("CelSampler", celSampler);

	materials.push_back(mat6);

	mat7->AddTextureSRV("SurfaceTexture", bronzeSRV);
	mat7->AddTextureSRV("NormalTexture", bronzeNormalSRV);
	mat7->AddTextureSRV("MetalMap", bronzeMetalSRV);
	mat7->AddTextureSRV("CelTexture", celTwoBandTexture);
	mat7->AddTextureSRV("CelSpecularTexture", celSpecTexture);
	mat7->AddTextureSRV("RoughnessTexture", bronzeRoughnessSRV);
	mat7->AddSampler("BasicSampler", sampler);
	mat7->AddSampler("CelSampler", celSampler);

	materials.push_back(mat7);

	Light light = {};
	light.Type = LIGHT_TYPE_DIRECTIONAL;
	light.Direction = XMFLOAT3(1, -1, 1);
	light.Color = XMFLOAT3(1, 1, 1);
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
	//Sky(std::shared_ptr<Mesh> mesh, std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device, 
	//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV, std::shared_ptr < SimpleVertexShader> skyVertexShader, std::shared_ptr < SimplePixelShader> skyPixelShader)
	sky = std::make_shared<Sky>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context), camera, sampler, device, skySRV, skyVertexShader, skyPixelShader);
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
	celShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"CelShadingPS.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context,
		GetFullPathTo_Wide(L"CustomPS.cso").c_str());
	skyPixelShader = std::make_shared<SimplePixelShader>(device,context, 
		GetFullPathTo_Wide(L"SkyPS.cso").c_str());
	skyVertexShader = std::make_shared<SimpleVertexShader>(device, context,
		GetFullPathTo_Wide(L"SkyVS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device, context), materials[6]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context), materials[5]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device, context), materials[6]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context), materials[5]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device, context), materials[1]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/quad_double_sided.obj").c_str(), device, context), materials[4]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device, context), materials[1]));
	entities.push_back(std::make_shared<GameEntity>(std::make_shared<Mesh>(GetFullPathTo("../../Assets/Models/crate_wood.obj").c_str(), device, context), materials[6]));
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
		celShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambiant);
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat("roughness", entities.at(i)->GetMaterial()->GetRoughness());
		entities.at(i)->GetMaterial()->GetPixelShader()->SetFloat3("cameraPosition", camera->GetTransform()->GetPosition());

		entities.at(i)->GetMaterial()->PrepareMaterial();
		entities.at(i)->Draw(context, camera);
	}

	sky->Draw(context);
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Specifically NOT generating mipmaps, as we usually don't need them for the sky!
	// - Order matters here! +X, -X, +Y, -Y, +Z, -Z
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)&textures[5], 0);
	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);
	// Describe the resource for the cube map, which is simply
	// a "texture 2d array". This is a special GPU resource format,
	// NOT just a C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width; // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE MAP, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);
	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero, since there's only one)
			i, // Which array element?
			1); // How many mip levels are in the texture?
			// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i], // Source resource
			0, // Source subresource index (we're assuming there's only one)
			0); // Source subresource "box" of data to copy (zero means the whole thing)
	}
	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see
	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());
	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release(); // Done with this particular reference (the SRV has another)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();
	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}