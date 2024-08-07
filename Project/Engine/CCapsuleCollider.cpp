#include "pch.h"
#include "CCapsuleCollider.h"
#include "components.h"

CCapsuleCollider::CCapsuleCollider()
    : CCollider(COMPONENT_TYPE::CAPSULECOLLIDER)
    , m_Radius(0.5f)
    , m_Height(2.f)
    , m_Direction(AXIS_TYPE::Y)
{
}

CCapsuleCollider::~CCapsuleCollider()
{
}

void CCapsuleCollider::begin()
{
    CCollider::begin();
}

void CCapsuleCollider::finaltick()
{
    CCollider::finaltick();

    // 콜라이더 비활성화 상태에서는 렌더링 X
    if (!m_bEnabled)
        return;

    Vec3 scale = Transform()->GetWorldScale();
    const float WorldRatio = Transform()->GetWorldRatio();

    float HalfHeight = m_Height / 2.f - m_Radius;

    float RadiusScale = 1.f;
    float HeightScale = 1.f;
    Matrix matPhysXScale = Matrix();

    switch (m_Direction)
    {
    case AXIS_TYPE::X: {
        RadiusScale = scale.y > scale.z ? scale.y : scale.z;
        HeightScale = scale.x;

        matPhysXScale = XMMatrixScaling(HeightScale * HalfHeight, RadiusScale * m_Radius, RadiusScale * m_Radius);
    }
    break;
    case AXIS_TYPE::Y: {
        RadiusScale = scale.x > scale.z ? scale.x : scale.z;
        HeightScale = scale.y;

        matPhysXScale = XMMatrixScaling(RadiusScale * m_Radius, HeightScale * HalfHeight, RadiusScale * m_Radius);
    }
    break;
    case AXIS_TYPE::Z: {
        RadiusScale = scale.x > scale.y ? scale.x : scale.y;
        HeightScale = scale.z;

        matPhysXScale = XMMatrixScaling(RadiusScale * m_Radius, RadiusScale * m_Radius, HeightScale * HalfHeight);
    }
    break;
    }

    Matrix matCenterTrans = XMMatrixTranslation(WorldRatio * m_Center.x, WorldRatio * m_Center.y, WorldRatio * m_Center.z);
    Matrix matWorldScaleInv = XMMatrixScaling(1.f / scale.x, 1.f / scale.y, 1.f / scale.z);
    Vec3 color = m_CollisionCount > 0 || m_TriggerCount > 0 ? Vec3(1.f, 0.f, 0.f) : Vec3(0.f, 1.f, 0.f);

    // PhysX SRT * Transform SRT
    GamePlayStatic::DrawDebugCapsule(matPhysXScale * matCenterTrans * matWorldScaleInv * Transform()->GetWorldMat(), 1.f, 1.f, m_Direction, color,
                                     true);
}

void CCapsuleCollider::SetRadius(float _Radius)
{
    m_Radius = _Radius;
    GamePlayStatic::Physics_Event(GetOwner(), Physics_EVENT_TYPE::RESPAWN);
}

void CCapsuleCollider::SetHeight(float _Height)
{
    m_Height = _Height;
    GamePlayStatic::Physics_Event(GetOwner(), Physics_EVENT_TYPE::RESPAWN);
}

void CCapsuleCollider::SetAxisDirection(AXIS_TYPE _Dir)
{
    m_Direction = _Dir;
    GamePlayStatic::Physics_Event(GetOwner(), Physics_EVENT_TYPE::RESPAWN);
}

UINT CCapsuleCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider::SaveToLevelFile(_File);
    fwrite(&m_Radius, sizeof(float), 1, _File);
    fwrite(&m_Height, sizeof(float), 1, _File);
    fwrite(&m_Direction, sizeof(AXIS_TYPE), 1, _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(AXIS_TYPE);

    return MemoryByte;
}

UINT CCapsuleCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider::LoadFromLevelFile(_File);
    fread(&m_Radius, sizeof(float), 1, _File);
    fread(&m_Height, sizeof(float), 1, _File);
    fread(&m_Direction, sizeof(AXIS_TYPE), 1, _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(AXIS_TYPE);

    return MemoryByte;
}
