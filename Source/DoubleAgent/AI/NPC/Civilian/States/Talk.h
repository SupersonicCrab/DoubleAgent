#pragma once
#include "Wander.h"
#include "DoubleAgent/AI/NPC/Civilian/Actions/TalkToCrowd.h"
#include "DoubleAgent/AI/NPC/FSM/FSMState.h"

class Talk : public FSMState
{
public:
    Talk()
    {
        Actions.Add(TalkToCrowd());
        Transitions.Add(CowerTransition());
        Transitions.Add(DespawnTransition());
        Transitions.Add(WanderTransition());
    };
};
