#pragma once
#include "CBossState.h"

class CMorphoMoveG_HoverDash : public CBossState
{
private:
    float m_Speed;
    float m_RotSpeed;
    float m_PrevDrag;
    float m_AccTime;

public:
    virtual void tick() override;
    virtual void Exit() override;

private:
    void Enter_Step();
    void Exit_Step();

    // tick
    void Start();
    void Progress();
    void End();
    void EndWait();

    void FlyToPlayer();

public:
    CLONE(CMorphoMoveG_HoverDash)
    CMorphoMoveG_HoverDash();
    virtual ~CMorphoMoveG_HoverDash();
};
