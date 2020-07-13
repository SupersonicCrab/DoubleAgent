// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume_BP_CPP.h"
#include "AICharacterBase_CHARACTER_CPP.h"
#include "Player_Character_CPP.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

ARoomVolume_BP_CPP::ARoomVolume_BP_CPP()
{
	//Register overlap events
	OnActorBeginOverlap.AddDynamic(this, &ARoomVolume_BP_CPP::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ARoomVolume_BP_CPP::OnOverlapEnd);
}

void ARoomVolume_BP_CPP::BeginPlay()
{
	Super::BeginPlay();

	//Call a timer with a half a second delay as circumvent the beginplay execution order
	GetWorld()->GetTimerManager().SetTimer(delayHandle, this, &ARoomVolume_BP_CPP::UpdateOverlappingActors, 0.5f, false);
}

void ARoomVolume_BP_CPP::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	//Update player
	if (OtherActor->IsA(APlayer_Character_CPP::StaticClass()))
	{
		Cast<APlayer_Character_CPP>(OtherActor)->roomSafe = playerSafe;
	}

	//Update NPC
	else if (OtherActor->GetClass()->IsChildOf(AAICharacterBase_CHARACTER_CPP::StaticClass()))
	{
		UpdateNPC(OtherActor);
	}
}

void ARoomVolume_BP_CPP::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	//Update player
	if (OtherActor->IsA(APlayer_Character_CPP::StaticClass()))
	{
		Cast<APlayer_Character_CPP>(OtherActor)->roomSafe = false;
	}

	//Update NPC
	else if (OtherActor->GetClass()->IsChildOf(AAICharacterBase_CHARACTER_CPP::StaticClass()))
	{
		UpdateNPC(OtherActor);
	}
}

void ARoomVolume_BP_CPP::UpdateOverlappingActors()
{
	//Get all actors of type base character
	TArray<AActor*> Actors;
	GetOverlappingActors(Actors, ABaseCharacter_CHARACTER_CPP::StaticClass());

	for (int a = 0; a < Actors.Num(); a++)
	{
		//Update player
		if (Actors[a]->IsA(APlayer_Character_CPP::StaticClass()))
		{
			Cast<APlayer_Character_CPP>(Actors[a])->roomSafe = playerSafe;
		}

		//Update NPC
		else if (Actors[a]->GetClass()->IsChildOf(AAICharacterBase_CHARACTER_CPP::StaticClass()))
		{
			UpdateNPC(Actors[a]);
		}
	}
}

void ARoomVolume_BP_CPP::UpdateLight(bool roomLit_)
{
	roomLit = roomLit_;
	UpdateOverlappingActors();
}

void ARoomVolume_BP_CPP::UpdateNPC(AActor* NPC)
{
	//Get NPC blackboard
	UBlackboardComponent* NPCBlackboard = UAIBlueprintHelperLibrary::GetBlackboard(NPC);

	if (roomLit)
	{
		//Removing light switch from NPC if room is now lit
		ALightSwitch_BP_CPP* NPCLightSwitch = Cast<ALightSwitch_BP_CPP>(NPCBlackboard->GetValueAsObject("LightSwitch"));
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
