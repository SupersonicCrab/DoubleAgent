// Please don't steal


#include "PowerBox.h"
#include "Kismet/GameplayStatics.h"
#include "LightSwitch.h"
#include "Blueprint/UserWidget.h"
#include "DoubleAgent/Player_Character.h"
#include "DoubleAgent/Security/Landline.h"
#include "Engine/Light.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APowerBox::APowerBox(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Allow replication
	bReplicates = true;
}

// Called when the game starts or when spawned
void APowerBox::BeginPlay(){
	Super::BeginPlay();
}

// Called every frame
void APowerBox::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

void APowerBox::Interact_Implementation(AActor* Interactor){
	APlayer_Character* character = dynamic_cast<APlayer_Character*>(Interactor);
	if(character){
	}	
}

void APowerBox::TurnLightsOn_Implementation(){
	MulticastLightsOn();
}

void APowerBox::TurnLightsOff_Implementation(){
	MulticastLightsOff();
}

// void APowerBox::TurnCamerasOn(){
// 	
// }
//
// void APowerBox::TurnCamerasOff(){
// 	
// }

void APowerBox::TurnLandlinesOn(){
	TArray<AActor*> temporaryArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandline::StaticClass(), temporaryArray);

	for(auto L: temporaryArray){
		dynamic_cast<ALandline*>(L)->EnableLandline();
	}
	bLandlinesOn = true;
}

void APowerBox::TurnLandlinesOff(){
	TArray<AActor*> temporaryArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALandline::StaticClass(), temporaryArray);

	for(auto L: temporaryArray){
		dynamic_cast<ALandline*>(L)->DisableLandline();
	}
	bLandlinesOn = false;
}

// void APowerBox::TurnRadiosOn(){
// 	
// }
//
// void APowerBox::TurnRadiosOff(){
// 	
// }

void APowerBox::MulticastLightsOn_Implementation(){
	bLightsOn = true;
	tempArray.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = true;
		dynamic_cast<ALightSwitch*>(Switch)->RestoreLights();
	}
}

void APowerBox::MulticastLightsOff_Implementation(){
	bLightsOn = false;
	tempArray.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->DisableLightGroup(true);
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = false;
	}
}

void APowerBox::CutPower(){
	bPowerCut = true;
}

void APowerBox::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const{ 
    DOREPLIFETIME( APowerBox, tempArray); 
}