// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PanelWidget.h"
#include "OrionDragDrop.generated.h"

/**
 * 
 */
UCLASS()
class ORION_API UOrionDragDrop : public UPanelWidget
{
	GENERATED_BODY()
public:
	UOrionDragDrop(const FObjectInitializer& ObejctInitializer);

	UPROPERTY(EditDefaultsOnly, Category = Events)
		FOnPointerEvent OnMouseButtonDownEvent;

	UPROPERTY(EditDefaultsOnly, Category = Events)
		FOnPointerEvent OnMouseButtonUpEvent;
	
protected:
	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
};
