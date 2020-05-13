// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacterBase_CHARACTER_CPP.generated.h"

UCLASS()
class PROJECTSPY_API AAICharacterBase_CHARACTER_CPP : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase_CHARACTER_CPP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override base functions
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
};
