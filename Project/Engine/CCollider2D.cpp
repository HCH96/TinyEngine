#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"

CCollider2D::CCollider2D()
    : CComponent(COMPONENT_TYPE::COLLIDER2D)
    , m_vOffsetPos(Vec3())
    , m_vOffsetScale(Vec3(1.f, 1.f, 1.f))
    , m_CollisionCount(0)
    , m_bAbsolute(false)
    , m_Type(COLLIDER2D_TYPE::RECT)
    , m_fRadius(0.f)
{
    m_BoundingSphere = BoundingSphere();
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::finaltick()
{
    // �浹ü�� ���� ��� ���
    m_matColWorld = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
    m_matColWorld *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);

    // �浹ü�� �����ų GameObject �� �������
    const Matrix& matObjWorld = Transform()->GetWorldMat();

    // GameObject �� Scale �� ������ ���� �������� ���
    if (m_bAbsolute)
    {
        Vec3 vWorldScale = Transform()->GetWorldScale();
        Matrix matScaleInv = XMMatrixScaling(vWorldScale.x, vWorldScale.y, vWorldScale.z);
        matScaleInv = XMMatrixInverse(nullptr, matScaleInv);

        // Object �� Scale �� ������� ���̿� ��ġ�ؼ� ��ȿȭ ��Ŵ
        m_matColWorld = m_matColWorld * matScaleInv * matObjWorld;
    }
    else
    {
        m_matColWorld *= matObjWorld;
    }

    // �浹���̸� Red, �浹�ϰ� ���� ������ Green
    if (m_Type == COLLIDER2D_TYPE::RECT)
    {
        if (0 == m_CollisionCount)
            GamePlayStatic::DrawDebugRect(m_matColWorld, Vec3(0.f, 1.f, 0.f), false);
        else
            GamePlayStatic::DrawDebugRect(m_matColWorld, Vec3(1.f, 0.f, 0.f), false);
    }
    else if (m_Type == COLLIDER2D_TYPE::CIRCLE)
    {
        if (0 == m_CollisionCount)
            GamePlayStatic::DrawDebugCircle(m_matColWorld.Translation(), m_fRadius, Vec3(0.f, 1.f, 0.f), false);
        else
            GamePlayStatic::DrawDebugCircle(m_matColWorld.Translation(), m_fRadius, Vec3(1.f, 0.f, 0.f), false);
    }

    // Bounding Sphere
    m_BoundingSphere.Center = m_matColWorld.Translation();
    if (m_Type == COLLIDER2D_TYPE::RECT)
    {
        // ��� ����
        XMVECTOR XMscale;
        XMVECTOR XMrot;
        XMVECTOR XMTr;

        XMMatrixDecompose(&XMscale, &XMrot, &XMTr, m_matColWorld);

        Vec3 scale = XMscale;

        m_BoundingSphere.Radius = (scale.x + scale.y + scale.z) / 3.f;
    }
    else if (m_Type == COLLIDER2D_TYPE::CIRCLE)
    {
        m_BoundingSphere.Radius = m_fRadius;
    }
}

void CCollider2D::BeginOverlap(CCollider2D* _OtherCollider)
{
    ++m_CollisionCount;
}

void CCollider2D::Overlap(CCollider2D* _OtherCollider)
{
}

void CCollider2D::EndOverlap(CCollider2D* _OtherCollider)
{
    --m_CollisionCount;
}