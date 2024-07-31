#pragma once
#include "CFSMScript.h"

enum class ElfilisG_ToAir
{
    ToAir,
    PUBLIC,
};

enum class ElfilisA_Idle
{
    Idle,
    PUBLIC,
};

enum class ElfilisA_Move
{
    MoveSide,
    PUBLIC,
};

enum class ElfilisA_SmallAtk
{
    ThrowSpear,
    PortalLazer,
    PUBLIC,
};

enum class ElfilisA_BigAtk
{
    PUBLIC,

    DrawLazer,
    PortalStar,
    RotSpear,
    BigElfilisCombo, //? 이거 BigAtk이 아니라 BigElfilisAtkState를 따로 파야하남
};

enum class ElfilisA_ToGround
{
    Teleport,
    TeleportCombo,
    PUBLIC,

    FlyDown,
    FlyDownCombo,
    FlyScratch,
};

enum class ElfilisStateGroup
{
    GroundIdle,
    GroundMove,
    GroundMoveAtk,
    GroundAtk,
    GroundToAir,

    AirIdle,
    AirMove,
    AirSmallAtk,
    AirBigAtk,
    AirToGround,

    END,
};

// ==============================================

class CElfilisFSM : public CFSMScript
{
private:
    map<ElfilisStateGroup, vector<wstring>[2]> m_StateGroup; // ( StateGroup, {{PublicStates}, {PrivateStates}} )
    ElfilisStateGroup m_CurStateGroup;
    UINT m_Phase;
    UINT m_ComboLevel;
    bool m_bAttackRepeat;

    // ground
    UINT m_GroundAttackCount;

    // map
    Vec3 m_MapFloorOffset;
    float m_MapSizeRadius;

public:
    void ChangeStateGroup_RandState(ElfilisStateGroup _Group);
    void ChangeStateGroup_SetState(ElfilisStateGroup _Group, const wstring& _State);
    void RepeatState(wstring _State = L"");
    ElfilisStateGroup FindNextStateGroup();

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void SetComboLevel(UINT _Level) { m_ComboLevel = _Level; }
    void AddComboLevel() { m_ComboLevel++; }

    ElfilisStateGroup GetCurStateGroup() const { return m_CurStateGroup; }
    const vector<wstring>& GetCurPublicStates() const;
    UINT GetPhase() const { return m_Phase; }
    UINT GetComboLevel() const { return m_ComboLevel; }
    float GetMapSizeRadius() const { return m_MapSizeRadius; }
    Vec3 GetMapFloorOffset() const { return m_MapFloorOffset; }

private:
    void ChangStateGroup(ElfilisStateGroup _Group);
    void AddGroupPublicState(ElfilisStateGroup _Group, const wstring& _StateName, CState* _State);
    void AddGroupPrivateState(ElfilisStateGroup _Group, const wstring& _StateName, CState* _State);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CElfilisFSM)
    CElfilisFSM();
    CElfilisFSM(const CElfilisFSM& _Origin);
    virtual ~CElfilisFSM();
};