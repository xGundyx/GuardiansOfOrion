// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDragDrop.h"


UOrionDragDrop::UOrionDragDrop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FReply UOrionDragDrop::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonDownEvent.IsBound())
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UOrionDragDrop::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonUpEvent.IsBound())
	{
		return OnMouseButtonUpEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}