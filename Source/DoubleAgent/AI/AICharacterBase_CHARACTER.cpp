// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterBase_CHARACTER.h"

#include "AIController.h"

// Sets default values
AAICharacterBase_CHARACTER::AAICharacterBase_CHARACTER()
{

}

void AAICharacterBase_CHARACTER::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location = GetMesh()->GetSocketLocation(FName("headSocket"));
	Rotation = GetMesh()->GetSocketTransform(FName("headSocket"), RTS_World).Rotator();
}

