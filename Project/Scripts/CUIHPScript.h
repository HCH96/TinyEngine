#pragma once
#include <Engine/CScript.h>

class CUnitScript;
class CKirbyFSM;

struct DamageTask
{
    float fCurHP;
    float fPrevHP;
};

class CUIHPScript : public CScript
{
private:
    vector<DamageTask> m_vDamageTask;
    vector<DamageTask> m_vHealTask;
    CGameObject* m_pNameObj;
    string m_TargetName;

    CKirbyFSM* m_pFSMScript;
    CUnitScript* m_pUnitScript;

    CMeshRender* m_pRenderer;

    float m_fAccTime;
    float m_fComboTime;
    float m_fHealingTime;

    float m_fMaxHP;
    float m_fCurHP;
    float m_fCurPrevHP;
    float m_fPrevHP;

    bool m_bIsScaling;
    bool m_bDamaged;

    Vec4 m_vDecreaseColor;
    Vec4 m_vBasicColor;

    float m_fDescSpeed;

    bool m_bHpHealed;
    bool m_bIsHealedScaling;

    int m_iEnterTickCount;

    // Sparkle
    float m_fSparkleAccTime;
    float m_fSparkleTermTime;
    bool m_bIsAlreadyFlag;
    bool m_bSparkle;
    bool m_bHPCurrentSparkleFlag;
    bool m_bEmissionFlag;
    bool m_bSoundFlag;

public:
    virtual void tick() override;

public:
    void SetPlayer();
    void SetInitInfo();

    void SetHealingTime(float _fValue) { m_fHealingTime = _fValue; }

private:
    void CaculateShading();
    void CaculateHealShading();

    void HealScaling();
    void Scaling();
    
    void SwitchKirbyName();

    void HPDamageTask();
    void HPHealTask();

    void Sparkle();
    void SparKleReset();

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CUIHPScript)
public:
    CUIHPScript();
    CUIHPScript(const CUIHPScript& Origin);
    virtual ~CUIHPScript();
};
