#pragma once
#include <Engine/CScript.h>
class CPlayerDetectScript : public CScript
{
public:
    virtual void OnTriggerEnter(CCollider* _OtherCollider)override;
    virtual void OnTriggerExit(CCollider* _OtherCollider)override;
};
