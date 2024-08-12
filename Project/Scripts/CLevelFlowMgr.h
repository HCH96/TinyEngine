#pragma once

#include <Engine/CScript.h>

class CGameObject;

class CLevelFlowMgr : public CScript
{
private:
    wstring m_CurLevelPath;
    string m_NextLevelPath;

    CGameObject* m_DimensionFadeEffect;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void OnDimensionFade();
    void OffDimensionFade();

public:
    virtual void LevelStart();
    virtual void LevelEnd();
    virtual void LevelExit();
    virtual void LevelRestart();
    
private:
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