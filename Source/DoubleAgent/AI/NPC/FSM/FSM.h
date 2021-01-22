// Please don't steal

#pragma once

#include "DoubleAgent/AI/NPC/AIControllerBase.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

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
    virtual ~FSMState();

    AFSMController* Controller;
    
    TArray<FSMTransition*> Transitions;

    TArray<FSMAction*> Actions;

    TArray<FSMAction*> EntryActions;

    TArray<FSMAction*> ExitActions;
};

class FSMTransition
{
public:
    FSMTransition(){};
    virtual ~FSMTransition();

    FSMCondition* Condition = nullptr;

    FSMAction* Action = nullptr;

    virtual FSMState* GetNewState()
    {
        return new FSMState();
    } 
};

UCLASS()
class AFSMController : public AAIControllerBase
{
    GENERATED_BODY()

public:
    FSMState* CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBlackboardData* BlackboardData;

    UPROPERTY()
    float BehaviourFPS = 15;
    
    FTimerHandle BehaviourTickHandle;
    
    void BehaviourTick();

    void FindEQS(UEnvQuery* Query);
    void GoToEQS(TSharedPtr<FEnvQueryResult> Result);

    FPathFollowingResult LastMoveResult = FPathFollowingResult(FPathFollowingResultFlags::Success);

    //Base overrides
    virtual void BeginPlay() override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
};
