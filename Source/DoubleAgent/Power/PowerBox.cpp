// Please don't steal


#include "PowerBox.h"
#include "Kismet/GameplayStatics.h"
#include "LightSwitch.h"
#include "Blueprint/UserWidget.h"
#include "DoubleAgent/Player_Character.h"
#include "Engine/Light.h"

// Sets default values
APowerBox::APowerBox(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

void APowerBox::TurnLightsOn(){
	bLightsOn = true;
	TArray<AActor*> tempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = true;
		dynamic_cast<ALightSwitch*>(Switch)->RestoreLights();
	}
}

void APowerBox::TurnLightsOff(){
	bLightsOn = false;
	TArray<AActor*> tempArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->DisableLightGroup(true);
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = false;
	}
}

// void APowerBox::TurnCamerasOn(){
// 	
// }
//
// void APowerBox::TurnCamerasOff(){
// 	
// }
//
// void APowerBox::TurnLandlinesOn(){
// 	
// }
//
// void APowerBox::TurnLandlinesOff(){
// 	
// }
//
// void APowerBox::TurnRadiosOn(){
// 	
// }
//
// void APowerBox::TurnRadiosOff(){
// 	
// }

void APowerBox::CutPower(){
	bPowerCut = true;
}
