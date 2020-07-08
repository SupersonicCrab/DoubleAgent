// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterBase_CHARACTER_CPP.h"

// Sets default values
AAICharacterBase_CHARACTER_CPP::AAICharacterBase_CHARACTER_CPP()
{
}

void AAICharacterBase_CHARACTER_CPP::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location = GetMesh()->GetSocketLocation(FName("headSocket"));
	Rotation = GetMesh()->GetSocketTransform(FName("headSocket"), RTS_World).Rotator();
}

