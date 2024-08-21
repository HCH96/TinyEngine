#include "pch.h"
#include "CUIHPscript.h"

#include <Engine/CTimeMgr.h>
#include "CUnitScript.h"

#include "CPlayerMgr.h"
#include "CKirbyFSM.h"

CUIHPScript::CUIHPScript()
    : CScript(UIHPSCRIPT)
    , m_eState(UnitHPState::Wait)
    , m_TargetName{}
    , m_pRenderer(nullptr)
    , m_pUnitScript(nullptr)
    , m_pNameObj(nullptr)
    , m_fAccTime(0.f)
    , m_fComboTime(2.f)
    , m_bIsCombo(false)
    , m_bIsScaling(false)
    , m_fDescSpeed(10.f)
    , m_vDecreaseColor{}
    , m_vBasicColor{}
    , m_fMaxHP(0.f)
    , m_fCurHP(0.f)
    , m_fPrevHP(0.f)
    , m_fHealingTime(1.f)
    , m_fCurPrevHP(0.f)
    , m_pFSMScript(nullptr)
    , m_bIsEnter(true)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_TargetName, "TargetName");
    AddScriptParam(SCRIPT_PARAM::VEC4, &m_vBasicColor, "Basic Color");
    AddScriptParam(SCRIPT_PARAM::VEC4, &m_vDecreaseColor, "Decrease Color");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fComboTime, "ComboTime");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fDescSpeed, "DecreaseSpeed");
}

CUIHPScript::CUIHPScript(const CUIHPScript& Origin)
    : CScript(Origin)
    , m_TargetName{}
    , m_pRenderer(nullptr)
    , m_pUnitScript(nullptr)
    , m_pNameObj(nullptr)
    , m_fAccTime(0.f)
    , m_fComboTime(Origin.m_fComboTime)
    , m_bIsCombo(false)
    , m_bIsScaling(false)
    , m_fDescSpeed(Origin.m_fDescSpeed)
    , m_vDecreaseColor(Origin.m_vDecreaseColor)
    , m_vBasicColor(Origin.m_vBasicColor)
    , m_fMaxHP(0.f)
    , m_fCurHP(0.f)
    , m_fPrevHP(0.f)
    , m_fHealingTime(1.f)
    , m_fCurPrevHP(0.f)
    , m_pFSMScript(nullptr)
    , m_bIsEnter(true)
{
    AddScriptParam(SCRIPT_PARAM::STRING, &m_TargetName, "TargetName");
    AddScriptParam(SCRIPT_PARAM::VEC4, &m_vBasicColor, "Basic Color");
    AddScriptParam(SCRIPT_PARAM::VEC4, &m_vDecreaseColor, "Decrease Color");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fComboTime, "ComboTime");
    AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fDescSpeed, "DecreaseSpeed");
}

CUIHPScript::~CUIHPScript()
{
}

void CUIHPScript::begin()
{
    // m_fMaxHP = m_fCurHP = m_fPrevH = TargetObject의 MaxHP!
}

void CUIHPScript::SetPlayer()
{
    if (PLAYER)
    {
        m_pUnitScript = PLAYER->GetScript<CUnitScript>();
        m_pFSMScript = PLAYER->GetScript<CKirbyFSM>();
        m_bIsEnter = true;
    }
}

void CUIHPScript::tick()
{
    if (!m_pUnitScript)
        return;

    SetInitInfo();

    switch (m_eState)
    {
    case UnitHPState::Damaged: {
        Damaged();
    }
    break;
    case UnitHPState::Healing: {
        Healing();
    }
    break;
    case UnitHPState::Wait: {
        Wait();
    }
    break;
    case UnitHPState::End:
        break;
    default:
        break;
    }

    SwitchKirbyName();
}

void CUIHPScript::ChangeState(UnitHPState _eState)
{
    ExitState();
    m_eState = _eState;
    EnterState();
}

void CUIHPScript::EnterState()
{
    switch (m_eState)
    {
    case UnitHPState::Damaged:
        break;
    case UnitHPState::Healing: {
        if (nullptr != m_pUnitScript)
            m_fPrevHP = m_pUnitScript->GetCurInfo().HP;
    }
    break;
    case UnitHPState::Wait:
        break;
    case UnitHPState::End:
        break;
    default:
        break;
    }
}

