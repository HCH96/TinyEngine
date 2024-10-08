#include "pch.h"
#include "CCircleCollider2D.h"

#include "components.h"

CCircleCollider2D::CCircleCollider2D()
    : CCollider2D(COMPONENT_TYPE::CIRCLECOLLIDER2D)
    , m_Radius(1.f)

{
}

CCircleCollider2D::~CCircleCollider2D()
{
}

void CCircleCollider2D::begin()
{
    CCollider2D::begin();
}

void CCircleCollider2D::finaltick()
{
    CCollider2D::finaltick();

    // 콜라이더 비활성화 상태에서는 렌더링 X
    if (!m_bEnabled)
        return;

    Vec3 sacle = Transform()->GetWorldScale();

    Matrix matWorld = Transform()->GetWorldMat();
    Matrix matTranslation = XMMatrixTranslation(m_Offset.x * sacle.x, m_Offset.y * sacle.y, 0.0f);
    Matrix matScale = XMMatrixScaling(m_Radius * sacle.x, m_Radius * sacle.x, 1.f);
    Matrix matInvScale = XMMatrixScaling(1.f / sacle.x, 1.f / sacle.y, 1.f / sacle.z);

    Vec3 color = m_CollisionCount > 0 || m_TriggerCount > 0 ? Vec3(1.f, 0.f, 0.f) : Vec3(0.f, 1.f, 0.f);

    GamePlayStatic::DrawDebugCircle(matScale * matTranslation * matInvScale * matWorld, color, false);
}

void CCircleCollider2D::SetRadius(float _Radius)
{
    if (m_Radius == _Radius)
        return;

    m_Radius = _Radius;
    GamePlayStatic::Physics2D_Event(GetOwner(), Physics2D_EVENT_TYPE::RESPAWN);
}

UINT CCircleCollider2D::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider2D::SaveToLevelFile(_File);
    fwrite(&m_Radius, sizeof(float), 1, _File);

    MemoryByte += sizeof(float);

    return MemoryByte;
}

UINT CCircleCollider2D::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += CCollider2D::LoadFromLevelFile(_File);
    fread(&m_Radius, sizeof(float), 1, _File);

    MemoryByte += sizeof(float);

    return MemoryByte;
}
