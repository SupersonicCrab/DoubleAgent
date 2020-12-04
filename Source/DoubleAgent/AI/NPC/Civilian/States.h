// Please don't steal

#pragma once

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
        Condition = DespawnCondition();
    };
    virtual FSMState GetNewState() override;
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
        Condition = CowerCondition();
    };
    virtual FSMState GetNewState() override;
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
        Condition = TalkCondition();
    };
    virtual FSMState GetNewState() override;
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
        Condition = WanderCondition();
    };
    virtual FSMState GetNewState() override;
};
