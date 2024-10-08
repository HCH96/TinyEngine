#pragma once
#include "CLevelEditor.h"
#include "CMaterialEditor.h"
#include "CBlueprintEditor.h"
#include "CSpriteEditor.h"
#include "CModelEditor.h"
#include "CTileMapEditor.h"
#include "CPhysics2DMaterialEditor.h"
#include "CPhysicMaterialEditor.h"
#include "CSoundEditor.h"

class CEditor;
class CGameObject;
class CGameObjectEx;

class CEditorMgr : public CSingleton<CEditorMgr>
{
    SINGLE(CEditorMgr);

private:
    bool m_bEnabled; // 에디터 활성화/비활성화
    CEditor* m_arrEditor[(UINT)EDITOR_TYPE::END];

    CGameObject* m_SelectedObj;
    bool m_bLockSelectedObj;

    Vec2 m_ViewportSize;
    Vec2 m_ViewportMousePos;

    vector<CGameObjectEx*> m_vecEditorObj;

    HANDLE m_hObserver;
    bool m_bContentAutoLoad;

public:
    void init();
    void tick();
    void render();

public:
    bool IsEnabled() const { return m_bEnabled; }

    CGameObject* GetSelectedObject() const { return m_SelectedObj; }
    void SetSelectedObject(CGameObject* obj);

    bool IsLockSelectedObject() const { return m_bLockSelectedObj; }
    void LockSelectedObject(bool _bLock) { m_bLockSelectedObj = _bLock; }

    void SetViewportSize(Vec2 size) { m_ViewportSize = size; }
    Vec2 GetViewportSize() const { return m_ViewportSize; }

    void SetViewportMousePos(Vec2 pos) { m_ViewportMousePos = pos; }
    Vec2 GetViewportMousePos() const { return m_ViewportMousePos; }

    bool IsContentAutoLoad() const { return m_bContentAutoLoad; }
    void SetContentAutoLoad();

public:
    CEditor* GetEditor(EDITOR_TYPE _Type) const { return m_arrEditor[(UINT)_Type]; }
    CLevelEditor* GetLevelEditor() const { return (CLevelEditor*)m_arrEditor[(UINT)EDITOR_TYPE::LEVEL]; }
    CMaterialEditor* GetMaterialEditor() const { return (CMaterialEditor*)m_arrEditor[(UINT)EDITOR_TYPE::MATERIAL]; }
    CBlueprintEditor* GetBlueprintEditor() const { return (CBlueprintEditor*)m_arrEditor[(UINT)EDITOR_TYPE::BLUEPRINT]; }
    CSpriteEditor* GetSpriteEditor() const { return (CSpriteEditor*)m_arrEditor[(UINT)EDITOR_TYPE::SPRITE]; }
    CModelEditor* GetModelEditor() const { return (CModelEditor*)m_arrEditor[(UINT)EDITOR_TYPE::MODEL]; }
    CTileMapEditor* GetTileMapEditor() const { return (CTileMapEditor*)m_arrEditor[(UINT)EDITOR_TYPE::TILEMAP]; }
    CPhysics2DMaterialEditor* GetPhysics2DMaterialEditor() const
    {
        return (CPhysics2DMaterialEditor*)m_arrEditor[(UINT)EDITOR_TYPE::PHYSICS2D_MATERIAL];
    }
    CPhysicMaterialEditor* GetPhysicMaterialEditor() const { return (CPhysicMaterialEditor*)m_arrEditor[(UINT)EDITOR_TYPE::PHYSIC_MATERIAL]; }
    CSoundEditor* GetSoundEditor() const { return (CSoundEditor*)m_arrEditor[(UINT)EDITOR_TYPE::SOUND]; }

private:
    void SetDarkThemeColors();
    void SetImGuizmoStyle();

private:
    void ObserveContent();
};
