#pragma once
#include "DoubleAgent/AI/NPC/FSM/FSMAction.h"

class RunToDespawn : FSMAction
{
public:
    RunToDespawn(){};

    virtual bool PerformAction() override;    
};
