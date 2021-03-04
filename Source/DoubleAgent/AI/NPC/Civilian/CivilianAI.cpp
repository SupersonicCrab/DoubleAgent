// Please don't steal

#include "CivilianAI.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "DoubleAgent/AI/NPC/FSM/GeneralStates.h"
#include "Perception/AIPerceptionComponent.h"

ACivilianAI::ACivilianAI()
{
    CurrentState = new Wander();
    BehaviourFPS = 3;
}

void ACivilianAI::HandleSight(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    Super::HandleSight(CurrentActor, CurrentStimulus);

    //UnconsciousNPC
    UObject* UnconsciousNPC = Blackboard->GetValueAsObject("UnconsciousNPC");
    if (IsValid(UnconsciousNPC) && !CurrentStimulus.WasSuccessfullySensed() && CurrentActor->IsA(AAICharacterBase_CHARACTER::StaticClass()))
    {
        if (UnconsciousNPC == CurrentActor)
        {
            Blackboard->ClearValue("UnconsciousNPC");
        }
    }
}
