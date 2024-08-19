#pragma once
#include "CState.h"

class CKirbyVacuum1Start : public CState
{
private:
    float m_SavedSpeed;

public:
    virtual void tick();
    virtual void Enter();
    virtual void Exit();

public:
    CLONE(CKirbyVacuum1Start)
    CKirbyVacuum1Start();
    virtual ~CKirbyVacuum1Start();
};