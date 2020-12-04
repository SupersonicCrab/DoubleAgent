// Please don't steal

#include "States.h"
#include "Actions.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

Despawn::Despawn()
{
    Actions.Add(DeleteSelf());
}

bool DespawnCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState DespawnTransition::GetNewState()
{
    return Despawn();
}

Cower::Cower()
{
    Actions.Add(CowerAnimation());
    Transitions.Add(DespawnTransition());
}

bool CowerCondition::TestCondition(AFSMController* Controller)
{
    return Controller->GetBlackboardComponent()->IsVectorValueSet("LoudNoiseLocation");
}

FSMState CowerTransition::GetNewState()
{
    return Cower();
}

Talk::Talk()
{
    Actions.Add(SpeakToCrowd());
    Transitions.Add(CowerTransition());
    Transitions.Add(DespawnTransition());
    Transitions.Add(WanderTransition());
}

bool TalkCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState TalkTransition::GetNewState()
{
    return Talk();
}

Wander::Wander()
{
    Actions.Add(GotoCrowd());
    Transitions.Add(CowerTransition());
    Transitions.Add(DespawnTransition());
    Transitions.Add(TalkTransition());
}

bool WanderCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState WanderTransition::GetNewState()
{
    return Wander();
}
