#pragma once

#include "OrionGameMode.h"
#include "OrionMenuCharacter.h"
#include "OrionGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API AOrionGameMenu : public AOrionGameMode
{
	GENERATED_BODY()
	
public:
	AOrionGameMenu(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(BlueprintReadWrite, Category = Placer)
		TArray<AOrionMenuCharacter*> CharacterPlacers;
};