void CUIHPScript::ExitState()
{
    switch (m_eState)
    {
    case UnitHPState::Damaged: {
        m_fAccTime = 0.f;
    }
    break;
    case UnitHPState::Healing: {
        m_fAccTime = 0.f;
    }
    break;
    case UnitHPState::Wait: {
        m_fAccTime = 0.f;
    }
    break;
    case UnitHPState::End:
        break;
    default:
        break;
    }
}

void CUIHPScript::Damaged()
{
    if (!m_pUnitScript)
        return;

    float fPrevCurhp = m_fCurHP;
    m_fCurHP = m_pUnitScript->GetCurInfo().HP;

    if (m_bIsCombo)
    {
        CaculateShading();
    }

    bool _IsCombo = IsCombo();

    if (!_IsCombo && m_bIsCombo)
    {
        m_bIsScaling = true;
    }

    if (m_bIsScaling)
        Scaling();

    if (fPrevCurhp < m_fCurHP)
    {
        m_fCurPrevHP = fPrevCurhp;
        ChangeState(UnitHPState::Healing);
    }
}

void CUIHPScript::Healing()
{
    m_fAccTime += DT;
    Scaling();

    if (m_fAccTime >= m_fHealingTime)
        CaculateHealingShading();

    float fCheckHP = m_fCurHP;
    m_fCurHP = m_pUnitScript->GetCurInfo().HP;
    if (m_fCurHP < fCheckHP)
    {
        ChangeState(UnitHPState::Damaged);
    }
}

// 현재 체력 보존
void CUIHPScript::Wait()
{
    if (nullptr == m_pUnitScript)
        return;

    // 현재 체력 계산 후 쉐이딩
    {
        m_pRenderer = GetOwner()->GetComponent<CMeshRender>();
        m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_0, m_fCurHP / m_fMaxHP);
        m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_1, m_fPrevHP / m_fMaxHP);
        m_pRenderer->GetMaterial(0)->SetScalarParam(VEC4_0, m_vBasicColor);
        m_pRenderer->GetMaterial(0)->SetScalarParam(VEC4_1, m_vDecreaseColor);
    }

    float fPrevCurhp = m_fCurHP;
    m_fCurHP = m_pUnitScript->GetCurInfo().HP;
    if (fPrevCurhp < m_fCurHP)
    {
        ChangeState(UnitHPState::Healing);
    }
    else if (m_fCurHP < fPrevCurhp)
    {
        ChangeState(UnitHPState::Damaged);
    }
}

void CUIHPScript::SetInitInfo()
{
    if (!m_bIsEnter)
        return;

    m_pNameObj = GetOwner()->GetChildObject(L"UI_PlayerName");

    if (!m_pUnitScript)
        return;

    m_pRenderer = GetOwner()->GetComponent<CMeshRender>();
    m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_0, m_fCurHP / m_fMaxHP);
    m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_1, m_fPrevHP / m_fMaxHP);
    m_pRenderer->GetMaterial(0)->SetScalarParam(VEC4_0, m_vBasicColor);
    m_pRenderer->GetMaterial(0)->SetScalarParam(VEC4_1, m_vDecreaseColor);

    m_fMaxHP = PLAYER->GetScript<CUnitScript>()->GetCurInfo().MAXHP;
    m_fPrevHP = m_fCurHP = PLAYER->GetScript<CUnitScript>()->GetCurInfo().HP;

    m_bIsEnter = false;
}

void CUIHPScript::CaculateShading()
{
    float _fShadingRatio = m_fCurHP / m_fMaxHP;
    m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_0, _fShadingRatio);
}

void CUIHPScript::CaculateHealingShading()
{
    m_fCurPrevHP += m_fDescSpeed * DT;
    float _fShadingRatio = m_fCurPrevHP / m_fMaxHP;
    m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_0, _fShadingRatio);
}

void CUIHPScript::Scaling()
{
    m_bIsCombo = false;

    m_fPrevHP -= CTimeMgr::GetInst()->GetDeltaTime() * m_fDescSpeed;

    if (m_fCurHP >= m_fPrevHP)
    {
        m_fPrevHP = m_fCurHP;
    }

    float _fScalingRatio = m_fPrevHP / m_fMaxHP;
    m_pRenderer->GetMaterial(0)->SetScalarParam(FLOAT_1, _fScalingRatio);

    if (m_fCurHP >= m_fPrevHP)
    {
        m_bIsScaling = false;
    }
}

