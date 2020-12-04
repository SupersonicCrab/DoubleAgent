#include "FSM.h"

void AFSMController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //Iterate through possible transitions
    for (int i = 0; i < CurrentState.Transitions.Num(); i++)
    {
        //If transition condition is valid
        if (CurrentState.Transitions[i].Condition.TestCondition(this))
        {
            //Perform exit actions of current state if any
            for (int a = 0; a < CurrentState.ExitActions.Num(); a++)
            {
                CurrentState.ExitActions[a].PerformAction(this);
            }

            //Perform transition action if any
            CurrentState.Transitions[i].Action.PerformAction(this);

            //Change state to new state
            CurrentState = CurrentState.Transitions[i].GetNewState();
            CurrentState.Controller = this;

            //Perform entry actions of new state if any
            for (int a = 0; a < CurrentState.EntryActions.Num(); a++)
            {
                CurrentState.EntryActions[a].PerformAction(this);
            }

            //Finish update
            return;
        }
    }

    //Perform actions of current state if any
    for (int a = 0; a < CurrentState.Actions.Num(); a++)
    {
        CurrentState.Actions[a].PerformAction(this);
    } 
}
