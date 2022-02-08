#include "Transform.h"

Transform::Transform()
{
    worldMatrix = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    worldInverseTranspose = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    position = DirectX::XMFLOAT3(0,0,0);
    scale = DirectX::XMFLOAT3(1, 1, 1);
    rotation = DirectX::XMFLOAT4(0,0,0,0);
}

Transform::Transform(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 worldInverseTranspose, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotaion)
{
    this->worldMatrix = world;
    this->worldInverseTranspose = worldInverseTranspose;
    this->position = position;
    this->scale = scale;
    this->rotation = rotaion;
}

void Transform::SetPosition(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    float cp = cos(pitch * .5);
    float sp = sin(pitch * .5);
    float cy = cos(yaw * .5);
    float sy = sin(yaw * .5);
    float cr = cos(roll * .5);
    float sr = sin(roll * .5);

    float w = cr * cp * cy + sr * sp * sy;
    float x = sr * cp * cy - cr * sp * sy;
    float y = cr * sp * cy + sr * cp * sy;
    float z = cr * cp * sy - sr * sp * cy;

    rotation = DirectX::XMFLOAT4(x,y,z,w);
}

void Transform::SetScale(float x, float y, float z)
{
    scale = DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
    return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{   
    
    //pitch
    float sinp = 2 * (rotation.w * rotation.y - rotation.z * rotation.x);
    float pitch = 0;
    if (abs(sinp) >= 1)
    {
        pitch = copysign(DirectX::XM_PI / 2, sinp);
    }
    else
    {
        pitch = asin(sinp);
    }
   
    //yaw
    float siny_cosp = 2 * (rotation.w * rotation.z + rotation.x * rotation.y);
    float cosy_cosp = 1 - 2 * (rotation.y * rotation.y + rotation.z * rotation.z);
    float yaw = atan2(siny_cosp, cosy_cosp);

    //roll
    float sinrCosp = 2 * (rotation.w * rotation.x + rotation.y * rotation.z);
    float cosrCosp = 1 - (2 * (rotation.x * rotation.x + rotation.y * rotation.y));
    float roll = atan2(sinrCosp, cosrCosp);

    return DirectX::XMFLOAT3(pitch, yaw, roll);
}

DirectX::XMFLOAT3 Transform::GetScale()
{
    return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
    return worldInverseTranspose;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
    DirectX::XMFLOAT3 pyr = GetPitchYawRoll();
    pyr.x += pitch;
    pyr.y += yaw;
    pyr.z += roll;
    SetRotation(pyr.x, pyr.y, pyr.z);
}

void Transform::Scale(float x, float y, float z)
{
    scale.x *= x;
    scale.y *= y;
    scale.z *= z;
}

void Transform::UpdateMatricies()
{
    DirectX::XMMATRIX world = (Scaling() * RotationRollPitchYaw() * Translation());

    DirectX::XMStoreFloat4x4(&worldMatrix, world);
    DirectX::XMStoreFloat4x4(&worldInverseTranspose, DirectX::XMMatrixTranspose(world));
}

DirectX::XMMATRIX Transform::Translation()
{
    return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX Transform::Scaling()
{
    return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
}

DirectX::XMMATRIX Transform::RotationRollPitchYaw()
{
    DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
    return DirectX::XMMatrixRotationQuaternion(quaternion);;
}
