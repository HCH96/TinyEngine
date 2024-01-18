#include "pch.h"
#include "CShockWaveScript.h"

CShockWaveScript::CShockWaveScript()
    : CScript(SCRIPT_TYPE::SHOCKWAVESCRIPT)
    , m_Time(0.f)
{
}

CShockWaveScript::~CShockWaveScript()
{
}

void CShockWaveScript::begin()
{
}

void CShockWaveScript::tick()
{
    float Radius = Collider2D()->GetRadius();
    Radius += DT * 3000.f * 2.f;

    if (Radius >= 3000.f)
        GamePlayStatic::DestroyGameObject(GetOwner());

    Collider2D()->SetRadius(Radius);

    float CircleSize = Radius / 10000.f;

    MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &CircleSize);
}

void CShockWaveScript::BeginOverlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider)
{
}

void CShockWaveScript::Overlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider)
{
}

void CShockWaveScript::EndOverlap(CCollider2D* _Collider, CGameObject* _OtherObj, CCollider2D* _OtherCollider)
{
}