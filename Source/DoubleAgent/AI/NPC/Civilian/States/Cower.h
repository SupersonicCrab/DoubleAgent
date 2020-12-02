#pragma once
#include "Wander.h"
#include "DoubleAgent/AI/NPC/Civilian/Actions/PlayAnimation.h"
#include "DoubleAgent/AI/NPC/FSM/FSMState.h"

class Cower : public FSMState
{
public:
    Cower()
    {
        Actions.Add(PlayAnimation());
        //dynamic_cast<PlayAnimation>(Actions[0]).AnimationSequence =
        Transitions.Add(DespawnTransition());
    };
};
