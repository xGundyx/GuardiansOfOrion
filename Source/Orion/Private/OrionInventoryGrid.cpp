// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionInventoryGrid.h"

AOrionInventoryGrid::AOrionInventoryGrid(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bOnlyRelevantToOwner = true;
	bReplicates = true;
}

void AOrionInventoryGrid::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionInventoryGrid, Inventory);
	DOREPLIFETIME(AOrionInventoryGrid, InventoryType);
	DOREPLIFETIME(AOrionInventoryGrid, Height);
	DOREPLIFETIME(AOrionInventoryGrid, Width);
}

//waiting for epic to fix rep notify of tarrays
void AOrionInventoryGrid::OnRep_Inventory()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwner());

	if (PC)
	{
		PC->EventRedrawInventory();
	}
}
