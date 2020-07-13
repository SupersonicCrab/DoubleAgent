// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter_CHARACTER_CPP.h"
#include "Player_Character_CPP.generated.h"

UCLASS()
class PROJECTSPY_API APlayer_Character_CPP : public ABaseCharacter_CHARACTER_CPP
{
	GENERATED_BODY()

public:
	//Constructor
	APlayer_Character_CPP();

	//Variables
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere = "True")
	bool roomSafe;
};
