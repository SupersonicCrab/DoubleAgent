#pragma once
#include "FSMAction.h"
#include "FSMCondition.h"

//Forward declarations
class AFSMController;
class FSMTransition;

class FSMState
{
public:
    FSMState(){};

    TArray<FSMTransition> Transitions;

    TArray<FSMAction> Actions;

    TArray<FSMAction> EntryActions;

    TArray<FSMAction> ExitActions;

    AFSMController* Controller;
};

class FSMTransition
{    
public:
    FSMTransition(){};
    
    FSMCondition Condition;

    FSMAction Action;

    FSMState NewState;
};
