// Please don't steal

#include "LightSwitch.h"
#include "HouseLight.h"
#include "../AI/RoomVolume.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ALightSwitch::ALightSwitch(){
    //Allow replication
    bReplicates = true;

    //Setup base mesh
    UStaticMesh* Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/LightSwitch/SM_LightSwitchBase.SM_LightSwitchBase'"));
    MeshBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
    MeshBase->SetStaticMesh(Mesh);
    RootComponent = MeshBase;

    //Setup switch mesh
    Mesh = LoadObject<UStaticMesh>(NULL, TEXT("StaticMesh'/Game/Art/Meshes/Building/LightSwitch/SM_LightSwitch.SM_LightSwitch'"));
    MeshSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Switch"));
    MeshSwitch->SetStaticMesh(Mesh);
    MeshSwitch->SetIsReplicated(true);
    MeshSwitch->SetupAttachment(RootComponent);
}

void ALightSwitch::RestoreLights_Implementation(){
    if(bLightGroupOn){
        EnableLightGroup();
    }else{
        DisableLightGroup(false);
    }
}

void ALightSwitch::EnableLightGroup_Implementation(){
    for(auto Light: Lights){ //Loops through all the lights in the light group and turn them on
        Light->TurnOn();
    }
    bLightGroupOn = true;
    AssociatedRoom->UpdateLight(true); //Update the rooms understanding of the light
    MulticastPlaySound(0.9f);

    MeshSwitch->SetRelativeRotation(FRotator(0, 0, 0), false);
}

void ALightSwitch::DisableLightGroup_Implementation(bool bFromPowerBox){ //Check comments for EnableLightGroup(), it's just that but reversed
    for(auto Light: Lights){
        Light->TurnOff();
    }
    if(!bFromPowerBox){
        bLightGroupOn = false;
    }
    AssociatedRoom->UpdateLight(false);
    MulticastPlaySound(1.0f);

    MeshSwitch->SetRelativeRotation(FRotator(0, 0, 180), false);
}

void ALightSwitch::BeginPlay(){
    Super::BeginPlay();

    //Update room volume of lightswitch
    AssociatedRoom->LightSwitch = this;

    //Update power box of light status
    dynamic_cast<APowerBox*>(UGameplayStatics::GetActorOfClass(GetWorld(), APowerBox::StaticClass()))->bLightsOn;
}

void ALightSwitch::MulticastPlaySound_Implementation(float Pitch)
{
    //Play sound
    USoundBase* Sound = LoadObject<USoundBase>(NULL, TEXT("SoundCue'/Game/Audio/SoundEffects/LightSwitch/Lightswitch_Cue.uasset.Lightswitch_Cue'"));
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), FRotator(), 1, Pitch, 0, nullptr, nullptr, this);
}

void ALightSwitch::FindRoomAndLights()
{
    TArray<FOverlapResult> Results;

    //Find room if needed
    if (AssociatedRoom == nullptr)
    {
        //Get camera collisions around light switch
        GetWorld()->OverlapMultiByChannel(Results, GetActorLocation(),   FQuat(GetActorRotation()), ECollisionChannel::ECC_Camera, FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams());

        //Iterate through results
        for (int i = 0; i < Results.Num(); i++)
        {
            ARoomVolume* temp = Cast<ARoomVolume>(Results[i].Actor);
            if (temp != nullptr)
            {
                AssociatedRoom = temp;
                break;
            }
        }

        //If no room was found
        if (AssociatedRoom == nullptr)
            return;
    }
    
    //Find any lights in that room
    TArray<AActor*> AllLights;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouseLight::StaticClass(), AllLights);

    //Iterate through all lights
    for (int i = 0; i < AllLights.Num(); i++)
    {
        Results.Empty();
        //Get camera collisions near lights
        GetWorld()->OverlapMultiByChannel(Results, AllLights[i]->GetActorLocation(),   FQuat(AllLights[i]->GetActorRotation()), ECollisionChannel::ECC_Camera, FCollisionShape::MakeSphere(10.0f), FCollisionQueryParams());

        //Iterate through results
        for (int a = 0; a < Results.Num(); a++)
        {
            AHouseLight* temp = Cast<AHouseLight>(AllLights[i]);
            if (Results[a].Actor == AssociatedRoom && Lights.Find(temp) == -1)
            {
                Lights.Add(temp);
                break;
            }
        }
    }
}