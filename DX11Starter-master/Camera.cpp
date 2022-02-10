#include "Camera.h"

Camera::Camera(float x, float y, float z, float aspectRatio)
{
    transform.SetPosition(x, y, z);
    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
}

void Camera::UpdateViewMatrix()
{
    DirectX::XMFLOAT3 pos = transform.GetPosition();
    DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0, 1, 0);
    DirectX::XMFLOAT4 rot = transform.GetQuaternion();
    DirectX::XMVECTOR forward = DirectX::XMVector3Rotate(
        DirectX::XMVectorSet(0, 0, 1, 0),
        DirectX::XMLoadFloat4(&rot));

    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&pos),forward, DirectX::XMLoadFloat3(&up));
    DirectX::XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    DirectX::XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XM_PIDIV4,
        aspectRatio,
        0.01f,
        100.0f);
    DirectX::XMStoreFloat4x4(&projectionMatrix, proj);
}

Transform* Camera::GetTransform()
{
    return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return projectionMatrix;
}
