#include "pch.h"
#include "CKirbyDeath.h"

#include "CCameraController.h"

#include "CLevelFlowMgr.h"

CKirbyDeath::CKirbyDeath()
{
}

CKirbyDeath::~CKirbyDeath()
{
}

void CKirbyDeath::tick()
{
    m_Acc += DT_ENGINE;

    if (m_Acc != 0.f)
    {
        PLAYER->Animator()->SetPlay(false);
    }

    // 멈추는 시간이 끝난경우
    if (m_Acc > m_Duration)
    {
        // 애니메이션 재생
        PLAYER->Animator()->SetPlay(true);
    }

    // 멈추는 시간이 끝난경우
    if (m_Acc > m_Duration && m_bSoundPlay == false)
    {
        // sound
        CSoundMgr::GetInst()->SetSoundLock(false);
        GamePlayStatic::Play2DSound(L"sound\\stream\\K15_Dead1\\K15_Dead1.marker.dspadpcm.wav", 1, KIRBY_EFFECTSOUND * 4.f);
        CSoundMgr::GetInst()->SetSoundLock(true);
        m_bSoundPlay = true;
    }

    if (m_Acc > m_FaceDuraion)
    {
        CPlayerMgr::SetPlayerFace(FaceType::Close);

        // Layer Off
        CAMERACTRL->TurnOffMonsterLayer();

        // 커비와 UI를 제외한 배경은 검은색으로 Fade Out
        if (m_bFadeEffect == false)
        {
            CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
            FlowMgr->OnDimensionFade(1.f, 0.f, 2.5f);

            m_bFadeEffect = true;
        }
    }

    if (m_Acc > m_DeathDuraion)
    {
        CTimeMgr::GetInst()->SetTimeScale(1.f);

        // Level Restart
        CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
        FlowMgr->LevelRestart();
    }
}

void CKirbyDeath::Enter()
{
    // Effect Lock
    CAMERACTRL->SetEffectLock(true);

    PLAYERFSM->SetInvincible(true);
    PLAYERFSM->SetEmissive(false, 0.f);

    m_Acc = 0.f;
    m_Duration = 1.f;
    m_FaceDuraion = 3.f;
    m_DeathDuraion = 6.f;
    m_bFadeEffect = false;
    m_bSoundPlay = false;

    // FlowMgr보다 틱이 늦기 때문에 첫틱에는 효과를 받을수 없어 기존에 저장되어있던 coef값을 기준으로 Fade가 된다. 그래서 coef를 미리 1로 초기화
    CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
    FlowMgr->OnDimensionFade(1.f);
    FlowMgr->OffDimensionFade();

    GamePlayStatic::StopAllSound();
    CSoundMgr::GetInst()->SetSoundLock(true);

    // m_Duration 만큼 시간을 멈추기
    CTimeMgr::GetInst()->SetTimeScale(0.f, m_Duration);

    // 커비 표정
    CPlayerMgr::SetPlayerFace(FaceType::Frown);

    // CameraSetting
    CCameraController* CamCtrl = CAMERACTRL;

    CamCtrl->Normal(false);
    CamCtrl->SetLock(true, m_FaceDuraion);
    CamCtrl->LockSetup(true);

    CamCtrl->SetOffset(Vec3(0.f, 10.f, 0.f));
    CamCtrl->SetTargetOffset(Vec3(0.f, 0.f, 0.f));
    CamCtrl->RotationLookDirRightAxis(50.f);

    CamCtrl->SetLookDist(100.f);
    CamCtrl->SetZoomMinSpeed(50.f);
    CamCtrl->SetZoomMaxSpeed(100.f);
    CamCtrl->SetZoomThreshold(500.f);

    // Camera Shake
    CamCtrl->Shake(0.5f, 50.f, 50.f);

    Vec3 CamPos = CamCtrl->GetOwner()->Transform()->GetWorldPos();

    // 커비 방향 설정
    Vec3 PlayerPos = PLAYER->Transform()->GetWorldPos();
    Vec3 Dir = CamPos - PlayerPos;
    Dir.y = 0.f;
    Dir.Normalize();

    ForceDirInfo DirInfo = {ForceDirType::STAGEEVENT, Dir, true};
    PLAYERCTRL->ForceDir(DirInfo);
    PLAYERFSM->SetGlobalState(true);
    GetOwner()->Animator()->Play(ANIMPREFIX("Death"), false);

    PLAYERCTRL->LockInput();
    PLAYERCTRL->LockMove();
    PLAYERCTRL->LockJump();

    // UI 끄기
    {
        CLevelFlowMgr* FlowMgr = CLevelMgr::GetInst()->GetCurrentLevel()->FindObjectByName(L"Manager")->GetScript<CLevelFlowMgr>();
        FlowMgr->TurnOffBossHP();
        FlowMgr->TurnOffPlayerHP();
        FlowMgr->ActiveOffDropUI();
    }
}

void CKirbyDeath::Exit()
{
    PLAYERCTRL->UnlockDirection();
    PLAYERCTRL->UnlockMove();
    PLAYERCTRL->UnlockJump();
}