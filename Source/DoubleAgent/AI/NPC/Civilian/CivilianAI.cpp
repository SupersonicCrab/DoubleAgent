// Please don't steal

#include "CivilianAI.h"
#include "DoubleAgent/AI/NPC/FSM/GeneralStates.h"

ACivilianAI::ACivilianAI()
{
    CurrentState = new Wander();
    BehaviourFPS = 3;
}
