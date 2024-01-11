#pragma once
#include "CEntity.h"

class CLevel;
class CGameObject;

class CLevelMgr : public CSingleton<CLevelMgr>
{
    SINGLE(CLevelMgr);

private:
    CLevel* m_CurLevel;

public:
    CLevel* GetCurrentLevel() const { return m_CurLevel; }

private:
    CGameObject* m_SelectedObj;

public:
    CGameObject* GetSelectedObject() const { return m_SelectedObj; }
    void SetSelectedObject(CGameObject* obj) { m_SelectedObj = obj; }

    void ChangeLevel(CLevel* _NextLevel);

public:
    void init();
    void tick();
};
