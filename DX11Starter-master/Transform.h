#pragma once
#include <DirectXMath.h>

class Transform
{
private:

public:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTranspose;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;
	Transform();
	Transform(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 worldInverseTranspose, 
		DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotaion);
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 xyz);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT4 quaternion);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 xyz);
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT4 GetQuaternion(); // XMFLOAT4 GetRotation() for quaternions 
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	void MoveAbsolute(float x, float y, float z);
	void Rotate(DirectX::XMFLOAT4 quaternionRotaion);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
	void UpdateMatricies();
	DirectX::XMMATRIX Translation();
	DirectX::XMMATRIX Scaling();
	DirectX::XMMATRIX RotationRollPitchYaw();
};

