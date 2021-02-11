// Please don't steal

#include "FSM.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/KismetMathLibrary.h"

FSMState::~FSMState()
{
    for (int i = 0; i < Transitions.Num(); i++)
    {
        delete Transitions[i];
    }
    for (int i = 0; i < EntryActions.Num(); i++)
    {
        delete EntryActions[i];
    }
    for (int i = 0; i < Actions.Num(); i++)
    {
        delete Actions[i];
    }
    for (int i = 0; i < ExitActions.Num(); i++)
    {
        delete ExitActions[i];
    }
}

FSMTransition::~FSMTransition()
{
    delete Condition;
    Condition = nullptr;
    delete Action;
    Action = nullptr;
}

void AFSMController::BehaviourTick()
{    
    //Iterate through possible transitions
    for (int i = 0; i < CurrentState->Transitions.Num(); i++)
    {
        //If transition condition is valid
        if (CurrentState->Transitions[i]->Condition->TestCondition(this))
        {
            //Perform exit actions of current state if any
            for (int a = 0; a < CurrentState->ExitActions.Num(); a++)
            {
                CurrentState->ExitActions[a]->PerformAction(this);
            }

            //Perform transition action if any
            if (CurrentState->Transitions[i]->Action != nullptr)
                if (!CurrentState->Transitions[i]->Action->PerformAction(this))
                    //Finish update if transition fails
                    return;

            //Change state to new state
            FSMState* NewState = CurrentState->Transitions[i]->GetNewState();
            delete CurrentState;
            CurrentState = NewState;
            CurrentState->Controller = this;

            //Perform entry actions of new state if any
            for (int a = 0; a < CurrentState->EntryActions.Num(); a++)
            {
                CurrentState->EntryActions[a]->PerformAction(this);
            }

            //Finish update
            return;
        }
    }

    //Perform actions of current state if any
    for (int a = 0; a < CurrentState->Actions.Num(); a++)
    {
        CurrentState->Actions[a]->PerformAction(this);
    } 
}

void AFSMController::FindEQS(UEnvQuery* Query)
{
    FEnvQueryRequest QueryRequest = FEnvQueryRequest(Query, this);
    QueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AFSMController::GoToEQS);
}

void AFSMController::GoToEQS(TSharedPtr<FEnvQueryResult> Result)
{
    //Stop if EQS failed
    if (!Result->IsSuccsessful())
        return;
    
    FVector Location = Result->GetItemAsLocation(0);

    LastMoveResult = MoveToLocation(Location, 10.0f, true, true, true, true, nullptr, false);
}

void AFSMController::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(BehaviourTickHandle, this, &AFSMController::BehaviourTick, 1.0/BehaviourFPS, true, UKismetMathLibrary::RandomFloatInRange(0, .9));
}

void AFSMController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    
    LastMoveResult = Result;
}
