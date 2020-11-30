// Please don't steal


#include "ButlerAI.h"

bool AButlerAI::HandleHearing(AActor* CurrentActor, FAIStimulus& CurrentStimulus)
{
    if (Super::HandleHearing(CurrentActor, CurrentStimulus))
    {
        //Movement
        if (CurrentStimulus.Tag == "Movement" && !CurrentActor->IsA(AAICharacterBase_CHARACTER::StaticClass()))
        {
            Blackboard->SetValueAsVector("NoiseLocation", CurrentStimulus.StimulusLocation);
        }

        //Something was perceived
        return true;
    }

    //Nothing could be perceived
    return false;
}
