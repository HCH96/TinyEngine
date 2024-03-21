#include "pch.h"
#include "CScriptMgr.h"

#include "CCameraMoveScript.h"
#include "CLevelChangeScript.h"
#include "CPlayButtonScript.h"
#include "CPlayerCameraScript.h"
#include "CPlayerLifeBarScript.h"
#include "CPlayerManaBarScript.h"
#include "CPlayerScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCameraMoveScript");
	_vec.push_back(L"CLevelChangeScript");
	_vec.push_back(L"CPlayButtonScript");
	_vec.push_back(L"CPlayerCameraScript");
	_vec.push_back(L"CPlayerLifeBarScript");
	_vec.push_back(L"CPlayerManaBarScript");
	_vec.push_back(L"CPlayerScript");
}

CScript* CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CCameraMoveScript" == _strScriptName)
		return new CCameraMoveScript;
	if (L"CLevelChangeScript" == _strScriptName)
		return new CLevelChangeScript;
	if (L"CPlayButtonScript" == _strScriptName)
		return new CPlayButtonScript;
	if (L"CPlayerCameraScript" == _strScriptName)
		return new CPlayerCameraScript;
	if (L"CPlayerLifeBarScript" == _strScriptName)
		return new CPlayerLifeBarScript;
	if (L"CPlayerManaBarScript" == _strScriptName)
		return new CPlayerManaBarScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	return nullptr;
}

CScript* CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CCameraMoveScript;
	case (UINT)SCRIPT_TYPE::LEVELCHANGESCRIPT:
		return new CLevelChangeScript;
	case (UINT)SCRIPT_TYPE::PLAYBUTTONSCRIPT:
		return new CPlayButtonScript;
	case (UINT)SCRIPT_TYPE::PLAYERCAMERASCRIPT:
		return new CPlayerCameraScript;
	case (UINT)SCRIPT_TYPE::PLAYERLIFEBARSCRIPT:
		return new CPlayerLifeBarScript;
	case (UINT)SCRIPT_TYPE::PLAYERMANABARSCRIPT:
		return new CPlayerManaBarScript;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
	}
	return nullptr;
}

const wchar_t* CScriptMgr::GetScriptName(CScript* _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CCameraMoveScript";
	case SCRIPT_TYPE::LEVELCHANGESCRIPT:
		return L"CLevelChangeScript";
	case SCRIPT_TYPE::PLAYBUTTONSCRIPT:
		return L"CPlayButtonScript";
	case SCRIPT_TYPE::PLAYERCAMERASCRIPT:
		return L"CPlayerCameraScript";
	case SCRIPT_TYPE::PLAYERLIFEBARSCRIPT:
		return L"CPlayerLifeBarScript";
	case SCRIPT_TYPE::PLAYERMANABARSCRIPT:
		return L"CPlayerManaBarScript";
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
	}
	return nullptr;
}