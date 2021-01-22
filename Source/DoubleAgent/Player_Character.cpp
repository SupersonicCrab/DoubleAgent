// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Character.h"
#include "AI/RoomVolume.h"
#include "Engine/DemoNetDriver.h"

APlayer_Character::APlayer_Character()
{
	//Register overlap events
	OnActorBeginOverlap.AddDynamic(this, &APlayer_Character::CheckRooms);
	OnActorEndOverlap.AddDynamic(this, &APlayer_Character::CheckRooms);
}

void APlayer_Character::CheckRooms(class AActor* OverlappedActor, class AActor* OtherActor)
{
	//If server
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		//Get overlapping room volumes
		TArray<AActor*> RoomVolumes;
		GetOverlappingActors(RoomVolumes, ARoomVolume::StaticClass());

		bool Temp = true;		
		for (int i = 0; i < RoomVolumes.Num(); i++)
		{
			if (Cast<ARoomVolume>(RoomVolumes[i])->bPublic)
				Temp = false;
		}

		bTresspassing = Temp;
	}
}

void APlayer_Character::PerformIllegalAction_Implementation(float TimeToRemove)
{
	bIllegalAction = true;

	if (TimeToRemove == 0 )
		return;
	
	//If timer is already active
	if (UKismetSystemLibrary::K2_IsTimerActiveHandle(GetWorld(), IllegalActionTimer))
		GetWorld()->GetTimerManager().SetTimer(IllegalActionTimer, this, &APlayer_Character::FinishIllegalAction, GetWorld()->GetTimerManager().GetTimerRemaining(IllegalActionTimer)+1.0f, false);
	else
		GetWorld()->GetTimerManager().SetTimer(IllegalActionTimer, this, &APlayer_Character::FinishIllegalAction, TimeToRemove, false);
}

void APlayer_Character::FinishIllegalAction()
{
	bIllegalAction = false;
}

void APlayer_Character::BeginPlay()
{
	Super::BeginPlay();
	
	CheckRooms(nullptr, nullptr);
}

void APlayer_Character::GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayer_Character, bTresspassing);
	DOREPLIFETIME(APlayer_Character, bIllegalAction);
	DOREPLIFETIME(APlayer_Character, bIllegalEquipment);
}
