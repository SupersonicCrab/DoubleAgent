// Please don't steal

#include "AICharacterBase_CHARACTER.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NPC/AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"

void AAICharacterBase_CHARACTER::NetRequestSpeak(ESpeechEvent NewSpeech)
{
	ABaseCharacter_CHARACTER* Speaker = GetNearbySpeaker();
	
	//Is no one speaking
	if (Speaker == nullptr)
		Super::NetRequestSpeak(NewSpeech);

	//If new speech event greater than current
	else if (NewSpeech > Speaker->CurrentSpeechEvent)
	{
		StopSpeaking();
		Super::NetRequestSpeak(NewSpeech);
	}
}

ABaseCharacter_CHARACTER* AAICharacterBase_CHARACTER::GetNearbySpeaker()
{
	TArray<AActor*> Characters;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> ignoreActors;
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), Cast<AAIControllerBase>(GetController())->HearingRange, traceObjectTypes, ABaseCharacter_CHARACTER::StaticClass(), ignoreActors, Characters);
	
	for (int i = 0; i < Characters.Num(); i++)
	{
		if (Cast<ABaseCharacter_CHARACTER>(Characters[i])->GetSpeechTimeRemaining() > 0)
			return Cast<ABaseCharacter_CHARACTER>(Characters[i]);
	}

	return nullptr;
}

void AAICharacterBase_CHARACTER::DisableNPC()
{
	//If client
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
		return;
		
	AAIControllerBase* NPCController = Cast<AAIControllerBase>(GetController());
	NPCController->BrainComponent->StopLogic(FString("DisablingNPCBehaviour"));
	NPCController->bPerceptionEnabled = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	UAIBlueprintHelperLibrary::GetAIController(this)->ClearFocus(EAIFocusPriority::Default);
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

