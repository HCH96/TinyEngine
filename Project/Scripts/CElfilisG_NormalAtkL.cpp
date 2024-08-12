#include "pch.h"
#include "CElfilisG_NormalAtkL.h"
#include "CElfilisFSM.h"

CElfilisG_NormalAtkL::CElfilisG_NormalAtkL()
    : m_bComboSuccess(false)
{
}

CElfilisG_NormalAtkL::~CElfilisG_NormalAtkL()
{
}

void CElfilisG_NormalAtkL::tick()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        Start();
    }
    break;
    case StateStep::Progress: {
        Progress();
    }
    break;
    case StateStep::End: {
        End();
    }
    break;
    }
}

void CElfilisG_NormalAtkL::Enter_Step()
{
    switch (m_Step)
    {
    case StateStep::Start: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeftStart"), false, false, 1.f, 0.5f);
    }
    break;
    case StateStep::Progress: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeft"), false, false, 1.f);
    }
    break;
    case StateStep::End: {
        GetOwner()->Animator()->Play(ANIMPREFIX("SwingLeftEnd"), false, false, 1.f);
    }
    break;
    }
}

void CElfilisG_NormalAtkL::Exit_Step()
{
    switch (m_Step)
    {
    case StateStep::Start:
        break;
    case StateStep::Progress: {
        if (m_bComboSuccess)
        {
            ELFFSM->AddComboLevel();
        }
        else
        {
            ELFFSM->ClearComboLevel();
        }
    }
    break;
    case StateStep::End:
        break;
    }
}

void CElfilisG_NormalAtkL::Start()
{
    // rotate
    RotateToPlayer();

    if (GetOwner()->Animator()->IsFinish())
    {
        ChangeStep(StateStep::Progress);
    }
}

void CElfilisG_NormalAtkL::Progress()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        // Check Combo
        float Rand = 0.f;
        if (ELFFSM->GetPhase() == 1)
        {
            Rand = GetRandomfloat(1.f, 100.f);
        }

        if (Rand <= 90)
        {
            m_bComboSuccess = true;

            if (ELFFSM->IsNearPlayer())
            {
                ELFFSM->ChangeStateGroup(ElfilisStateGroup::GroundAtkNear, L"GROUND_ATK_NORMAL_R");
            }
            else
            {
                ELFFSM->ChangeStateGroup(ElfilisStateGroup::GroundAtkNear, L"GROUND_ATK_NORMALTELEPORT_R");
            }
        }
        else
        {
            ChangeStep(StateStep::End);
        }
    }
}

void CElfilisG_NormalAtkL::End()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ElfilisStateGroup NextState = ELFFSM->FindNextStateGroup();

        if (NextState == ELFFSM->GetCurStateGroup())
        {
            ELFFSM->RepeatState(L"GROUND_ATK_NORMAL");
        }
        else
        {
            ELFFSM->ChangeStateGroup(NextState);
        }
    }
}