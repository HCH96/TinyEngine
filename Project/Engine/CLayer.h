#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer : public CEntity
{
private:
    vector<CGameObject*> m_vecParent;
    vector<CGameObject*> m_vecObjects;
    int m_iLayerIdx;

public:
    void begin();
    void tick();
    void finaltick();

public:
    // 특정 오브젝트를 레이어에서 제거
    bool DetachGameObject(CGameObject* _Object);
    void RegisterGameObject(CGameObject* _Object) { m_vecObjects.push_back(_Object); }
    int GetLayerIdx() const { return m_iLayerIdx; }

    const vector<CGameObject*>& GetParentObjects() const { return m_vecParent; }
    const vector<CGameObject*>& GetLayerObjects() const { return m_vecObjects; }

    void AddObject(CGameObject* _Object, bool _bMove);

    CLONE(CLayer);
public:
    CLayer();
    CLayer(const CLayer& origin);
    virtual ~CLayer();

    friend class CLevel;
};
