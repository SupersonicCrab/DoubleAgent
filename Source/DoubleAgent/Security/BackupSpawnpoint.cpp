// Please don't steal


#include "BackupSpawnpoint.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABackupSpawnpoint::ABackupSpawnpoint(){
}
void ABackupSpawnpoint::SpawnGuard(){
	FVector loc = GetActorTransform().GetLocation();
	FQuat rot = GetActorTransform().GetRotation();
	FNavLocation output;
	FNavAgentProperties AP;
	FSharedConstNavQueryFilter QueryFilter;
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	bool bOnNavMesh = NavSystem->ProjectPointToNavigation(loc, output, FVector(100,100,100));
	//UNavigationSystemV1::ProjectPointToNavigation(loc, output, FVector(100,100,100));
FTransform trans = UKismetMathLibrary::MakeTransform(output, rot.Rotator(), FVector(1.0f, 1.0f, 1.0f));
	if(!Backup){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("No guard class to spawn selected")));
		return;
	}
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	GetWorld()->SpawnActor<ACharacter>(Backup, trans, spawnParams);
}

