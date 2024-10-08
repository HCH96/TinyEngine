#pragma once

#include <Engine/CScript.h>

#define DOT_TERM 1.f

enum class DAMAGE_TYPE
{
    NORMAL,
    DOT,

    END,
};

// Unit 정보 구조체는 자식 생성자에서 넣어주기
struct UnitInfo
{
    float HP;
    float MAXHP;
    float Speed;
    float RotationSpeed;
    float JumpPower;
    float ATK;
};

struct UnitHit
{
    DAMAGE_TYPE Type;
    Vec3 HitDir;
    float Damage;
    float Duration;
    float Acc;
};

class CUnitScript : public CScript
{
private:
    Vec3 m_HitDir;

protected:
    UnitInfo m_InitInfo;
    UnitInfo m_CurInfo;
    UnitInfo m_PrevInfo;
    std::list<UnitHit> m_HitHistory;

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void AttackReward() {}           // Atk: Unit의 Attack에 따른 보상 처리 (흡혈효과 등)
    virtual void GetDamage(UnitHit _Damage); // Hit: Unit의 Hit에 따른 패널티 처리 (HP감소 등)

public:
    void SetTestInfo(UnitInfo _info) { m_CurInfo = _info; }

protected:
    float DamageProc();
    void SetInitInfo(UnitInfo _Info) { m_InitInfo = _Info; }
    void SetInfo(UnitInfo _Info) { m_CurInfo = _Info; }
    void SetHitDirHorizen();
    void ClearHitDir() { m_HitDir = Vec3(); }

public:
    const UnitInfo& GetInitInfo() const { return m_InitInfo; }
    const UnitInfo& GetPrevInfo() const { return m_PrevInfo; }
    const UnitInfo& GetCurInfo() const { return m_CurInfo; }
    Vec3 GetHitDir() const { return m_HitDir; }
    bool IsGetDamage() const { return m_PrevInfo.HP - m_CurInfo.HP >= 0.1f ? true : false; }
    bool IsDeath() const { return m_CurInfo.HP <= 0; }

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    virtual CUnitScript* Clone() = 0;
    CUnitScript(UINT _Type);
    CUnitScript(const CUnitScript& _Origin);
    virtual ~CUnitScript();
};
