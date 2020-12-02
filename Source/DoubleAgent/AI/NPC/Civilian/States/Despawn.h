#pragma once
#include "DoubleAgent/AI/NPC/Civilian/Actions/DeleteSelf.h"
#include "DoubleAgent/AI/NPC/FSM/FSMState.h"

class Despawn : public FSMState
{
public:
    Despawn()
    {
      Actions.Add(DeleteSelf());
    };
};
