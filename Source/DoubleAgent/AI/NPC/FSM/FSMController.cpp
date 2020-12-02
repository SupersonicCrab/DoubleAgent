// Please don't steal


#include "FSMController.h"

void AFSMController::Update()
{
    //Iterate through possible transitions
    for (int i = 0; i < CurrentState.Transitions.Num(); i++)
    {
        //If transition condition is valid
        if (CurrentState.Transitions[i].Condition.TestCondition())
        {
            //Perform exit actions of current state if any
            for (int a = 0; a < CurrentState.ExitActions.Num(); a++)
            {
                CurrentState.ExitActions[a].PerformAction();
            }
            
            //Perform transition action if any
            CurrentState.Transitions[i].Action.PerformAction();

            //Change state to new state
            CurrentState = CurrentState.Transitions[i].NewState;
            CurrentState.Controller = this;

            //Perform entry actions of new state if any
            for (int a = 0; a < CurrentState.EntryActions.Num(); a++)
            {
                CurrentState.EntryActions[a].PerformAction();
            }

            //Finish update
            return;
        }
    }

    //Perform actions of current state if any
    for (int a = 0; a < CurrentState.Actions.Num(); a++)
    {
        CurrentState.Actions[a].PerformAction();
    }
}
