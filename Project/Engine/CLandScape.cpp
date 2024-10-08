#include "pch.h"
#include "CLandScape.h"

#include "CKeyMgr.h"
#include "CTransform.h"
#include "CRenderMgr.h"

#include "CCamera.h"

CLandScape::CLandScape()
    : CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
    , m_FaceX(64)
    , m_FaceZ(64)
    , m_CSHeightMap(nullptr)
    , m_HeightMapTex(nullptr)
    , m_CSRaycast(nullptr)
    , m_CrossBuffer(nullptr)
    , m_CSWeightMap(nullptr)
    , m_WeightMapBuffer(nullptr)
    , m_WeightWidth(0)
    , m_WeightHeight(0)
    , m_WeightIdx(0)
    , m_TileArrTex(nullptr)
    , m_Mode(LANDSCAPE_MODE::NONE)
    , m_BrushTex(nullptr)
    , m_BrushStrength(1.f)
    , m_BrushScale(Vec2(0.3f, 0.3f))

{
    Init();
    SetFrustumCheck(false);
}

CLandScape::~CLandScape()
{
    if (nullptr != m_CrossBuffer)
    {
        delete m_CrossBuffer;
        m_CrossBuffer = nullptr;
    }

    if (nullptr != m_WeightMapBuffer)
    {
        delete m_WeightMapBuffer;
        m_WeightMapBuffer = nullptr;
    }
}

void CLandScape::finaltick()
{
    if (LANDSCAPE_MODE::NONE == m_Mode)
    {
        return;
    }

    if (KEY_PRESSED(KEY::LBTN))
    {
        Raycasting();

        if (LANDSCAPE_MODE::HEIGHT_MAP == m_Mode)
        {
            // 교점 위치정보를 토대로 높이를 수정 함
            m_CSHeightMap->SetInputBuffer(m_CrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅

            m_CSHeightMap->SetBrushTex(m_BrushTex); // 사용할 브러쉬 텍스쳐 세팅
            m_CSHeightMap->SetBrushIndex(0);        // 브러쉬 인덱스 설정

            // Shift 키를 누른 상태면 방향 반전
            if (KEY_TAP(KEY::LSHIFT) || KEY_PRESSED(KEY::LSHIFT))
                m_CSHeightMap->SetBrushStrength(m_BrushStrength * -1.f);
            else
                m_CSHeightMap->SetBrushStrength(m_BrushStrength);

            m_CSHeightMap->SetBrushScale(m_BrushScale); // 브러쉬 크기
            m_CSHeightMap->SetHeightMap(m_HeightMapTex);
            m_CSHeightMap->Execute();
        }
        else if (LANDSCAPE_MODE::SPLAT == m_Mode)
        {
            // 피킹 위치정보를 토대로 가중치를 수정함
            m_CSWeightMap->SetInputBuffer(m_CrossBuffer); // 레이 캐스트 위치
            m_CSWeightMap->SetBrushArrTex(m_BrushTex);
            m_CSWeightMap->SetBrushIndex(0);
            m_CSWeightMap->SetBrushScale(m_BrushScale);                                    // 브러쉬 크기
            m_CSWeightMap->SetWeightMap(m_WeightMapBuffer, m_WeightWidth, m_WeightHeight); // 가중치맵, 가로 세로 개수
            m_CSWeightMap->SetWeightIdx(m_WeightIdx);
            m_CSWeightMap->Execute();
        }
    }
}

void CLandScape::render()
{
    UpdateData();

    GetMesh()->render(0);

    // 가중치 버퍼 클리어(컴퓨터 쉐이더에서도 써야해서)
    m_WeightMapBuffer->Clear(17);
}

void CLandScape::render(Ptr<CMaterial> _mtrl)
{
    Transform()->UpdateData();
    _mtrl->UpdateData();

    GetMesh()->render(0);
}

void CLandScape::render(UINT _Subset)
{
    render();
}

UINT CLandScape::SaveToLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fwrite(&m_BrushStrength, 1, sizeof(float), _File);
    fwrite(&m_BrushScale, 1, sizeof(Vec2), _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(Vec2);

    return MemoryByte;
}

UINT CLandScape::LoadFromLevelFile(FILE* _File)
{
    UINT MemoryByte = 0;

    fread(&m_BrushStrength, 1, sizeof(float), _File);
    fread(&m_BrushScale, 1, sizeof(Vec2), _File);

    MemoryByte += sizeof(float);
    MemoryByte += sizeof(Vec2);

    return MemoryByte;
}

void CLandScape::UpdateData()
{
    Transform()->UpdateData();

    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_0, m_FaceX);
    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::INT_1, m_FaceZ);
    GetMaterial(0)->SetTexParam(TEX_PARAM::TEX_3, m_HeightMapTex);

    // 가중치 버퍼 전달
    m_WeightMapBuffer->UpdateData(17);

    // 가중치 버퍼 해상도 전달
    Vec2 vWeightMapResolution = Vec2((float)m_WeightWidth, (float)m_WeightHeight);
    GetMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC2_0, vWeightMapResolution);

    // 타일 텍스쳐 전달
    if (nullptr != m_TileArrTex)
    {
        GetMaterial(0)->SetTexParam(TEX_PARAM::TEXARR_0, m_TileArrTex);

        // 타일 배열 개수 전달
        float m_fTileCount = float(m_TileArrTex->GetArraySize() / 2); // 색상, 노말 합쳐져있어서 나누기 2 해줌
        GetMaterial(0)->SetScalarParam(SCALAR_PARAM::FLOAT_0, m_fTileCount);
    }

    GetMaterial(0)->UpdateData();
}

void CLandScape::Raycasting()
{
    // 시점 카메라를 가져옴
    CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCamera();
    if (nullptr == pMainCam)
        return;

    // 월드 기준 광선을 지형의 로컬로 보냄
    const Matrix& matWorldInv = Transform()->GetWorldInvMat();
    const tRay ray = pMainCam->GetRay();

    tRay CamRay = {};
    CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
    CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
    CamRay.vDir.Normalize();

    // 지형과 카메라 Ray 의 교점을 구함
    tRaycastOut out = {Vec2(0.f, 0.f), (float)0x7fffffff, 0};
    m_CrossBuffer->SetData(&out, 1);

    m_CSRaycast->SetHeightMap(m_HeightMapTex);
    m_CSRaycast->SetFaceCount(m_FaceX, m_FaceZ);
    m_CSRaycast->SetCameraRay(CamRay);
    m_CSRaycast->SetOuputBuffer(m_CrossBuffer);

    m_CSRaycast->Execute();

    // 디버깅용 코드
    // m_CrossBuffer->GetData(&out);
}