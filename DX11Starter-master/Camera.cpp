#include "Camera.h"
using namespace DirectX;

Camera::Camera(float fov, float x, float y, float z, float aspectRatio)
{
    this->fov = fov;
    transform.SetPosition(x, y, z);
    UpdateViewMatrix();
    UpdateProjectionMatrix(fov,aspectRatio,.1,900);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
    Input& input = Input::GetInstance();
    float speed = 1.0f;

    if (input.KeyDown('W')) { transform.MoveRelative(0, 0, speed * dt); }
    if (input.KeyDown('S')) { transform.MoveRelative(0, 0, -speed * dt); }
    if (input.KeyDown('A')) { transform.MoveRelative(-speed * dt, 0, 0); }
    if (input.KeyDown('D')) { transform.MoveRelative(speed * dt, 0, 0); }
    if (input.KeyDown('Q')) { transform.MoveAbsolute(0, -speed * dt, 0); }
    if (input.KeyDown('E')) { transform.MoveAbsolute(0, speed * dt, 0); }
    float rotSpeed = 2 * dt;
    if (input.MouseLeftDown())
    {
        
        float cursorMovementX = input.GetMouseXDelta() * rotSpeed;
        float cursorMovementY = input.GetMouseYDelta() * rotSpeed;
        /* Other mouse movement code here */

        transform.Rotate(cursorMovementY, cursorMovementX, 0);
    }
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // Get the camera's forward vector and position
    XMFLOAT3 forward = transform.GetForward();
    XMFLOAT3 pos = transform.GetPosition();

    // Make the view matrix and save
    XMMATRIX view = XMMatrixLookToLH(
        XMLoadFloat3(&pos),
        XMLoadFloat3(&forward),
        XMVectorSet(0, 1, 0, 0)); // World up axis
    XMStoreFloat4x4(&viewMatrix, view);
}
void Camera::UpdateProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane)
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
