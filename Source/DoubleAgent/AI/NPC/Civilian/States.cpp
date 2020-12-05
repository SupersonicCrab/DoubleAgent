// Please don't steal

#include "States.h"
#include "Actions.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

Despawn::Despawn()
{
    Actions.Add(new DeleteSelf());
}

bool DespawnCondition::TestCondition(AFSMController* Controller)
{
    UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
    return Blackboard->GetValueAsFloat("Detection") > 90 && !Blackboard->IsVectorValueSet("LoudNoiseLocation");
}

FSMState* DespawnTransition::GetNewState()
{
    return new Despawn();
}

Cower::Cower()
{
    Actions.Add(new CowerAnimation());
    Transitions.Add(new DespawnTransition());
}

bool CowerCondition::TestCondition(AFSMController* Controller)
{
    return Controller->GetBlackboardComponent()->IsVectorValueSet("LoudNoiseLocation");
}

FSMState* CowerTransition::GetNewState()
{
    return new Cower();
}

Talk::Talk()
{
    Actions.Add(new SpeakToCrowd());
    Transitions.Add(new CowerTransition());
    Transitions.Add(new DespawnTransition());
    Transitions.Add(new WanderTransition());
}

bool TalkCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState* TalkTransition::GetNewState()
{
    return new Talk();
}

Wander::Wander()
{
    Actions.Add(new GotoCrowd());
    Transitions.Add(new CowerTransition());
    Transitions.Add(new DespawnTransition());
    Transitions.Add(new TalkTransition());
}

bool WanderCondition::TestCondition(AFSMController* Controller)
{
    return false;
}

FSMState* WanderTransition::GetNewState()
{
    return new Wander();
}
