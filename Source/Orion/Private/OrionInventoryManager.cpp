

#include "Orion.h"
#include "OrionInventoryManager.h"
#include "OrionInventoryArmor.h"


AOrionInventoryManager::AOrionInventoryManager(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void AOrionInventoryManager::SwapItems(UOrionInventoryGrid *theGrid1, int32 x1, int32 y1, UOrionInventoryGrid *theGrid2, int32 x2, int32 y2)
{
	AOrionInventory *tempInv = theGrid1->Column[x1].Row[y1];
	UObject *tempItem = theGrid1->Icons[x1 * theGrid1->MaxWidth + y1];

	theGrid1->Column[x1].Row[y1] = theGrid2->Column[x2].Row[y2];
	theGrid2->Column[x2].Row[y2] = tempInv;

	theGrid1->Icons[x1 * theGrid1->MaxWidth + y1] = theGrid2->Icons[x2 * theGrid2->MaxWidth + y2];
	theGrid2->Icons[x2 * theGrid2->MaxWidth + y2] = tempItem;

	//check for any equipment changes
	EquipItems(Cast<AOrionCharacter>(OwnerController->GetPawn()));
}

AOrionInventory *AOrionInventoryManager::GetItemAt(UOrionInventoryGrid *theGrid, int32 x, int32 y)
{
	if (x >= theGrid->Column.Num() || y >= theGrid->Column[x].Row.Num())
		return NULL;

	return theGrid->Column[x].Row[y];
}

//this will equip all items based on what's in the inventory
void AOrionInventoryManager::EquipItems(AOrionCharacter *aPawn)
{
	AOrionArmor *Armor = Cast<AOrionArmor>(HelmetSlot->Column[0].Row[0]);
	if (Armor)
		aPawn->SetHelmetMesh(Armor->Mesh);
	else
		aPawn->SetHelmetMesh(NULL);
}

void AOrionInventoryManager::Init()
{
	Grid = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (Grid)
		Grid->CreateInventory(5, 5, 96, 1);

	HelmetSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (HelmetSlot)
		HelmetSlot->CreateInventory(1, 1, 96, 1);

	HandsSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (HandsSlot)
		HandsSlot->CreateInventory(1, 1, 96, 1);

	BodySlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (BodySlot)
		BodySlot->CreateInventory(1, 1, 96, 1);

	LegsSlot = ConstructObject<UOrionInventoryGrid>(UOrionInventoryGrid::StaticClass());
	if (LegsSlot)
		LegsSlot->CreateInventory(1, 1, 96, 1);
}

//returns index we were added at, -1 means failure
int32 AOrionInventoryManager::AddItemToInventory(UOrionInventoryGrid *theGrid, AOrionInventory* newItem)
{
	for (int32 i = 0; i < theGrid->MaxHeight; i++)
	{
		for (int32 j = 0; j < theGrid->MaxWidth; j++)
		{
			if (theGrid->Column[i].Row[j] == NULL)
			{
				theGrid->Column[i].Row[j] = newItem;
				return i * theGrid->MaxWidth + j;
			}
		}
	}

	newItem->Destroy();
	newItem = NULL;
	return -1;
}

void AOrionInventoryManager::RemoveItemFromInventory(UOrionInventoryGrid *theGrid, int32 x, int32 y)
{
	if (theGrid->Column.Num() > y)
	{
		if (theGrid->Column[y].Row.Num() > x)
		{
			theGrid->Column[y].Row[x] = NULL;
		}
	}
}
