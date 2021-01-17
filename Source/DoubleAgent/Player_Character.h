// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter_CHARACTER.h"
#include "Player_Character.generated.h"

UCLASS()
class DOUBLEAGENT_API APlayer_Character : public ABaseCharacter_CHARACTER
{
	GENERATED_BODY()

public:
	//Constructor
	APlayer_Character();

	//Variables
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere)
	bool bTresspassing;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere)
	bool bIllegalAction;
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere)
	bool bIllegalEquipment;

	UFUNCTION()
	void CheckRooms(class AActor* OverlappedActor = nullptr, class AActor* OtherActor = nullptr);

	FTimerHandle IllegalActionTimer;
	
	UFUNCTION(BlueprintCallable)
	void PerformIllegalAction();

	UFUNCTION()
	void FinishIllegalAction();
	
	//Overrides
	virtual void BeginPlay() override;
};
