#include "pch.h"
#include "CKirbyBodyCollider.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"
#include "CKirbyVacuumCollider.h"
#include "CKirbyUnitScript.h"

CKirbyBodyCollider::CKirbyBodyCollider()
    : CScript(KIRBYBODYCOLLIDER)
{
}

CKirbyBodyCollider::~CKirbyBodyCollider()
{
}

void CKirbyBodyCollider::OnTriggerEnter(CCollider* _OtherCollider)
{
    int LayerIdx = _OtherCollider->GetOwner()->GetLayerIdx();

    // 흡입을 시작한 상태(타겟을 결정해 빨아들이기 시작한 상태)에서 충돌한 경우
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
        return;
    }

    // monster : 데미지 가함
    if (LayerIdx == LAYER_MONSTER)
    {
        Vec3 HitDir = (_OtherCollider->Transform()->GetWorldPos() - Transform()->GetWorldPos()).Normalize();
        float HitDamage = FindDamage();
        UnitHit HitInfo = {DAMAGE_TYPE::NORMAL, HitDir, HitDamage, 0.f, 0.f};
        CUnitScript* pMonster = _OtherCollider->GetOwner()->GetScript<CUnitScript>();

        if (!pMonster)
            return;
        pMonster->GetDamage(HitInfo);
        ((CUnitScript*)PLAYERUNIT)->AttackReward();
    }
}

void CKirbyBodyCollider::OnTriggerStay(CCollider* _OtherCollider)
{
    if (PLAYERFSM->IsDrawing())
    {
        PLAYERFSM->DrawingCollisionEnter(_OtherCollider->GetOwner());
        return;
    }
}

float CKirbyBodyCollider::FindDamage()
{
    float Damage = 5.f;

    if (PLAYERFSM->GetCurObject())
    {
        switch (PLAYERFSM->GetCurObjectIdx())
        {
        case ObjectCopyType::CONE:
            Damage = 100.f;
            break;
        }
    }

    return Damage;
}

UINT CKirbyBodyCollider::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}

UINT CKirbyBodyCollider::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    return MemoryByte;
}
