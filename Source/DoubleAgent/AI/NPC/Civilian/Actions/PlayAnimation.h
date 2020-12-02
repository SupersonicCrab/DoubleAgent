#pragma once
#include "DoubleAgent/AI/NPC/FSM/FSMAction.h"

class PlayAnimation : FSMAction
{
public:
    virtual bool PerformAction() override;

    UAnimSequence* AnimationSequence;
};
