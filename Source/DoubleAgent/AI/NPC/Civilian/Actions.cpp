// Please don't steal

#include "Actions.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DoubleAgent/BaseCharacter_CHARACTER.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


bool DeleteSelf::PerformAction(AFSMController* Controller)
{
    //If NPC is not moving or animating
    if (Controller->GetMoveStatus() == EPathFollowingStatus::Idle && !Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
    {
        Controller->GetPawn()->Destroy();
        Controller->Destroy();
        return true;
    }
       
    return false;
}

bool CowerAnimation::PerformAction(AFSMController* Controller)
{
    //Stop all movement if any
    Controller->StopMovement();

    //If another animation is already playing
    if (Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
        return false;
    
    //Update blackboard
    UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
    Controller->RaiseDetection(100.0f);
    Blackboard->ClearValue("LoudNoiseLocation");

    //Load animation
    UAnimSequence* AnimationToPlay = LoadObject<UAnimSequence>(
        NULL, TEXT("AnimSequence'/Game/Animations/Sequences/Crouched/A_Crouch_Idle.A_Crouch_Idle'"));

    //If animation wasn't found
    if (AnimationToPlay == NULL)
        return false;

    //Play animation as dynamic montage
    UAnimInstance* AnimInstance = Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance();
    if (!AnimInstance->IsAnyMontagePlaying())
        Cast<ABaseCharacter_CHARACTER>(Controller->GetPawn())->NetPlayAnimation(AnimationToPlay);
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
    //Change characters speed to running
    Cast<ACharacter>(Controller->GetPawn())->GetCharacterMovement()->MaxWalkSpeed = 630;

    //Load eqs query to be run
    UEnvQuery* Query = LoadObject<UEnvQuery>(NULL, TEXT("EnvQuery'/Game/AI/EQS/Queries/FindCivlianDespawn_EQSQUERY.FindCivlianDespawn_EQSQUERY'"));

    //Run eqs behaviour
    Controller->FindEQS(Query);
    return true;
}
