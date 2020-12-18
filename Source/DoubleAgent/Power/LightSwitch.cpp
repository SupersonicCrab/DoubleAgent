// Please don't steal


#include "LightSwitch.h"
#include "HouseLight.h"
#include "../AI/RoomVolume.h"
#include "DoubleAgent/AI/AICharacterBase_CHARACTER.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


// Sets default values
ALightSwitch::ALightSwitch(){
    //Allow replication
    bReplicates = true;
}

void ALightSwitch::MulticastEnableLights_Implementation(AHouseLight* Light){
    Light->TurnOn();
}

void ALightSwitch::MulticastDisableLights_Implementation(AHouseLight* Light){
    Light->TurnOff();
}

void ALightSwitch::RestoreLights_Implementation(){
    if(bLightGroupOn){
        EnableLightGroup();
    }else{
        DisableLightGroup(false);
    }
}

void ALightSwitch::EnableLightGroup_Implementation(){
    TArray<AActor*> overlapping; //Stores NPCs that are within the light collision
    for(auto Light: Lights){ //Loops through all the lights in the light group and turn them on
        MulticastEnableLights(Light);
    }
    bLightGroupOn = true;
    AssociatedRoom->UpdateLight(true); //Update the rooms understanding of the light
    for(auto Light: Lights){ //For every light
        Light->Sphere->GetOverlappingActors(overlapping, AAICharacterBase_CHARACTER::StaticClass()); //Check for any overlapping actors of type ABaseCharacter_CHARACTER
        for(auto NPC: overlapping){ //For every NPC we overlap
            if(AssociatedRoom->IsOverlappingActor(NPC)){ //Check if the room is overlapping the actor
                Light->CheckNPC(NPC); //Call the light NPC check
            }
        }
    }

   PlaySound(0.9f);
}

void ALightSwitch::DisableLightGroup_Implementation(bool bFromPowerBox){ //Check comments for EnableLightGroup(), it's just that but reversed
    TArray<AActor*> overlapping;
    for(auto Light: Lights){
        MulticastDisableLights(Light);
    }
    if(!bFromPowerBox){
        bLightGroupOn = false;
    }
    AssociatedRoom->UpdateLight(false);
    for(auto Light: Lights){
        Light->Sphere->GetOverlappingActors(overlapping, AAICharacterBase_CHARACTER::StaticClass());
        for(auto NPC: overlapping){
            if(AssociatedRoom->IsOverlappingActor(NPC)){
                Light->CheckNPC(NPC);
            }
        }
    }

    PlaySound(1.0f);
}

void ALightSwitch::BeginPlay(){
    Super::BeginPlay();
    FTimerHandle DelayHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayHandle, this, &ALightSwitch::HandleAssociatedRoom, 2.0f, false);
    TArray<AActor*> tempArray;
    UWorld* world = GetWorld();
    
    PowerBox = dynamic_cast<APowerBox*>(UGameplayStatics::GetActorOfClass(world, APowerBox::StaticClass()));
    bPowerOn = PowerBox->bLightsOn;
    UGameplayStatics::GetAllActorsOfClass(world, AHouseLight::StaticClass(), tempArray);
    
    for(int i = 0; i < tempArray.Num(); i++){
        if(dynamic_cast<AHouseLight*>(tempArray[i])->LightGroup == LightGroup){
            Lights.Add(dynamic_cast<AHouseLight*>(tempArray[i]));
        }
    }
}

void ALightSwitch::PlaySound_Implementation(float Pitch)
{
    //Play sound
    USoundBase* Sound = LoadObject<USoundBase>(NULL, TEXT("SoundCue'/Game/Audio/Lightswitch_Cue.Lightswitch_Cue'"));
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), FRotator(), 1, Pitch, 0, nullptr, nullptr, this);

}

void ALightSwitch::Tick(float DeltaTime){
    
}

void ALightSwitch::Interact_Implementation(AActor* Interactor){
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Interacted, pog")));
}


template<typename T>
void FindAllActors(UWorld* World, TArray<T>& Out){
    for(TActorIterator<T> It(World); It; ++It){
        Out.Add(*It);
    }
}

void ALightSwitch::HandleAssociatedRoom_Implementation(){
    AssociatedRoom->LightSwitch = this;
    for(auto Light : Lights){
        Light->Room = AssociatedRoom;
    }
}
//Needed to replicate the light array
void ALightSwitch::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const{ 
    DOREPLIFETIME( ALightSwitch, Lights); 
}