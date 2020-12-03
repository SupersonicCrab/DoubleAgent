// Please don't steal

#pragma once

#include "AIControllerBase.h"
#include "FSM.generated.h"

class FSMCondition
{
public:
    FSMCondition(){};

    virtual bool TestCondition()
    {
        return false;
    };
};

class FSMAction
{
public:
    FSMAction(){};

    virtual bool PerformAction()
    {
      return false;  
    };
};

class FSMTransition;
class AFSMController;

class FSMState
{
public:
    FSMState(){};

    AFSMController* Controller;
    
    TArray<FSMTransition> Transitions;

    TArray<FSMAction> Actions;

    TArray<FSMAction> EntryActions;

    TArray<FSMAction> ExitActions;
};

class FSMTransition
{
public:
    FSMTransition(){};

    FSMCondition Condition;

    FSMAction Action;

    FSMState NewState;
};

UCLASS()
class AFSMController : public AAIControllerBase
{
    GENERATED_BODY()
public:
    FSMState CurrentState;

    void Update();
};
