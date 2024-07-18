#include "pch.h"
#include "CHotHeadScript.h"

#include "CFireProjectileScript.h"

CHotHeadScript::CHotHeadScript()
    : CMonsterUnitScript(HOTHEADSCRIPT)
    , m_pFlameRotObject(nullptr)
    , m_pFlameCol(nullptr)
    , m_pFlameRotCol(nullptr)
    , m_eState(HOTHEAD_STATE::Idle)
    , m_vDamageDir{}
    , m_fAimingTime(0.f)
    , m_fAccTime(0.f)
    , m_fShotLength(0.f)
    , m_fFlameLength(0.f)
    , m_fFlameRotLength(0.f)
    , m_fRotRadian(0.f)
    , m_bFlag(false)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fAimingTime, "Aiming Time");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fAccTime, "AccTime");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fShotLength, "ShotLength");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFlameLength, "FlameLength");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFlameRotLength, "FlameRotLength");
}

CHotHeadScript::CHotHeadScript(const CHotHeadScript& _Origin)
    : CMonsterUnitScript(_Origin)
    , m_pFlameRotObject(nullptr)
    , m_pFlameCol(nullptr)
    , m_pFlameRotCol(nullptr)
    , m_eState(HOTHEAD_STATE::Idle)
    , m_vDamageDir{}
    , m_fAimingTime(_Origin.m_fAccTime)
    , m_fAccTime(0.f)
    , m_fShotLength(_Origin.m_fShotLength)
    , m_fFlameLength(_Origin.m_fFlameLength)
    , m_fFlameRotLength(_Origin.m_fFlameRotLength)
    , m_fRotRadian(_Origin.m_fRotRadian)
    , m_bFlag(false)
{
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fAimingTime, "Aiming Time");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fAccTime, "Aiming Time");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fShotLength, "ShotLength");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFlameLength, "FlameLength");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFlameRotLength, "FlameRotLength");
}

CHotHeadScript::~CHotHeadScript()
{
}

void CHotHeadScript::begin()
{
    m_pFlameRotObject = GetOwner()->GetChildObject(L"FlameRot Object");

    if (nullptr == m_pFlameRotObject)
    {
        MessageBox(nullptr, L"FlameRot Object Does Not Exist", L"FlameRot Object Issue", MB_OK);
        // TODO : 위치 정해지면 코드로 생성해주기
    }

    m_pFlameCol = GetOwner()->GetChildObject(L"Flame Collider")->CapsuleCollider();
    m_pFlameRotCol = m_pFlameRotObject->GetChildObject(L"FlameRot Collider")->SphereCollider();

    if (nullptr == m_pFlameCol)
    {
        MessageBox(nullptr, L"Flame Collider Does Not Exist", L"Flame Collider Issue", MB_OK);
        // TODO : 위치 정해지면 코드로 생성해주기
    }

    if (nullptr == m_pFlameRotCol)
    {
        MessageBox(nullptr, L"FlameRot Collider Does Not Exist", L"FlameRot Collider Issue", MB_OK);
        // TODO : 위치 정해지면 코드로 생성해주기
    }

    m_pFlameCol->SetEnabled(false);
    m_pFlameRotCol->SetEnabled(false);

    ChangeState(HOTHEAD_STATE::Idle);
}

void CHotHeadScript::tick()
{
    switch (m_eState)
    {
    case HOTHEAD_STATE::Idle: {
        Idle();
    }
    break;
    case HOTHEAD_STATE::Find: {
        Find();
    }
    break;
    case HOTHEAD_STATE::Aiming: {
        Aiming();
    }
    break;
    case HOTHEAD_STATE::AttackShootStart: {
        AttackShootStart();
    }
    break;
    case HOTHEAD_STATE::AttackShoot: {
        AttackShoot();
    }
    break;

    case HOTHEAD_STATE::AttackFlameStart: {
        AttackFlameStart();
    }
    break;
    case HOTHEAD_STATE::AttackFlame: {
        AttackFlame();
    }
    break;
    case HOTHEAD_STATE::AttackFlameRotStart: {
        AttackFlameRotStart();
    }
    break;
    case HOTHEAD_STATE::AttackFlameRot: {
        AttackFlameRot();
    }
    break;
    case HOTHEAD_STATE::AttackShootEnd:
    case HOTHEAD_STATE::AttackFlameEnd:
    case HOTHEAD_STATE::AttackFlameRotEnd: {
        AttackEnd();
    }
    break;
    case HOTHEAD_STATE::Damage: {
        Damage();
    }
    break;
    case HOTHEAD_STATE::Eaten: {
        Eaten();
    }
    break;
    case HOTHEAD_STATE::Death: {
        Death();
    }
    break;
    case HOTHEAD_STATE::End:
        break;
    default:
        break;
    }
}

