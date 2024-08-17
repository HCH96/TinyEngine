#include "pch.h"
#include "CFlowMgr_BossElfilis.h"
#include "CBossMgr.h"
#include "CElfilisFSM.h"
#include "CCameraController.h"

CFlowMgr_BossElfilis::CFlowMgr_BossElfilis()
    : CLevelFlowMgr(FLOWMGR_BOSSELFILIS)
    , m_FlowState(BossLevelFlow::LevelStart)
    , m_LevelEnterWall(nullptr)
{
}

CFlowMgr_BossElfilis::CFlowMgr_BossElfilis(const CFlowMgr_BossElfilis& _Origin)
    : CLevelFlowMgr(_Origin)
    , m_FlowState(BossLevelFlow::LevelStart)
    , m_LevelEnterWall(nullptr)
{
}

CFlowMgr_BossElfilis::~CFlowMgr_BossElfilis()
{
}

void CFlowMgr_BossElfilis::begin()
{
    CLevelFlowMgr::begin();
    SetToneMappingParam(true, true, 1.f, 1.88f, 0.3f, 0.3f, 0.5f);
    SetFadeEffectColor(Vec3(180.f, 140.f, 200.f));

    CGameObject* Camera = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Main Camera", LAYER_DEFAULT);
    CCameraController* CameraController = Camera ? Camera->GetScript<CCameraController>() : nullptr;

    if (CameraController)
    {
        Vec3 Dir = Vec3(-0.57f, -0.49f, -0.66f).Normalize();
        CameraController->SetLookDir(Dir);
        CameraController->SetLookDist(400.f);
    }

    m_LevelEnterWall = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"LevelEnterWall", LAYER_STATIC_TRIGGER);
}

void CFlowMgr_BossElfilis::tick()
{
    CLevelFlowMgr::tick();

    switch (m_FlowState)
    {
    case BossLevelFlow::LevelStart: {
        BOSS->SetActive(false);
        if (m_LevelEnterWall)
        {
            m_LevelEnterWall->SetActive(false);
        }

        m_FlowState = BossLevelFlow::WaitBoss;
    }
    break;

    case BossLevelFlow::WaitBoss:
        break;
    case BossLevelFlow::Fight:
        break;
    }
}

void CFlowMgr_BossElfilis::TriggerEvent(UINT _Idx)
{
    if (_Idx == 0)
    {
        SpawnElfilis();
        m_FlowState = BossLevelFlow::Fight;
    }
}

void CFlowMgr_BossElfilis::SpawnElfilis()
{
    BOSS->SetActive(true);
    ELFFSM->ChangeStateGroup(ElfilisStateGroup::DEMO, L"DEMO_APPEAR1");

    if (m_LevelEnterWall)
    {
        m_LevelEnterWall->SetActive(true);
    }
}

UINT CFlowMgr_BossElfilis::SaveToLevelFile(FILE* _File)
{
    return 0;
}

UINT CFlowMgr_BossElfilis::LoadFromLevelFile(FILE* _File)
{
    return 0;
}