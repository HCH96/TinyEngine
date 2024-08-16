#pragma once

#include <Engine/CScript.h>

class CGameObject;
class CFadeOutScript;

class CLevelFlowMgr : public CScript
{
private:
    bool m_bFadeOut;
    float m_FadeOutAcc;
    float m_FadeOutDuration;

    wstring m_CurLevelPath;
    string m_NextLevelPath;

    CGameObject* m_DimensionFadeEffect;
    CGameObject* m_FadeOutObj;
    CFadeOutScript* m_FadeOutScript;
    Ptr<CMaterial> m_ToneMappingMtrl;

public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void TriggerEvent(UINT _Idx){};

public:
    void OnDimensionFade();
    void OffDimensionFade();
    void SetFadeOutColor(Vec3 _Color);
    void SetFadeOut(Vec3 _Color, bool _bReverse, float _Duration, float _Speed);
    void SetToneMappingParam(bool _bBloomEnable = false, bool _bBlendMode = false, float _BloomStrength = 0.5f, float _Threshold = 0.f,
                             float _FilterRadius = 1.f, float _Exposure = 1.f, float _Gamma = 2.2f);

public:
    virtual void LevelStart();
    virtual void LevelEnd();
    virtual void LevelRestart();

private:
    virtual void LevelExit();
    virtual void MtrlParamUpdate();

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

public:
    virtual CLevelFlowMgr* Clone() = 0;
    CLevelFlowMgr(UINT _Type);
    CLevelFlowMgr(const CLevelFlowMgr& _Origin);
    virtual ~CLevelFlowMgr();
};
