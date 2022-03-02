#pragma once
#include "Transform.h"
#include <memory>
#include "Mesh.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "Material.h"
class GameEntity
{
public:
	
	GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	void SetMaterial(std::shared_ptr<Material> mat);
	Transform* GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
		std::shared_ptr<Camera> camera);
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

