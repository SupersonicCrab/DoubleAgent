// Please don't steal

#include "AICharacterBase_CHARACTER.h"
#include "AIController.h"
#include "NPC/AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"

void AAICharacterBase_CHARACTER::QueueSpeech(ESpeechEvent Speech, float TimeToWait)
{
}

void AAICharacterBase_CHARACTER::Speak(ESpeechEvent NewSpeech)
{
}

void AAICharacterBase_CHARACTER::DisableNPC()
{
	AAIControllerBase* NPCController = Cast<AAIControllerBase>(GetController());
	NPCController->BrainComponent->StopLogic(FString("DisablingNPCBehaviour"));
	NPCController->bPerceptionEnabled = false;
}

void AAICharacterBase_CHARACTER::EnableNPC()
{
	AAIControllerBase* NPCController = Cast<AAIControllerBase>(GetController());
	NPCController->BrainComponent->StartLogic();
	NPCController->bPerceptionEnabled = true;
}

void AAICharacterBase_CHARACTER::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location = GetMesh()->GetSocketLocation(FName("headSocket"));
	Rotation = GetMesh()->GetSocketTransform(FName("headSocket"), RTS_World).Rotator();
}

