// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "BaseCharacter_CHARACTER.generated.h"

UCLASS()
class DOUBLEAGENT_API ABaseCharacter_CHARACTER : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter_CHARACTER();

	//Override base function to add socket locations to raycast
	UFUNCTION(BlueprintCallable)
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = nullptr) const override;
};
