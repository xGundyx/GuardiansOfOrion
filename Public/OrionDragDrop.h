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
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Events)
		FOnPointerEvent OnMouseButtonDownEvent;

	UPROPERTY(EditDefaultsOnly, Category = Events)
		FOnPointerEvent OnMouseButtonUpEvent;
	
protected:
	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
};
