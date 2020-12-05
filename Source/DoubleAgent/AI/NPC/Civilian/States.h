// Please don't steal

#pragma once

#include "Actions.h"
#include "DoubleAgent/AI/NPC/FSM.h"

class Despawn : public FSMState
{
public:
    Despawn();
};

class DespawnCondition : public FSMCondition
{
public:
    virtual bool TestCondition(AFSMController* Controller) override;
};

class DespawnTransition : public FSMTransition
{
public:
    DespawnTransition()
    {
        Action = new GoToDespawn();
        Condition = new DespawnCondition();
    };    
    virtual FSMState* GetNewState() override;
};

class Cower : public FSMState
{
public:
    Cower();
};

class CowerCondition : public FSMCondition
{
public:
    virtual bool TestCondition(AFSMController* Controller) override;
};

class CowerTransition : public FSMTransition
{
public:
    CowerTransition()
    {
        Condition = new CowerCondition();
    };
    virtual FSMState* GetNewState() override;
};

class Talk : public FSMState
{
public:
    Talk();
};

class TalkCondition : public FSMCondition
{
public:
    virtual bool TestCondition(AFSMController* Controller) override;
};

class TalkTransition : public FSMTransition
{
public:
    TalkTransition()
    {
        Condition = new TalkCondition();
    };
    virtual FSMState* GetNewState() override;
};

class Wander : public FSMState
{
public:
    Wander();
};

class WanderCondition : public FSMCondition
{
public:
    virtual bool TestCondition(AFSMController* Controller) override;
};

class WanderTransition : public FSMTransition
{
public:
    WanderTransition()
    {
        Condition = new WanderCondition();
    };
    virtual FSMState* GetNewState() override;
};
