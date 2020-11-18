// Please don't steal

#include "AICharacterBase_CHARACTER.h"
#include "AIController.h"
#include "NPC/AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"

void AAICharacterBase_CHARACTER::DisableNPC()
{
	Cast<AAIControllerBase>(GetController())->BrainComponent->StopLogic(FString("NPCTakedown"));
}

void AAICharacterBase_CHARACTER::EnableNPC()
{
	Cast<AAIControllerBase>(GetController())->BrainComponent->StartLogic();
}

void AAICharacterBase_CHARACTER::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location = GetMesh()->GetSocketLocation(FName("headSocket"));
	Rotation = GetMesh()->GetSocketTransform(FName("headSocket"), RTS_World).Rotator();
}

