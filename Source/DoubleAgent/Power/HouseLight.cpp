// Please don't steal


#include "HouseLight.h"

// Sets default values
AHouseLight::AHouseLight(){
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//This is constructing and setting the default varaibles for the light component of the object
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	Light->Intensity = 10000;
	Light->Mobility = EComponentMobility::Stationary;
	Light->SetVisibility(true);
	RootComponent = Light;

	//This is constructing and attaching the collision component to the light component
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetVisibility(false);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AHouseLight::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AHouseLight::BeginPlay(){
	Super::BeginPlay();
	Sphere->AttachToComponent(Light, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Sphere->SetSphereRadius(Light->AttenuationRadius*1.4);
}

// Called every frame
void AHouseLight::Tick(float DeltaTime){
	Super::Tick(DeltaTime);
}

void AHouseLight::TurnOff(){
	Light->SetVisibility(false);
}

void AHouseLight::TurnOn(){
	Light->SetVisibility(true);
}

void AHouseLight::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult){
	if(OtherActor->GetClass()->GetSuperClass()->GetName() == "BP_AICharacter_Base" && !Room->bRoomLit && !OtherActor->IsOverlappingActor(Room)){
		CheckNPC(OtherActor);
	}
}

void AHouseLight::CheckNPC(AActor* NPC){
	FHitResult hit;
	FCollisionQueryParams Params;
	GetWorld()->LineTraceSingleByChannel(hit, GetActorLocation(), NPC->GetActorLocation(), ECC_Visibility, Params);

	if(hit.Actor == NPC){
		Room->UpdateNPC(NPC);
	}
}