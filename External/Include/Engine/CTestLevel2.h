#pragma once
#include "CLevel.h"

class CTestLevel2 : public CLevel
{
public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;
    virtual void render() override;

public:
    CTestLevel2();
    ~CTestLevel2();
};