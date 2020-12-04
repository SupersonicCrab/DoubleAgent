// Please don't steal

#include "Actions.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"

bool DeleteSelf::PerformAction(AFSMController* Controller)
{
    return false;
}

bool CowerAnimation::PerformAction(AFSMController* Controller)
{
    //Load animation
    UAnimSequence* AnimationToPlay = LoadObject<UAnimSequence>(nullptr, TEXT("Game/Animations/Sequences/Crouched/A_Crouch_Idle"));

    //If animation wasn't found
    if (AnimationToPlay == nullptr)
        return false;

    //Play animation as dynamic montage
    Cast<ACharacter>(Controller->GetPawn())->GetMesh()->GetAnimInstance()->PlaySlotAnimationAsDynamicMontage(AnimationToPlay, FName("Default"));
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
