#pragma once
#include "CComponent.h"

enum class ForceMode
{
    Force,          // 질량을 사용한 강체에 지속적인 힘을 추가
    Acceleration,   // 질량을 무시하고 강체에 지속적인 가속도 추가
    Impulse,        // 질량을 사용하여 강체에 즉각적인 힘 충격 추가
    VelocityChange, // 질량을 무시하고 강체에 즉각적인 속도 변화 추가
};

enum class CollisionDetection_TYPE
{
    Discrete,
    Continuous,
    ContinuousSpecutive,
};

class CRigidbody : public CComponent
{
private:
    void* m_RuntimeBody;
    float m_Mass;
    float m_Drag;
    float m_AngularDrag;
    bool m_bGravity;
    bool m_bKinematic;
    CollisionDetection_TYPE m_CollisionDetectionType;
    bool m_FreezePosition[3];
    bool m_FreezeRotation[3];

public:
    virtual void finaltick() override;

public:
    void AddForce(Vec3 _Force, ForceMode _Mode = ForceMode::Force);
    void AddTorque(Vec3 _Torque, ForceMode _Mode = ForceMode::Force);

    float GetSpeed();

    Vec3 GetVelocity();
    void SetVelocity(Vec3 _vel);

    Vec3 GetAngularVelocity();
    void SetAngularVelocity(Vec3 _vel);

    Vec3 GetInertiaTensor();
    void SetInertiaTensor(Vec3 _Tensor);

    Vec3 GetLocalCenterOfMass();
    void SetLocalCenterOfMass(Vec3 _Center);

    bool IsSleeping();
    void Sleep();
    void WakeUp();

public:
    float GetMass() const { return m_Mass; }
    void SetMass(float _Mass);

    float GetDrag() const { return m_Drag; }
    void SetDrag(float _Drag);

    float GetAngularDrag() const { return m_AngularDrag; }
    void SetAngularDrag(float _Drag);

    bool IsUseGravity() const { return m_bGravity; }
    void SetUseGravity(bool _bGravity);

    bool IsKinematic() const { return m_bKinematic; }
    void SetKinematic(bool _kinematic);
    CollisionDetection_TYPE GetCollisionDetectionType() { return m_CollisionDetectionType; }
    void SetCollisionDetectionType(CollisionDetection_TYPE _ColDet);

    bool IsFreezePosition(AXIS_TYPE _Axis) { return m_FreezePosition[(UINT)_Axis]; }
    void SetFreezePosition(AXIS_TYPE _Axis, bool _Freeze);

    bool IsFreezeRotation(AXIS_TYPE _Axis) { return m_FreezeRotation[(UINT)_Axis]; }
    void SetFreezeRotation(AXIS_TYPE _Axis, bool _Freeze);

public:
    virtual UINT SaveToLevelFile(FILE* _File) override;
    virtual UINT LoadFromLevelFile(FILE* _File) override;

    CLONE(CRigidbody);

public:
    CRigidbody();
    CRigidbody(const CRigidbody& origin);
    virtual ~CRigidbody();

    friend class CPhysicsMgr;
};
