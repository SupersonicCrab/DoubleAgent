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
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere, Replicated)
	bool bTresspassing;
	
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere, Replicated)
	bool bIllegalAction;
	UPROPERTY(BlueprintReadWrite, Category = "Default", EditAnywhere, Replicated)
	bool bIllegalEquipment;

	UFUNCTION()
	void CheckRooms(class AActor* OverlappedActor = nullptr, class AActor* OtherActor = nullptr);

	FTimerHandle IllegalActionTimer;

	//If TimeToRemove is set to 0 the state will not disappear
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PerformIllegalAction(float TimeToRemove = 1.0f);

	UFUNCTION()
	void FinishIllegalAction();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsThreat()
	{
		return (bTresspassing || bIllegalAction || bIllegalEquipment);
	};
	
	//Overrides
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
