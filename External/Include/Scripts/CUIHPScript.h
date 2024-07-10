#pragma once
#include <Engine/CScript.h>

class CUIHPScript : public CScript
{
private:
    vector<CGameObject*> m_pSlicingObjects;
    CGameObject* m_pTargetObject;

    CTransform* m_pTr;
    CMeshRender* m_pRenderer;

    Vec3 m_vNormalScale;
    Vec3 m_vNormalPos;

    float m_fAccTime;
    float m_fComboTime;

    float m_fMaxHP;
    float m_fCurHP;
    float m_fPrevHP;

    bool m_bIsCombo;
    bool m_bIsScaling;

    Vec4 m_vDecreaseColor;
    Vec4 m_vBasicColor;

    float m_fDescSpeed;

public:
    virtual void begin() override;
    virtual void tick() override;

private:
    void CaculateShading();
    void Scaling();

private:
    bool IsCombo();
    void SlicingUI(UINT _iSlicingNum);

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

    CLONE(CUIHPScript)
public:
    CUIHPScript();
    virtual ~CUIHPScript();
};
