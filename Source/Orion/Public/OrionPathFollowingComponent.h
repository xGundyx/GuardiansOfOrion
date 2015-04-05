// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/PathFollowingComponent.h"
#include "OrionAIController.h"
#include "OrionPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()
	
protected:
	/** cached UCharacterMovementComponent */
	UPROPERTY(transient)
		UCharacterMovementComponent* CharacterMoveComp;

public:
	/** follow current path segment */
	virtual void FollowPathSegment(float DeltaTime) override;

	// used to detect properties of a path's segment a character is about to follow
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;

	// used to cache UCharacterMovementComponent we're using in SetMoveSegment implementation
	virtual void SetMovementComponent(UNavMovementComponent* MoveComp) override;

	AOrionAIController *Controller;

	void DoRotation();

	bool HandleRotation();

	FTimerHandle RotationTimer;
};
