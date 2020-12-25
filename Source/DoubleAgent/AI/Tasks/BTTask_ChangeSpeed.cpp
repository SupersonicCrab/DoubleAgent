// Please don't steal


#include "BTTask_ChangeSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_ChangeSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UCharacterMovementComponent* CharacterMovement = Cast<ACharacter>(Cast<AAIController>(OwnerComp.GetOwner())->GetPawn())->GetCharacterMovement();
    
    switch (NewSpeed)
    {
        //Change speed to walking
        case EMoveSpeed::Speed_Walk:
            CharacterMovement->MaxWalkSpeed = 130.0f;
        break;

        //Change speed to jogging
        case EMoveSpeed::Speed_Jog:
            CharacterMovement->MaxWalkSpeed = 345.0f;
        break;

        //Change speed to running
        case EMoveSpeed::Speed_Run:
            CharacterMovement->MaxWalkSpeed = 630.0f;
        break;
    }
     
    return EBTNodeResult::Succeeded;
}
