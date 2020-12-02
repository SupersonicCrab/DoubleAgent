#pragma once
#include "DoubleAgent/AI/NPC/FSM/FSMAction.h"

class TalkToCrowd : public FSMAction
{
public:
    TalkToCrowd(){};

    virtual bool PerformAction() override;    
};
