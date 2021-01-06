// Please don't steal

#pragma once

#include "FSM.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

class DeleteSelf : public FSMAction
{
public:
    DeleteSelf(){};
    
    virtual bool PerformAction(AFSMController* Controller) override;
};

class CowerAnimation : public FSMAction
{
public:
    CowerAnimation(){};
    virtual bool PerformAction(AFSMController* Controller) override;
};

class SpeakToCrowd : public FSMAction
{
public:
    SpeakToCrowd(){};
    virtual bool PerformAction(AFSMController* Controller) override;
};

class GotoCrowd : public FSMAction
{
public:
    GotoCrowd(){};
    virtual bool PerformAction(AFSMController* Controller) override;
};

class GoToDespawn : public FSMAction
{
public:
    GoToDespawn(){};
    virtual bool PerformAction(AFSMController* Controller) override;
};