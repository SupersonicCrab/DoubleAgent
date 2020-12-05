// Please don't steal

#include "Actions.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


bool DeleteSelf::PerformAction(AFSMController* Controller)
{
    return false;
}

bool CowerAnimation::PerformAction(AFSMController* Controller)
{
    //Update blackboard
    UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
    Blackboard->SetValueAsFloat("Detection", 100);
    Blackboard->ClearValue("LoudNoiseLocation");
    
    //Load animation
    UAnimSequence* AnimationToPlay = LoadObject<UAnimSequence>(NULL, TEXT("AnimSequence'/Game/Animations/Sequences/Crouched/A_Crouch_Idle.A_Crouch_Idle'"));

    //If animation wasn't found
    if (AnimationToPlay == NULL)
        return false;

    //Play animation as dynamic montage
    UAnimInstance* AnimInstance = Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance();
    if (!AnimInstance->IsAnyMontagePlaying())
        AnimInstance->PlaySlotAnimationAsDynamicMontage(AnimationToPlay, FName("DefaultSlot"));
    return true;
}

bool SpeakToCrowd::PerformAction(AFSMController* Controller)
{
    return false;
}

bool GotoCrowd::PerformAction(AFSMController* Controller)
{
    return false;
}

bool GoToDespawn::PerformAction(AFSMController* Controller)
{
    UEnvQuery* Query = LoadObject<UEnvQuery>(NULL, TEXT("'EnvQuery'/Content/AI/EQS/Queries/FindCivlianDespawn_EQSQUERY.FindCivlianDespawn_EQSQUERY'"));

    FEnvQueryRequest QueryRequest = FEnvQueryRequest(Query, Controller);
    
    return false;
}
