// Please don't steal

#include "AICharacterBase_CHARACTER.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NPC/AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"

void AAICharacterBase_CHARACTER::QueueSpeech(ESpeechEvent Speech, float TimeToWait)
{
	FTimerDelegate QueueDelegate;
	QueueDelegate.BindUFunction(this, FName("Speak"), Speech);
	FTimerHandle QueueHandle;
	GetWorldTimerManager().SetTimer(QueueHandle, QueueDelegate, TimeToWait, false);
	QueuedSpeech = Speech;
}

void AAICharacterBase_CHARACTER::Speak(ESpeechEvent NewSpeech)
{
	UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	ABaseCharacter_CHARACTER* Speaker = Cast<AAICharacterBase_CHARACTER>(Blackboard->GetValueAsObject("Speaker"));
	
	//Is no one speaking
	if (!IsValid(Speaker))
		Super::Speak(NewSpeech);
	
	//If someone is speaking
	else
	{
		//If new speech event greater than current
		if (NewSpeech > Speaker->GetCurrentSpeechEvent())
		{
			StopSpeaking();
			Super::Speak(NewSpeech);
		}
		else
			QueueSpeech(NewSpeech, Speaker->GetSpeechTimeRemaining() + UKismetMathLibrary::RandomFloatInRange(0, 2));
	}
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