void CUIHPScript::SwitchKirbyName()
{
    if (nullptr == m_pFSMScript)
        return;

    AbilityCopyType eAbility = m_pFSMScript->GetCurAbilityIdx();
    ObjectCopyType eObject = m_pFSMScript->GetCurObjectIdx();

    int AbilityIdx = -1;
    int ObjectIdx = -1;

    switch (eAbility)
    {
    case AbilityCopyType::NORMAL: {
        AbilityIdx = 3;
    }
    break;
    case AbilityCopyType::FIRE: {
        AbilityIdx = 6;
    }
    break;
    case AbilityCopyType::CUTTER: {
        AbilityIdx = 5;
    }
    break;
    case AbilityCopyType::SWORD: {
        AbilityIdx = 4;
    }
    break;
    case AbilityCopyType::SLEEP: {
        AbilityIdx = 7;
    }
    break;
    case AbilityCopyType::END:
        break;
    default:
        break;
    }

    switch (eObject)
    {
    case ObjectCopyType::NONE: {
    }
    break;
    case ObjectCopyType::CONE: {
        ObjectIdx = 0;
    }
    break;
    case ObjectCopyType::VENDING_MACHINE: {
        ObjectIdx = 1;
    }
    break;
    case ObjectCopyType::LIGHT: {
        ObjectIdx = 2;
    }
    break;
    case ObjectCopyType::END:
        break;
    default:
        break;
    }

    if (-1 == ObjectIdx)
    {
        for (int i = 3; i < 8; i++)
        {
            if (i == AbilityIdx)
            {
                if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                    GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(true);
            }
            else
            {
                if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                    GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(false);
            }
        }

        for (int i = 0; i < 3; i++)
        {
            if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(false);
        }
    }
    else
    {
        for (int i = 3; i < 8; i++)
        {
            if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(false);
        }

        for (int i = 0; i < 3; i++)
        {
            if (i == ObjectIdx)
            {
                if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                    GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(true);
            }
            else
            {
                if (GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i)))
                    GetOwner()->GetChildObject(L"UI_PlayerName" + std::to_wstring(i))->SetActive(false);
            }
        }
    }
}

bool CUIHPScript::IsCombo()
{
    bool _IsCombo = m_pUnitScript->IsGetDamage();

    if (_IsCombo)
    {
        if (!m_bIsCombo)
        {
            m_bIsCombo = true;
            m_bIsScaling = false;
        }
    }

    _IsCombo == true ? m_fAccTime = 0.f : m_fAccTime += CTimeMgr::GetInst()->GetDeltaTime();

    if (m_fAccTime >= m_fComboTime)
    {
        _IsCombo = false;
    }
    else
    {
        _IsCombo = true;
    }

    return _IsCombo;
}

UINT CUIHPScript::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    MemoryByte += SaveWStringToFile(ToWstring(m_TargetName), _File);
    fwrite(&m_vBasicColor, sizeof(Vec4), 1, _File);
    fwrite(&m_vDecreaseColor, sizeof(Vec4), 1, _File);
    fwrite(&m_fComboTime, sizeof(float), 1, _File);
    fwrite(&m_fDescSpeed, sizeof(float), 1, _File);

    MemoryByte += sizeof(Vec4);
    MemoryByte += sizeof(Vec4);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);

    return MemoryByte;
}

UINT CUIHPScript::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    wstring _TargetName = {};
    MemoryByte += LoadWStringFromFile(_TargetName, _File);
    fread(&m_vBasicColor, sizeof(Vec4), 1, _File);
    fread(&m_vDecreaseColor, sizeof(Vec4), 1, _File);
    fread(&m_fComboTime, sizeof(float), 1, _File);
    fread(&m_fDescSpeed, sizeof(float), 1, _File);

    m_TargetName = ToString(_TargetName);

    MemoryByte += sizeof(Vec4);
    MemoryByte += sizeof(Vec4);
    MemoryByte += sizeof(float);
    MemoryByte += sizeof(float);

    return MemoryByte;
}