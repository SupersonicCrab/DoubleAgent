// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterBase_CHARACTER_CPP.h"

// Sets default values
AAICharacterBase_CHARACTER_CPP::AAICharacterBase_CHARACTER_CPP()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAICharacterBase_CHARACTER_CPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAICharacterBase_CHARACTER_CPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAICharacterBase_CHARACTER_CPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAICharacterBase_CHARACTER_CPP::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
	Location = GetMesh()->GetSocketLocation(FName("headSocket"));
	Rotation = GetMesh()->GetSocketTransform(FName("headSocket"), RTS_World).Rotator();
}

