// Please don't steal


#include "Radio.h"

FRadioEvent::FRadioEvent()
{
}

FRadioEvent::FRadioEvent(ERadioEvent RadioEvent_, FVector Location_, float Delay_, AAICharacterBase_CHARACTER* NPC_)
{
	RadioEvent = RadioEvent_;
	Location = Location_;
	Delay = Delay_;
	NPC = NPC_;
}

// Sets default values
ARadio::ARadio()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ARadio::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARadio::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

