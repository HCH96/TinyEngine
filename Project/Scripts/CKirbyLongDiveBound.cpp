#include "pch.h"
#include "CKirbyLongDiveBound.h"

CKirbyLongDiveBound::CKirbyLongDiveBound()
{
}

CKirbyLongDiveBound::~CKirbyLongDiveBound()
{
}
void CKirbyLongDiveBound::tick()
{
    switch (PLAYERFSM->GetCurAbilityIdx())
    {
    case AbilityCopyType::NORMAL: {
        if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
        {
            ChangeState(L"VACUUM1_START");
        }
        else if (PLAYERCTRL->IsGround() && PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"IDLE");
        }
    }
    break;
    case AbilityCopyType::FIRE: {
        if (KEY_TAP(KEY_ATK) && PLAYERFSM->IsNearDeformObject())
        {
            ChangeState(L"VACUUM1_START");
        }
        else if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
        {
            if (KEY_PRESSED_ARROW && KEY_TAP(KEY_ATK))
            {
                ChangeState(L"BURNING_PRE");
            }
            else
            {
                ChangeState(L"ATTACK_CHARGE1_START");
            }
        }
        else if (PLAYERCTRL->IsGround() && PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"IDLE");
        }
    }
    break;
    case AbilityCopyType::CUTTER: {
        if (KEY_TAP(KEY_ATK) && PLAYERFSM->IsNearDeformObject())
        {
            ChangeState(L"VACUUM1_START");
        }
        else if (KEY_TAP(KEY_ATK))
        {
            if (PLAYERFSM->CanBladeAttack())
            {
                ChangeState(L"ATTACK");
            }
        }
        else if (PLAYERCTRL->IsGround() && PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"IDLE");
        }
    }
    break;
    case AbilityCopyType::SWORD: {
        if (KEY_TAP(KEY_ATK) && PLAYERFSM->IsNearDeformObject())
        {
            ChangeState(L"VACUUM1_START");
        }
        else if (PLAYERFSM->GetSlideComboLevel())
        {
            if (PLAYERCTRL->IsGround())
            {
                ChangeState(L"IDLE");
            }
            else if ((KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK)) && PLAYERFSM->GetSlideComboLevel() == 1)
            {
                ChangeState(L"JUMP_ATTACK_START");
            }
        }
        else if (KEY_TAP(KEY_ATK) || KEY_PRESSED(KEY_ATK))
        {
            if (KEY_TAP(KEY_GUARD) || KEY_PRESSED(KEY_GUARD))
            {
                ChangeState(L"ATTACK_AIRGUARD_CHARGE_START");
            }
            else
            {
                ChangeState(L"JUMP_ATTACK_START");
            }
        }
        else if (PLAYERCTRL->IsGround() && PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"IDLE");
        }
    }
    break;
    case AbilityCopyType::SLEEP: {
        if (PLAYERCTRL->IsGround() && PLAYER->Animator()->IsFinish())
        {
            ChangeState(L"IDLE");
        }
    }
    break;
    }
}

void CKirbyLongDiveBound::Enter()
{
    // Change State
    if (PLAYERFSM->GetCurObjectIdx() != ObjectCopyType::NONE)
    {
    }
    else
    {
        switch (PLAYERFSM->GetCurAbilityIdx())
        {
        case AbilityCopyType::NORMAL:
            break;
        case AbilityCopyType::FIRE:
            break;
        case AbilityCopyType::CUTTER:
            break;
        case AbilityCopyType::SWORD: {
            PLAYERFSM->LockSlideCombo();
        }
        break;
        case AbilityCopyType::SLEEP:
            break;
        }
    }

    // 애니메이션 재생
    PLAYER->Animator()->Play(ANIMPREFIX("LongDiveAttackBound"), false, false, 2.f);

    PLAYERCTRL->ClearVelocityY();
    PLAYERCTRL->AddVelocity(Vec3(0.f, 7.f, 0.f));
}

void CKirbyLongDiveBound::Exit()
{
    // Change State
    if (PLAYERFSM->GetCurObjectIdx() != ObjectCopyType::NONE)
    {
    }
    else
    {
        switch (PLAYERFSM->GetCurAbilityIdx())
        {
        case AbilityCopyType::NORMAL:
            break;
        case AbilityCopyType::FIRE:
            break;
        case AbilityCopyType::CUTTER:
            break;
        case AbilityCopyType::SWORD: {
            PLAYERFSM->UnlockSlideCombo();
        }
        break;
        case AbilityCopyType::SLEEP:
            break;
        }
    }
}
