#pragma once
#include "CBossState.h"

class CMorphoAtkG_NormalNear_Atk3 : public CBossState
{
private:
    Vec3 m_ForceDir;
    bool m_bFrmEnter;

public:
    virtual void tick() override;
    virtual void Enter() override;

private:
    virtual void Enter_Step() override;
    virtual void Exit_Step() override;

    // tick
    void Progress();
    void End();

public:
    CLONE(CMorphoAtkG_NormalNear_Atk3)
    CMorphoAtkG_NormalNear_Atk3();
    virtual ~CMorphoAtkG_NormalNear_Atk3();
};
