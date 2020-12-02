#pragma once
#include "DoubleAgent/AI/NPC/FSM/FSMAction.h"

class DeleteSelf : public FSMAction
{
public:
    virtual bool PerformAction() override;
};
