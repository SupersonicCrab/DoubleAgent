// Please don't steal

#pragma once

#include "AIControllerBase.h"
#include "FSM.generated.h"


class AFSMController;

class FSMCondition
{
public:
    FSMCondition(){};

    virtual bool TestCondition(AFSMController* Controller)
    {
        return false;
    };
};

class FSMAction
{
public:
    FSMAction(){};
    
    virtual bool PerformAction(AFSMController* Controller)
    {
      return false;  
    };
};

class FSMTransition;

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

    virtual FSMState GetNewState()
    {
        return FSMState();
    } 
};

UCLASS()
class AFSMController : public AAIControllerBase
{
    GENERATED_BODY()
public:
    FSMState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBlackboardData* BlackboardData;

    virtual void Tick(float DeltaSeconds) override;
};
