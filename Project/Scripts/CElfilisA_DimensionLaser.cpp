#include "pch.h"
#include "CElfilisA_DimensionLaser.h"
#include "CElfilisFSM.h"
#include "CElfilisDimensionLaser.h"
#include <Engine\CAssetMgr.h>
#include <Engine\CPrefab.h>

CElfilisA_DimensionLaser::CElfilisA_DimensionLaser()
    : m_Dimension{nullptr,}
    , m_DimensionStart(nullptr)
    , m_DimensionScript{nullptr,}
    , m_ProgressStep(0)
    , m_AccTime(0.f)
{
    m_DimensionStartPref =
        CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\ElfilisDimensionLaserStart.pref", L"prefab\\ElfilisDimensionLaserStart.pref");
    Ptr<CPrefab> DimensionPref = CAssetMgr::GetInst()->Load<CPrefab>(L"prefab\\ElfilisDimensionLaser.pref", L"prefab\\ElfilisDimensionLaser.pref");

    if (DimensionPref != nullptr)
    {
        for (int i = 0; i < 5; i++)
        {
            m_bDimensionSpawn[i] = -1;
            m_Dimension[i] = DimensionPref->Instantiate();
            CElfilisDimensionLaser* Script = m_Dimension[i]->GetScript<CElfilisDimensionLaser>();

            if (Script)
            {
                m_DimensionScript[i] = Script;
            }
        }
    }
}

CElfilisA_DimensionLaser::~CElfilisA_DimensionLaser()
{
    for (int i = 0; i < 5; ++i)
    {
        if (m_Dimension[i] && (m_bDimensionSpawn[i] == -1 || m_Dimension[i]->GetLayerIdx() == -1))
        {
            delete m_Dimension[i];
        }
    }
}

void CElfilisA_DimensionLaser::tick()
{
    switch (m_Step)
    {
    case StateStep::Ready: {
        Ready();
    }
    break;
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

void CElfilisA_DimensionLaser::Enter()
{
    m_Step = StateStep::Ready;
    Enter_Step();
}

void CElfilisA_DimensionLaser::Enter_Step()
{
    switch (m_Step)
    {
    case StateStep::Ready: {
        GetOwner()->Animator()->Play(ANIMPREFIX("DimensionLaserReady"), false);
        //@Effect ��¡ ��ƼŬ
        //@CAMERA �ϴú�
    }
    break;
    case StateStep::Start: {
        GetOwner()->Animator()->Play(ANIMPREFIX("DimensionLaserStart"), false, false, 2.5f, 0.3f);
    }
    break;
    case StateStep::Progress: {
        GetOwner()->Animator()->Play(ANIMPREFIX("DimensionLaser"));
        m_ProgressStep = 0;
        m_AccTime = 0.f;
        for (int i = 0; i < 5; ++i)
        {
            m_bDimensionSpawn[i] = false;
        };

        // Laser Spawn
        if (m_DimensionStartPref != nullptr)
        {
            Vec3 NewRot = GetOwner()->Transform()->GetWorldRotation();
            NewRot.x += XMConvertToRadians(50.f);

            CGameObject* DimensionStart = m_DimensionStartPref->Instantiate();
            m_DimensionStart = DimensionStart->GetScript<CElfilisDimensionLaser>();

            if (m_DimensionStart)
            {
                DimensionStart->Transform()->SetWorldPos(GetOwner()->Transform()->GetWorldPos());
                DimensionStart->Transform()->SetWorldRotation(NewRot);
                m_DimensionStart->SetLaserStartLcok(true);
                m_DimensionStart->PlaySpawn();
                GamePlayStatic::SpawnGameObject(DimensionStart, LAYER_MONSTERATK_TRIGGER);
            }
        }
    }
    break;
    case StateStep::End: {
        GetOwner()->Animator()->Play(ANIMPREFIX("DimensionLaserEnd"), false);
    }
    break;
    }
}

void CElfilisA_DimensionLaser::Exit_Step()
{
    switch (m_Step)
    {
    case StateStep::Ready:
        break;
    case StateStep::Start:
        break;
    case StateStep::Progress:
        break;
    case StateStep::End: {
        //@CAMERA ����
    }
    break;
    }
}

void CElfilisA_DimensionLaser::Ready()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ChangeStep(StateStep::Start);
    }
}

void CElfilisA_DimensionLaser::Start()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ChangeStep(StateStep::Progress);
    }
}

void CElfilisA_DimensionLaser::Progress()
{
    m_AccTime += DT;

    switch (m_ProgressStep)
    {
    case 0: {
        // �߽� ����Ʈ ������ �����ٸ� : ������ �߻� & idx 0 ����Ʈ ����
        if (!m_bDimensionSpawn[0] && m_DimensionStart->IsSpawnFinish())
        {
            m_DimensionStart->PlayStartLaser();
            m_DimensionStart->SetLaserRepeat(true);
            SpawnDimension(0);

            m_ProgressStep = 1;
        }
    }
    break;
    case 1: {
        // ����Ʈ ���� ���� : ���� ����Ʈ���� ������ �߻簡 ���۵ƴٸ� ���� ����Ʈ ����
        for (int i = 1; i < 5; i++)
        {
            if (!m_bDimensionSpawn[i] && m_DimensionScript[i - 1]->IsLaserStart())
            {
                SpawnDimension(i);
            }
        }

        // ����Ʈ ���� ���� : ������ ����Ʈ���� ������ �߻簡 ����ƴٸ� �߾� ����Ʈ ������ ����
        if (m_DimensionScript[4]->IsLaserEnd())
        {
            m_DimensionStart->PlayEndLaser();
            m_ProgressStep = 2;
        }
    }
    break;
    case 2: {
        // Change State
        // �߾� ����Ʈ�� �����ٸ� : ����Ʈ�� �������� ������ ���� ��ȯ
        if (m_DimensionStart->IsDisappearFinish())
        {
            GamePlayStatic::DestroyGameObject(m_DimensionStart->GetOwner());
            for (int i = 0; i < 5; ++i)
            {
                if (m_Dimension[i])
                {
                    m_DimensionScript[i]->Reset();
                    GamePlayStatic::DetachObject(m_Dimension[i]);
                }
            }

            ChangeStep(StateStep::End);
        }
    }
    break;
    }
}

void CElfilisA_DimensionLaser::End()
{
    if (GetOwner()->Animator()->IsFinish())
    {
        ELFFSM->ChangeStateGroup(ElfilisStateGroup::AirToGround, L"AIR_TOGROUND_TELEPORT");
    }
}

void CElfilisA_DimensionLaser::SpawnDimension(int _Idx)
{
    Vec3 InitDir = GetOwner()->Transform()->GetWorldDir(DIR_TYPE::FRONT);
    InitDir.y = 0.f;
    InitDir.Normalize();

    Vec3 InitPos = Vec3();
    InitPos += InitDir * -150.f * (2.f - _Idx);

    m_DimensionScript[_Idx]->PlaySpawn();
    m_DimensionScript[_Idx]->Transform()->Slerp(InitDir, 1.f);
    m_DimensionScript[_Idx]->Transform()->SetWorldPos(InitPos);
    GamePlayStatic::SpawnGameObject(m_Dimension[_Idx], LAYER_MONSTERATK_TRIGGER);

    m_bDimensionSpawn[_Idx] = true;
}