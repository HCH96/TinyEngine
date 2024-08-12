#pragma once
#include "CBossState.h"

class CMorpho_Idle : public CBossState
{
public:
    virtual void tick() override;

private:
    virtual void Enter_Step() override;
    virtual void Exit_Step() override;

public:
    CLONE(CMorpho_Idle)
    CMorpho_Idle();
    virtual ~CMorpho_Idle();
};