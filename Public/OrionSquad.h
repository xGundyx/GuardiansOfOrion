// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionAIController.h"
#include "OrionSquad.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionSquad : public UObject
{
	GENERATED_BODY()

public:
	UOrionSquad();

	UPROPERTY(BlueprintReadWrite, Category = AI)
		AOrionAIController *Leader;

	void AddSquadMember(AOrionAIController *NewMember);
	void RemoveSquadMember(AOrionAIController *MemberToRemove);
	void FindNewLeader();

	//leader sets the enemy for us
	void SetEnemy(APawn *pEnemy);

	void InitMember(AOrionAIController *NewMember);

	UFUNCTION(BlueprintCallable, Category = Squad)
		FVector GetLeaderDestination();

	UFUNCTION(BlueprintCallable, Category = Squad)
		void SetSquadDestination(FVector NewDestination);

private:
	TArray<AOrionAIController*> Members;
	FVector SquadDestination;
};
