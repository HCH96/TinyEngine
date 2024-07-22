#include "pch.h"
#include "CKirbyBulletScript.h"
#include "CPlayerMgr.h"
#include "CKirbyFSM.h"

CKirbyBulletScript::CKirbyBulletScript()
    : CScript(KIRBYBULLETSCRIPT)
    , m_PlayTime(5.f)
    , m_Speed(1.f)
    , m_bHasTickCol(false)
    , m_bInit(true)
{
}

CKirbyBulletScript::~CKirbyBulletScript()
{
}

void CKirbyBulletScript::begin()
{
    if (!Rigidbody())
    {
        GetOwner()->AddComponent(new CRigidbody());
    }

    Rigidbody()->SetUseGravity(false);

    // ===========================
    // Create Child
    // ===========================
    if (!PLAYERFSM->GetStuffedObj())
        return;

    CGameObject* pStuffedObj = new CGameObject();
    CTransform* pTransform = PLAYERFSM->GetStuffedObj()->Transform()->Clone();

    // set offset
    if (PLAYERFSM->GetStuffedObj()->SphereCollider())
    {
        m_ChildOffest = PLAYERFSM->GetStuffedObj()->SphereCollider()->GetCenter();
    }
    else if (PLAYERFSM->GetStuffedObj()->CapsuleCollider())
    {
        m_ChildOffest = PLAYERFSM->GetStuffedObj()->CapsuleCollider()->GetCenter();
    }
    else if (PLAYERFSM->GetStuffedObj()->BoxCollider())
    {
        m_ChildOffest = PLAYERFSM->GetStuffedObj()->BoxCollider()->GetCenter();
    }
    else if (PLAYERFSM->GetStuffedObj()->MeshCollider())
    {
        m_ChildOffest = PLAYERFSM->GetStuffedObj()->MeshCollider()->GetCenter();
    }

    // add component
    pStuffedObj->AddComponent(pTransform);
    pTransform->SetAbsolute(true);
    pTransform->SetLocalPos(-m_ChildOffest);
    pTransform->SetLocalRotation(Vec3());

    if (PLAYERFSM->GetStuffedObj()->MeshRender())
    {
        pStuffedObj->AddComponent(PLAYERFSM->GetStuffedObj()->MeshRender()->Clone());
    }

    if (PLAYERFSM->GetStuffedObj()->Animator() && PLAYERFSM->GetStuffedObj()->Animator()->FindClipIndex(L"Damage") != -1)
    {
        CAnimator* pAnimator = PLAYERFSM->GetStuffedObj()->Animator()->Clone();
        pAnimator->Play(L"Damage", true, false, 1.f);
        pStuffedObj->AddComponent(pAnimator);
    }
    else
    {
        pTransform->SetLocalRotation(Vec3(XMConvertToRadians(-90.f), XMConvertToRadians(180.f), 0.f));
    }

    GamePlayStatic::AddChildObject(GetOwner(), pStuffedObj);
    GamePlayStatic::LayerChange(pStuffedObj, LAYER_PLAYERATK);
}

void CKirbyBulletScript::tick()
{
    // init child pos
    if (m_bInit && !GetOwner()->GetChildObject().empty())
    {
        GetOwner()->GetChildObject()[0]->Transform()->SetLocalPos(-m_ChildOffest);
        m_bInit = false;
    }

    m_bHasTickCol = false;
    m_PlayTime -= DT;
    Rigidbody()->SetVelocity(m_MoveDir * m_Speed);
    Vec3 AngularVelocity = Transform()->GetWorldDir(DIR_TYPE::FRONT);
    Rigidbody()->SetAngularVelocity(AngularVelocity);

    if (m_PlayTime <= 0.f)
    {
        GamePlayStatic::DestroyGameObject(GetOwner());
    }
}

void CKirbyBulletScript::SetInitVelocity(Vec3 _Vel)
{
    m_Speed = _Vel.Length();

    _Vel.y = 0.f;
    _Vel = _Vel.Normalize();
    m_MoveDir = _Vel;
}

void CKirbyBulletScript::OnCollisionEnter(CCollider* _OtherCollider)
{
    if (m_bHasTickCol)
        return;

    Vec3 ColRadius = SphereCollider()->GetRadius() * Transform()->GetWorldScale();
    Vec3 RayStart = Transform()->GetWorldPos() + m_MoveDir *  ColRadius;
    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(RayStart, m_MoveDir, ColRadius.Length() + 10.f, {L"World Static"});

    if (Hit.pCollisionObj)
    {
        m_bHasTickCol = true;
        Vec3 Normal = Hit.Normal;
        m_MoveDir = m_MoveDir + 2.f * Normal * (-m_MoveDir.Dot(Normal));
        m_MoveDir.Normalize();

        // 예외처리 Dir 이 Vec3(0.f, 0.f, -1.f)인경우 Up벡터가 반전됨
        Vec3 up = Vec3(0.f, 1.f, 0.f);
        if (m_MoveDir == Vec3(0.f, 0.f, -1.f))
        {
            up = Vec3(0.f, -1.f, 0.f);
        }

        Quat ToWardQuaternion = Quat::LookRotation(-m_MoveDir, up);
        Transform()->SetWorldRotation(ToWardQuaternion);
    }
 }
