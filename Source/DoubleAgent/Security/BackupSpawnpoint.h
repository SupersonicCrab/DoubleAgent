// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "BackupSpawnpoint.generated.h"

UCLASS()
class DOUBLEAGENT_API ABackupSpawnpoint : public AActor{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABackupSpawnpoint();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacter> Backup;
	
	void SpawnGuard();
};