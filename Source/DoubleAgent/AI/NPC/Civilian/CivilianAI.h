// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "DoubleAgent/AI/NPC/FSM/FSMController.h"
#include "Actions/RunToDespawn.h"
#include "States/Despawn.h"
#include "States/Talk.h"
#include "States/Wander.h"
#include "States/Cower.h"
#include "CivilianAI.generated.h"

class WanderCondition : public FSMCondition
{
public:
    virtual bool TestCondition() override;
};

class WanderTransition : public FSMTransition
{
public:
    WanderTransition()
    {
        Condition = WanderCondition();
        NewState = Wander();
    };
};

class TalkCondition : public FSMCondition
{
public:
    virtual bool TestCondition() override;
};

class TalkTransition : public FSMTransition
{
public:
    TalkTransition()
    {
        Condition = TalkCondition();
        NewState = Talk();
    };
};

class DespawnCondition : public FSMCondition
{
public:
    virtual bool TestCondition() override;
};

class DespawnTransition : public FSMTransition
{
public:
    DespawnTransition()
    {
        Condition = DespawnCondition();
        NewState = Despawn();
        Action = RunToDespawn();
    }
};

class CowerCondition : public FSMCondition
{
public:
    virtual bool TestCondition() override;
};

class CowerTransition : public FSMTransition
{
public:
    CowerTransition()
    {
        Condition = CowerCondition();
        NewState = Cower();
    }
};

/**
 * 
 */
UCLASS()
class DOUBLEAGENT_API ACivilianAI : public AFSMController
{
	GENERATED_BODY()
public:
    ACivilianAI()
    {
        CurrentState = Wander();
    };    
};
