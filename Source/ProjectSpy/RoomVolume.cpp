// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume.h"
#include "AICharacterBase_CHARACTER.h"
#include "Player_Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

ARoomVolume::ARoomVolume()
{
	//Register overlap events
	OnActorBeginOverlap.AddDynamic(this, &ARoomVolume::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ARoomVolume::OnOverlapEnd);
}

void ARoomVolume::BeginPlay()
{
	Super::BeginPlay();

	//Call a timer with a half a second delay as circumvent the beginplay execution order
	GetWorld()->GetTimerManager().SetTimer(delayHandle, this, &ARoomVolume::UpdateOverlappingActors, 0.5f, false);
}

void ARoomVolume::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	//Update player
	if (OtherActor->IsA(APlayer_Character::StaticClass()))
	{
		Cast<APlayer_Character>(OtherActor)->roomSafe = playerSafe;
	}

	//Update NPC
	else if (OtherActor->GetClass()->IsChildOf(AAICharacterBase_CHARACTER::StaticClass()))
	{
		UpdateNPC(OtherActor);
	}
}

void ARoomVolume::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	//Update player
	if (OtherActor->IsA(APlayer_Character::StaticClass()))
	{
		Cast<APlayer_Character>(OtherActor)->roomSafe = false;
	}

	//Update NPC
	else if (OtherActor->GetClass()->IsChildOf(AAICharacterBase_CHARACTER::StaticClass()))
	{
		UpdateNPC(OtherActor);
	}
}

void ARoomVolume::UpdateOverlappingActors()
{
	//Get all actors of type base character
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors, ABaseCharacter_CHARACTER::StaticClass());

	for (int a = 0; a < Actors.Num(); a++)
	{
		//Update player
		if (Actors[a]->IsA(APlayer_Character::StaticClass()))
		{
			Cast<APlayer_Character>(Actors[a])->roomSafe = playerSafe;
		}

		//Update NPC
		else if (Actors[a]->GetClass()->IsChildOf(AAICharacterBase_CHARACTER::StaticClass()))
		{
			UpdateNPC(Actors[a]);
		}
	}
}

void ARoomVolume::UpdateLight(bool roomLit_)
{
	roomLit = roomLit_;
	UpdateOverlappingActors();
}

void ARoomVolume::UpdateNPC(AActor* NPC)
{
	//Get NPC blackboard
	UBlackboardComponent* NPCBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(NPC);

	if (roomLit)
	{
		//Removing light switch from NPC if room is now lit
		ALightSwitch* NPCLightSwitch = Cast<ALightSwitch>(NPCBlackboard->GetValueAsObject("LightSwitch"));
		if (NPCLightSwitch != nullptr && lightSwitch == NPCLightSwitch)
		{
			NPCBlackboard->ClearValue("LightSwitch");
		}
	}
	else
	{
		//Setting light switch if room is no longer lit
		if (NPCBlackboard->GetValueAsObject("LightSwitch") == nullptr && lightSwitch)
		{
			NPCBlackboard->SetValueAsObject("LightSwitch",	lightSwitch);
		}
	}
}
