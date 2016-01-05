// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionLoadingWidget.h"

void UOrionLoadingWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	//don't remove from level on map change, requires care not to 
	/*if (InLevel == nullptr && InWorld == GetWorld())
	{
		RemoveFromParent();
		MarkPendingKill();
	}*/
}


