// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter_CHARACTER.h"
#include "AICharacterBase_CHARACTER.generated.h"

UCLASS()
class PROJECTSPY_API AAICharacterBase_CHARACTER : public ABaseCharacter_CHARACTER
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase_CHARACTER();

	// Override base function to set NPC vision to headSocket
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
};