void CHotHeadScript::EnterState(HOTHEAD_STATE _state)
{
    switch (m_eState)
    {
    case HOTHEAD_STATE::Idle: {
        Animator()->Play(ANIMPREFIX(L"Wait"));
    }
    break;
    case HOTHEAD_STATE::Find: {
        Animator()->Play(ANIMPREFIX(L"Find"), false);
    }
    break;
    case HOTHEAD_STATE::Aiming: {
        Animator()->Play(ANIMPREFIX(L"Walk"), true, false, 1.5f);
    }
    break;
    case HOTHEAD_STATE::AttackShootStart: {
        Animator()->Play(ANIMPREFIX(L"AttackShootStart"), false);
    }
    break;
    case HOTHEAD_STATE::AttackShoot: {
        ProjectileAttack();
        Animator()->Play(ANIMPREFIX(L"AttackShoot"), false);
    }
    break;
    case HOTHEAD_STATE::AttackShootEnd: {
        Animator()->Play(ANIMPREFIX(L"AttackShootEnd"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlameStart: {
        Animator()->Play(ANIMPREFIX(L"AttackFlameStart"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlame: {
        // TODO : 앞에 Attack Area 키기
        m_pFlameCol->SetEnabled(true);
        Animator()->Play(ANIMPREFIX(L"AttackFlame"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlameEnd: {
        // TODO : 앞에 Attack Area 끄기
        m_pFlameCol->SetEnabled(false);
        Animator()->Play(ANIMPREFIX(L"AttackFlameEnd"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlameRotStart: {
        Animator()->Play(ANIMPREFIX(L"AttackFlameRotStart"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlameRot: {
        // 현재 Radian 가져오기
        m_fRotRadian = m_pFlameRotObject->Transform()->GetLocalRotation().y;
        // TODO : 앞에 Attack Area 키기
        m_pFlameRotCol->SetEnabled(true);
        Animator()->Play(ANIMPREFIX(L"AttackFlameRot"), false);
    }
    break;
    case HOTHEAD_STATE::AttackFlameRotEnd: {
        // TODO : 앞에 Attack Area 끄기
        m_pFlameRotCol->SetEnabled(false);
        Animator()->Play(ANIMPREFIX(L"AttackFlameRotEnd"), false);
    }
    break;
    case HOTHEAD_STATE::Damage: {
        Animator()->Play(ANIMPREFIX(L"Damage"), false);
    }
    break;
    case HOTHEAD_STATE::Eaten: {
        Animator()->Play(ANIMPREFIX(L"Damage"));
    }
    break;
    case HOTHEAD_STATE::Death: {
        Animator()->Play(ANIMPREFIX(L"Damage"));
    }
    break;
    case HOTHEAD_STATE::End:
        break;
    default:
        break;
    }
}

void CHotHeadScript::ChangeState(HOTHEAD_STATE _state)
{
    ExitState(m_eState);
    m_eState = _state;
    EnterState(m_eState);
}

void CHotHeadScript::ExitState(HOTHEAD_STATE _state)
{
    switch (m_eState)
    {
    case HOTHEAD_STATE::Idle: {
        m_fAccTime = 0.f;
    }
    break;
    case HOTHEAD_STATE::Find:
        break;
    case HOTHEAD_STATE::Aiming: {
        m_fAccTime = 0.f;
    }
    break;
    case HOTHEAD_STATE::AttackShootStart:
        break;
    case HOTHEAD_STATE::AttackShoot:
        break;
    case HOTHEAD_STATE::AttackShootEnd:
        break;
    case HOTHEAD_STATE::AttackFlameStart:
        break;
    case HOTHEAD_STATE::AttackFlame: {
        m_fAccTime = 0.f;
    }
    break;
    case HOTHEAD_STATE::AttackFlameEnd:
        break;
    case HOTHEAD_STATE::AttackFlameRotStart:
        break;
    case HOTHEAD_STATE::AttackFlameRot: {
        // 원래 Rotation으로 초기화
        m_fRotRadian = 0.f;
        m_pFlameRotObject->Transform()->SetWorldRotation(GetOwner()->Transform()->GetWorldQuaternion());
    }
    break;
    case HOTHEAD_STATE::AttackFlameRotEnd:
        break;
    case HOTHEAD_STATE::Damage:
    case HOTHEAD_STATE::Eaten: {
        m_bFlag = false;
    }
    break;
    case HOTHEAD_STATE::Death:
        break;
    case HOTHEAD_STATE::End:
        break;
    default:
        break;
    }
}

void CHotHeadScript::Idle()
{
    m_fAccTime += DT;
    if (nullptr != GetTarget() && m_fAccTime >= 2.f)
    {
        ChangeState(HOTHEAD_STATE::Find);
    }
}

void CHotHeadScript::Find()
{
    if (Animator()->IsFinish())
    {
        if (nullptr != GetTarget())
        {
            ChangeState(HOTHEAD_STATE::Aiming);
        }
        else
        {
            ChangeState(HOTHEAD_STATE::Idle);
        }
    }
}

void CHotHeadScript::Aiming()
{
    if (nullptr != GetTarget())
    {
        TransformRotate();
        m_fAccTime += DT;
        if (m_fAccTime >= m_fAimingTime)
        {
            float fDistance = (GetTarget()->GetComponent<CTransform>()->GetLocalPos() - Transform()->GetLocalPos()).Length();
            HOTHEAD_STATE state = {};

            if (fDistance <= m_fShotLength)
            {
                state = HOTHEAD_STATE::AttackShootStart;
            }

            if (fDistance <= m_fFlameLength)
            {
                state = HOTHEAD_STATE::AttackFlameStart;
            }

            if (fDistance <= m_fFlameRotLength)
            {
                state = HOTHEAD_STATE::AttackFlameRotStart;
            }

            ChangeState(state);
        }
    }
    else
    {
        ChangeState(HOTHEAD_STATE::Idle);
    }
}

void CHotHeadScript::AttackShootStart()
{
    if (Animator()->IsFinish())
    {
        ChangeState(HOTHEAD_STATE::AttackShoot);
    }
}

void CHotHeadScript::AttackShoot()
{
    if (Animator()->IsFinish())
    {
        ChangeState(HOTHEAD_STATE::AttackShootEnd);
    }
}

void CHotHeadScript::AttackFlameStart()
{
    if (Animator()->IsFinish())
    {
        ChangeState(HOTHEAD_STATE::AttackFlame);
    }
}

void CHotHeadScript::AttackFlame()
{
    m_fAccTime += DT;
    if (Animator()->IsFinish() && m_fAccTime >= 3.f)
    {
        ChangeState(HOTHEAD_STATE::AttackFlameEnd);
    }
}

void CHotHeadScript::AttackFlameRotStart()
{
    if (Animator()->IsFinish())
    {
        ChangeState(HOTHEAD_STATE::AttackFlameRot);
    }
}

void CHotHeadScript::AttackFlameRot()
{
    m_fRotRadian += DT * 5.f;
    Quat Quaternion = Quat::CreateFromAxisAngle(Vec3(0.f, 1.f, 0.f), m_fRotRadian);
    m_pFlameRotObject->Transform()->SetWorldRotation(Quaternion);

    if (Animator()->IsFinish())
    {
        ChangeState(HOTHEAD_STATE::AttackFlameRotEnd);
    }
}

void CHotHeadScript::AttackEnd()
{
    Animator()->IsFinish() ? ChangeState(HOTHEAD_STATE::Idle) : void();
}

void CHotHeadScript::Damage()
{
    if (!m_bFlag)
    {
        Rigidbody()->SetVelocity(Vec3(0.f, 0.f, 0.f));

        m_vDamageDir.Normalize();
        m_vDamageDir.y = 1.5f;
        Rigidbody()->AddForce(m_vDamageDir * 5.f, ForceMode::Impulse);
        m_bFlag = true;
    }

    // 아마 Damage 받을 때 사망 띠~
    if (GetCurInfo().HP <= 0.f)
    {
        ChangeState(HOTHEAD_STATE::Death);
    }

    Animator()->IsFinish() ? ChangeState(HOTHEAD_STATE::Idle) : void();
}

void CHotHeadScript::Eaten()
{
    if (!m_bFlag)
    {
        m_vDamageDir.Normalize();
        Vec3 vUp = Vec3(0.f, 0.f, -1.f) == m_vDamageDir ? Vec3(0.f, -1.f, 0.f) : Vec3(0.f, 1.f, 0.f);
        Quat vQuat = Quat::LookRotation(-m_vDamageDir, vUp);
        Transform()->SetWorldRotation(vQuat);
        m_bFlag = true;
    }
}

void CHotHeadScript::Death()
{
    Animator()->IsFinish() ? GamePlayStatic::DestroyGameObject(GetOwner()) : void();
}

void CHotHeadScript::ProjectileAttack()
{
    Ptr<CPrefab> bulletPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\HotHeadBullet.pref", L"prefab\\HotHeadBullet.pref");
    if (nullptr != bulletPref)
    {
        CGameObject* pBullet = bulletPref->Instantiate();
        pBullet->Transform()->SetLocalPos(Transform()->GetLocalPos() + Transform()->GetWorldDir(DIR_TYPE::FRONT) * 150.f + Vec3(0.f, 84.f, 0.f));
        pBullet->Transform()->SetWorldRotation(Transform()->GetWorldQuaternion());
        // TODO : LAYER_MONSTER -> LAYER_MOSTERATK 변경
        GamePlayStatic::SpawnGameObject(pBullet, LAYER_MONSTER);
    }
}

void CHotHeadScript::SaveToLevelFile(FILE* _File)
{
    CMonsterUnitScript::SaveToLevelFile(_File);
    fwrite(&m_fAimingTime, sizeof(float), 1, _File);
    fwrite(&m_fShotLength, sizeof(float), 1, _File);
    fwrite(&m_fFlameLength, sizeof(float), 1, _File);
    fwrite(&m_fFlameRotLength, sizeof(float), 1, _File);
}

void CHotHeadScript::LoadFromLevelFile(FILE* _File)
{
    CMonsterUnitScript::LoadFromLevelFile(_File);
    fread(&m_fAimingTime, sizeof(float), 1, _File);
    fread(&m_fShotLength, sizeof(float), 1, _File);
    fread(&m_fFlameLength, sizeof(float), 1, _File);
    fread(&m_fFlameRotLength, sizeof(float), 1, _File);
}

void CHotHeadScript::OnTriggerEnter(CCollider* _OtherCollider)
{
    if (HOTHEAD_STATE::Eaten == m_eState)
        return;

    CGameObject* pObj = _OtherCollider->GetOwner();
    bool flag = false;

    UnitHit hit;
    ZeroMemory(&hit, sizeof(hit));
    /**********************
    | 1. Player ATK Hit
    ***********************/

    // 충돌한 오브젝트가 플레이어 공격인지 확인
    if (LAYER_PLAYERATK == pObj->GetLayerIdx())
    {
        flag = true;
        // TODO : 플레이어 공격 데미지 가지고 오기

        GetDamage(hit);
        ChangeState(HOTHEAD_STATE::Damage);
        m_vDamageDir = pObj->GetParent()->GetComponent<CTransform>()->GetWorldDir(DIR_TYPE::FRONT);
    }

    /**********************
    | 2. Player Body Hit
    ***********************/
    // 충돌한 오브젝트가 PlayerBody인지 확인

    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        // 충돌한 오브젝트 Vaccum 이라면 Collider가 켜진 상태임 즉, 빨아들이고 있는 상태
        if (L"Vacuum Collider" == pObj->GetName())
        {
            ChangeState(HOTHEAD_STATE::Eaten);
            m_vDamageDir = -pObj->GetComponent<CTransform>()->GetWorldDir(DIR_TYPE::FRONT);
            return;
        }

        if (L"Body Collider" == pObj->GetName())
            pObj->GetParent()->GetScript<CUnitScript>()->GetDamage(GetHitInfo());

        flag = true;
    }

    // 둘 중 하나라도 피격 되었다면 체력 확인
    if (flag)
    {
        if (GetCurInfo().HP - hit.Damage <= 0.f)
        {
            ChangeState(HOTHEAD_STATE::Death);
        }
    }
}

void CHotHeadScript::OnTriggerExit(CCollider* _OtherCollider)
{
    CGameObject* pObj = _OtherCollider->GetOwner();
    if (LAYER_PLAYER == pObj->GetLayerIdx())
    {
        // 충돌한 오브젝트 Vaccum 이라면 Collider가 켜진 상태임 즉, 빨아들이는게 끝난 상태
        if (L"Vacuum Collider" == pObj->GetName())
        {
            ChangeState(HOTHEAD_STATE::Idle);
        }
    }
}