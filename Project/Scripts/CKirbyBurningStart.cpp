#include "pch.h"
#include "CKirbyBurningStart.h"

CKirbyBurningStart::CKirbyBurningStart()
    : m_SaveRotSpeed(0.f)
    , m_SaveSpeed(0.f)
{
}

CKirbyBurningStart::~CKirbyBurningStart()
{
}


void CKirbyBurningStart::tick()
{
    // 전체 글라이딩 시간을 위해 FSM에 Gliding 시간을 더해준다.
    PLAYERFSM->AddGlidingTime(DT);

    // 중력값 조절
    float CurGravity = PLAYERCTRL->GetGravity();
    if (CurGravity < 0)
    {
        CurGravity += 100.f * DT;
        PLAYERCTRL->SetGravity(CurGravity);
    }


    if (PLAYER->GetChildObject(L"KirbyDragon")->Animator()->IsFinish())
    {
        ChangeState(L"BURNING");
    }

    Vec3 Raystart = GetOwner()->Transform()->GetWorldPos();
    Raystart.y += 20.f;

    Vec3 RayDir = GetOwner()->Transform()->GetWorldDir(DIR_TYPE::FRONT);

    float RayLength = GetOwner()->CharacterController()->GetRadius() * 20.f + 5.f;

    static vector<wstring> FireWallCollision{L"World Static"};
    RaycastHit Hit = CPhysicsMgr::GetInst()->RayCast(Raystart, RayDir, RayLength, FireWallCollision);

    if (Hit.pCollisionObj != nullptr)
    {
        Vec3 Normal = Hit.Normal;
        Normal.y = 0.f;

        if (Normal.Length() == 0.f)
            return;

        Normal.Normalize();

        // 벽과 날아가는 각도가 45도 이하일 경우 WallEnd상태로 이동
        if (Normal.Dot(-RayDir) >= cosf(XM_PI / 4.f))
        {
            ChangeState(L"BURNING_WALL_END");
        }
    }
}

void CKirbyBurningStart::Enter()
{

    CGameObject* Wing = PLAYER->GetChildObject(L"KirbyDragon");

    if (Wing != nullptr)
    {
        Wing->SetActive(true);
    }

    Wing->Animator()->Play(ANIMPREFIX("BurningStart"), false, false, 1.5f);

    // 애니메이션 재생
    PLAYER->Animator()->Play(ANIMPREFIX("Burning"), true, false, 1.5f);

    // 글라이딩 시간 초기화
    PLAYERFSM->ClearGlidingTime();

    // Movement
    PLAYERCTRL->LockJump();
    PLAYERCTRL->SetForwardMode(true);
    m_SaveRotSpeed = PLAYERCTRL->GetRotSpeed();
    PLAYERCTRL->SetRotSpeed(0.5f);

    m_SaveSpeed = PLAYERCTRL->GetSpeed();
    PLAYERCTRL->SetSpeed(13.f);

    PLAYERCTRL->ClearVelocityY();

    //  무적 상태
    PLAYERFSM->SetInvincible(true);
}

void CKirbyBurningStart::Exit()
{
    CGameObject* Wing = PLAYER->GetChildObject(L"KirbyDragon");

    if (Wing != nullptr)
    {
        Wing->SetActive(false);
    }

    PLAYERCTRL->UnlockJump();
    PLAYERCTRL->SetForwardMode(false);
    PLAYERCTRL->SetRotSpeed(m_SaveRotSpeed);
    PLAYERCTRL->SetSpeed(m_SaveSpeed);

    //  무적 상태
    PLAYERFSM->SetInvincible(false);
}
