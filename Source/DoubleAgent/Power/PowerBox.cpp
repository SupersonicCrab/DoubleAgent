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
	//Allow replication
	bReplicates = true;
}

// Called when the game starts or when spawned
void APowerBox::BeginPlay(){
	Super::BeginPlay();
	AActor* temphub = UGameplayStatics::GetActorOfClass(GetWorld(), ARadioHub::StaticClass()); // Get the radio hub actor
	RadioHub = dynamic_cast<ARadioHub*>(temphub); //Cast the radio hub into a radio hub class to store it properly
	temphub = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraHub::StaticClass()); //Get the camera hub actor
	CameraHub = dynamic_cast<ACameraHub*>(temphub); //Cast the camera hub into a camera hub class to store it properly
}

void APowerBox::GetLifetimeReplicatedProps(::TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APowerBox, bLightsOn);
	DOREPLIFETIME(APowerBox, bLandlinesOn);
	DOREPLIFETIME(APowerBox, bRadiosOn);
	DOREPLIFETIME(APowerBox, bCamerasOn);
	DOREPLIFETIME(APowerBox, bPowerCut);
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
	bLightsOn = true;
	tempArray.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = true;
		dynamic_cast<ALightSwitch*>(Switch)->RestoreLights();
	}
}

void APowerBox::TurnLightsOff_Implementation(){
	bLightsOn = false;
	tempArray.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALightSwitch::StaticClass(), tempArray);
	for(auto Switch: tempArray){
		dynamic_cast<ALightSwitch*>(Switch)->DisableLightGroup(true);
		dynamic_cast<ALightSwitch*>(Switch)->bPowerOn = false;
	}
}

void APowerBox::TurnCamerasOn(){
	bCamerasOn = true;
	CameraHub->PowerEnabled();
}

void APowerBox::TurnCamerasOff(){
	bCamerasOn = false;
	CameraHub->PowerDisabled();
}

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

void APowerBox::TurnRadiosOn(){
	RadioHub->PowerOn();
	bRadiosOn = true;
}

void APowerBox::TurnRadiosOff(){
	RadioHub->PowerOff();
	bRadiosOn = false;
}

void APowerBox::CutPower(){
	bPowerCut = true;
}

bool APowerBox::TurnAllPowerOn(){
	if(bPowerCut){
		return false;
	}
	TurnLightsOn();
	TurnCamerasOn();
	TurnLandlinesOn();
	TurnRadiosOn();
	bLightsOn = true;
	bCamerasOn = true;
	bLandlinesOn = true;
	bRadiosOn = true;
	return true;
}