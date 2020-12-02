#pragma once
#include "DoubleAgent/AI/NPC/FSM/FSMAction.h"

class GoToCrowd : public FSMAction
{
public:
    GoToCrowd(): FSMAction(){};
    
    virtual bool PerformAction() override;
};
