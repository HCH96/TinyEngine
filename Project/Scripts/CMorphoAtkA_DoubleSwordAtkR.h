#pragma once
#include "CBossState.h"

class CMorphoAtkA_DoubleSwordAtkR : public CBossState
{
private:
    CGameObject* m_LightningEffect;
    bool m_bFrmEnter;

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

public:
    CLONE(CMorphoAtkA_DoubleSwordAtkR)
    CMorphoAtkA_DoubleSwordAtkR();
    virtual ~CMorphoAtkA_DoubleSwordAtkR();
};
