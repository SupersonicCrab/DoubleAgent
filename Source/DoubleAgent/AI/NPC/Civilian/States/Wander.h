#pragma once

class Wander : public FSMState
{
public:
    Wander()
    {
        Actions.Add(GoToCrowd());
        Transitions.Add(CowerTransition());
        Transitions.Add(DespawnTransition());
        Transitions.Add(TalkTransition());
    };
};
