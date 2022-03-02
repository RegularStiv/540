#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
{
	this->mesh = mesh;
	this->material = material;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> mat)
{
	material = mat;
}

Transform* GameEntity::GetTransform()
{
	return &transform;
}

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
	std::shared_ptr<Camera> camera)
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	material->GetPixelShader()->SetFloat4("colorTint", material->GetColorTint());
	material->GetVertexShader()->SetMatrix4x4("world", transform.GetWorldMatrix());
	material->GetVertexShader()->SetMatrix4x4("view", camera->GetViewMatrix());
	material->GetVertexShader()->SetMatrix4x4("projection", camera->GetProjectionMatrix());

	material->GetVertexShader()->CopyAllBufferData();
	material->GetPixelShader()->CopyAllBufferData();
	// 
	// 
	//std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	//vs->SetFloat4("colorTint", material->GetColorTint());    // Strings here MUST  
	//vs->SetMatrix4x4("world", transform.GetWorldMatrix());   // match variable  
	//vs->SetMatrix4x4("view", camera->GetViewMatrix());             // names in the  
	//vs->SetMatrix4x4("projection", camera->GetProjectionMatrix()); // shader’s cbuffer!

	//vs->CopyAllBufferData(); // Adjust “vs” variable name if necessary

	mesh->Draw();
}
