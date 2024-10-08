#pragma once
#include "CBossState.h"

class CMorphoAtkA_DoubleSwordAtkL : public CBossState
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
    CLONE(CMorphoAtkA_DoubleSwordAtkL)
    CMorphoAtkA_DoubleSwordAtkL();
    virtual ~CMorphoAtkA_DoubleSwordAtkL();
};