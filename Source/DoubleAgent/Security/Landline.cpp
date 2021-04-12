// Please don't steal


#include "Landline.h"
#include "BackupSpawnpoint.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALandline::ALandline()
{
    //Setup mesh
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Security/Landline/SM_Landline.SM_Landline'"));
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMesh->SetStaticMesh(Mesh);
    RootComponent = StaticMesh;
}

bool ALandline::CallBackup(){
    if(!bPowerOn){ //If the power isn't on, then no backup can be called
        return false;
    }
    if(!bFunctional){ //If the landline isn't functional, then no backup can be called
        return false;
    }
    TArray<AActor*> tempArray;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABackupSpawnpoint::StaticClass(), tempArray); //Get all of the spawnpoints for enemy backup in the world
    for(auto sp : tempArray){
        dynamic_cast<ABackupSpawnpoint*>(sp)->SpawnGuard(); //For every spawnpoint, call the spawn guard function
    }
    tempArray.Empty(); //Empty the array to re-use it
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandline::StaticClass(), tempArray); //Get all the other landlines in the world
    for(auto L : tempArray){
        dynamic_cast<ALandline*>(L)->bFunctional = false; //Mark them all as non-functional so that backup can only be called once
    }
    return true;
}

void ALandline::EnableLandline(){
    bPowerOn = true;
}

void ALandline::DisableLandline(){
    bPowerOn = false;
}