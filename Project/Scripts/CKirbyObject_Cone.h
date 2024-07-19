#pragma once
#include "CKirbyObject.h"

class CKirbyObject_Cone : public CKirbyObject
{
private:
    Ptr<CPrefab> m_Crater;
    bool m_bFrmEnter;

public:
    // ������� (����)
    virtual void Attack();
    virtual void AttackEnter();
    virtual void AttackExit();

    virtual void AttackStart();
    virtual void AttackStartEnter();
    virtual void AttackStartExit();

    virtual void AttackEnd();
    virtual void AttackEndEnter();
    virtual void AttackEndExit();

    // ������� (��)
    virtual void AttackCombo1();
    virtual void AttackCombo1Enter();
    virtual void AttackCombo1Exit();

    virtual void AttackCombo1End();
    virtual void AttackCombo1EndEnter();
    virtual void AttackCombo1EndExit();

    // Change/Drop Object
    virtual void DropObjectEnter();
    virtual void ChangeObjectEnter();

public:
    CLONE_DISABLE(CKirbyObject_Cone)
    CKirbyObject_Cone();
    virtual ~CKirbyObject_Cone();
};
