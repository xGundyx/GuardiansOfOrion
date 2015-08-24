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

	void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = Placer)
		TArray<AOrionMenuCharacter*> CharacterPlacers;

	UFUNCTION(BlueprintCallable, Category = Placer)
		void AddPlacer(int32 index, AOrionMenuCharacter *Picker);
};
