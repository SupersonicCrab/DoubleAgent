// Please don't steal

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABBTree.h"
#include "DialogueParser.generated.h"

USTRUCT(BlueprintType)
struct FDialogueInfo{
	GENERATED_BODY()

	FDialogueInfo();
	
	//This holds the display name of the person who is saying the dialogue line
	UPROPERTY(BlueprintReadWrite)
	FString Speaker = "";
	//This holds the content of the dialogue line that is being spoken
	UPROPERTY(BlueprintReadWrite)
	FString Message = "";
	//This holds the length of the dialogue line in seconds (Determined by the audio lines)
	UPROPERTY(BlueprintReadWrite)
	float MessageLength = 0.0f;
	//This determines whether or not the line can be said when censored lines are turned on
	UPROPERTY(BlueprintReadWrite)
	bool CensoredLine = false;
};
 
class DialogueParser{
public:
	DialogueParser(){};

	FDialogueInfo GetDialogueInfo(FString DialogueKey);
};
