// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionFlyableArea.h"

FVector FFlyableOctree::TOffsets[8];

#if WITH_EDITOR
void AOrionFlyableArea::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	FName PropertyName = PropertyThatChanged != NULL ? PropertyThatChanged->GetFName() : NAME_None;

	if (PropertyName == FName(TEXT("RebuildFlightPaths")))
	{
		RebuildFlightPaths = false;
		BuildFlightPaths();
	}
	else if (PropertyName == FName(TEXT("ShowBoxes")))
	{
		FlushPersistentDebugLines(GetWorld());

		if (Data && ShowBoxes)
			DrawDebugBoxes(Data);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FVector AOrionFlyableArea::GetRandomPoint()
{
	if (Data)
		return Data->GetRandomPoint();

	return FVector(0.0f);
}

//this is the heavy lifter
void AOrionFlyableArea::BuildFlightPaths()
{
	if (Data)
	{
		delete Data;
		Data = nullptr;
		//Data.clear();
	}

	Data = new FFlyableOctree;
	Data->Locator = std::to_string(0);

	FlushPersistentDebugLines(GetWorld());

	VolumeCenter = GetBrushComponent()->Bounds.Origin;//->GetActorBounds(false, Center, Extent);
	VolumeExtent = GetBrushComponent()->Bounds.BoxExtent;

	//convert our box to a size that is suitable for an octree (power of two)
	float width = FMath::Max(VolumeExtent.Y, VolumeExtent.Z);
	width = FMath::Max(VolumeExtent.X, width) * 2.0f;

	int32 size = 1;

	FFlyableOctree::TOffsets[0] = FVector(0.25, 0.25, -0.25);
	FFlyableOctree::TOffsets[1] = FVector(0.25, -0.25, -0.25);
	FFlyableOctree::TOffsets[2] = FVector(-0.25, -0.25, -0.25);
	FFlyableOctree::TOffsets[3] = FVector(-0.25, 0.25, -0.25);
	FFlyableOctree::TOffsets[4] = FVector(0.25, 0.25, 0.25);
	FFlyableOctree::TOffsets[5] = FVector(0.25, -0.25, 0.25);
	FFlyableOctree::TOffsets[6] = FVector(-0.25, -0.25, 0.25);
	FFlyableOctree::TOffsets[7] = FVector(-0.25, 0.25, 0.25);

	while (true)
	{
		size *= 2;

		if (size >= width)
		{
			width = float(size);
			break;
		}
	}

	//if we have some left over data, delete it!
	//if (Data)
	//	delete Data;

	//Data = ConstructObject<FFlyableOctree>(FFlyableOctree::StaticClass());

	Data->TreeCenter = VolumeCenter;
	Data->TreeExtent = FVector(width);

	DoTrace(VolumeCenter, width);

	//optimize our results (leave off for now)
	//Optimize(Data);

	//parents
	FillInParents(Data);

	if (ShowBoxes)
		DrawDebugBoxes(Data);

	////FindPath(Data->GetRandomPoint(), Data->GetRandomPoint());
}

bool AOrionFlyableArea::IsInit()
{
	return Data != nullptr;
}

void AOrionFlyableArea::DoTrace(FVector Center, float Width)
{
	//if the width is small enough, just add it 
	if (Width <= 512.0f)
	{
		//AddCube(Center, Width);
		return;
	}

	FHitResult Hit;
	FVector start = Center;// +FVector(0, 0, Width);
	FVector end = Center - FVector(0.0f, 0.0f, 5.0f);
	FQuat rot;

	FCollisionQueryParams Params;

	FCollisionObjectQueryParams QueryParams;

	if (GetWorld()->SweepSingle(Hit, start, end, rot, FCollisionShape::MakeBox(FVector(Width)/2.0f)/*FCollisionShape::MakeSphere(Width / 2.0f)*/, Params, QueryParams))
	{
		for (int32 i = 0; i < 8; i++)
		{
			DoTrace(Center + FFlyableOctree::TOffsets[i] * Width, Width / 2.0f);
		}
	}
	else
	{
		//add a box if there's nothing colliding
		AddCube(Center, Width);
	}

	/*FVector FirstCenter = Center - FVector(Extent.X, Extent.Y, -Extent.Z);
	FirstCenter += FVector(1250.0, 1250.0, -1250.0);

	for (int32 x = 0; x < Extent.X * 2.0 / 2500.0; x++)
	{
		for (int32 y = 0; y < Extent.Y * 2.0 / 2500.0; y++)
		{
			for (int32 z = 0; z < Extent.Z * 2.0 / 2500.0; z++)
			{
				FHitResult Hit;
				FVector start = FirstCenter + FVector(2500.0 * x, 2500.0 * y, -2500.0 * (z-1));
				FVector end = start - FVector(0, 0, 2500.0);
				FQuat rot;

				FCollisionQueryParams Params;

				FCollisionObjectQueryParams QueryParams;

				if (GetWorld()->SweepSingle(Hit, start, end, rot, FCollisionShape::MakeSphere(1250.0f), Params, QueryParams))
					DrawDebugBox(GetWorld(), end, FVector(1250.0f), FColor::MakeRandomColor(), true);
				else
					DrawDebugSphere(GetWorld(), end, 1250.0f, 10, FColor::MakeRandomColor(), true);
			}
		}
	}*/
}

void AOrionFlyableArea::AddCube(FVector Center, float Width)
{
	//if this cube is outside of the original volume, ignore it completely
	if (Center.X + Width / 2.0f < VolumeCenter.X - VolumeExtent.X)
		return;
	if (Center.X - Width / 2.0f > VolumeCenter.X + VolumeExtent.X)
		return;
	if (Center.Y + Width / 2.0f < VolumeCenter.Y - VolumeExtent.Y)
		return;
	if (Center.Y - Width / 2.0f > VolumeCenter.Y + VolumeExtent.Y)
		return;
	if (Center.Z + Width / 2.0f < VolumeCenter.Z - VolumeExtent.Z)
		return;
	if (Center.Z - Width / 2.0f > VolumeCenter.Z + VolumeExtent.Z)
		return;

	//DrawDebugBox(GetWorld(), Center, FVector(Width / 2.0f), FColor::MakeRandomColor(), true);

	Data->InsertCube(Center, Width);
}

void AOrionFlyableArea::DrawDebugBoxes(FFlyableOctree *pData)
{
	//cycle through all of our datas and draw the flyable areas
	if (pData->bFlyable)
		DrawDebugBox(GetWorld(), pData->TreeCenter, FVector(pData->TreeExtent / 2.0f), FColor::MakeRandomColor(), true);
	else
	{
		for (int32 i = 0; i < 8; i++)
		{
			if (pData->Children[i])
				DrawDebugBoxes(pData->Children[i]);
		}
	}
}

void AOrionFlyableArea::Optimize(FFlyableOctree *pData)
{
	bool bGroupable = true;

	for (int32 i = 0; i < 4; i++)
	{
		if (!pData->Children[i] || !pData->Children[i]->bFlyable)
		{
			bGroupable = false;
			if (pData->Children[i])
				Optimize(pData->Children[i]);
		}
	}

	if (bGroupable)
	{
		pData->Children[0]->TreeCenter = pData->Children[0]->TreeCenter - FFlyableOctree::TOffsets[0] * pData->Children[0]->TreeExtent * 2.0f;
		pData->Children[0]->TreeExtent *= FVector(2.0f, 2.0f, 1.0f);

		//remove the other pointers
		for (int32 i = 1; i < 4; i++)
		{
			delete pData->Children[i];
			pData->Children[i] = nullptr;
		}
	}

	bGroupable = true;

	for (int32 i = 4; i < 8; i++)
	{
		if (!pData->Children[i] || !pData->Children[i]->bFlyable)
		{
			bGroupable = false;
			if (pData->Children[i])
				Optimize(pData->Children[i]);
		}
	}

	if (bGroupable)
	{
		pData->Children[4]->TreeCenter = (pData->Children[4]->TreeCenter + pData->Children[5]->TreeCenter + pData->Children[6]->TreeCenter + pData->Children[7]->TreeCenter) / 4.0f;
		pData->Children[4]->TreeExtent *= FVector(2.0f, 2.0f, 1.0f);

		//remove the other pointers
		for (int32 i = 5; i < 8; i++)
		{
			delete pData->Children[i];
			pData->Children[i] = nullptr;
		}
	}
}

void AOrionFlyableArea::FillInParents(FFlyableOctree *pData)
{
	//cycle through every node and populate the parent pointers
	for (int32 i = 0; i < 8; i++)
	{
		if (pData->Children[i])
		{
			//check for null boxes and remove them completely
			bool bDelete = !pData->Children[i]->bFlyable;
			for (int32 j = 0; j < 8; j++)
			{
				if (pData->Children[i]->Children[j])
					bDelete = false;
			}

			if (bDelete)
			{
				delete pData->Children[i];
				pData->Children[i] = nullptr;
			}
			else
			{
				pData->Children[i]->Parent = pData;
				FillInParents(pData->Children[i]);
			}
		}
	}
}

TArray<FVector> AOrionFlyableArea::FindPath(FVector Start, FVector End)
{
	FlushPersistentDebugLines(GetWorld());

	TArray<FVector> FullPath;

	//find a quick and dirty path with A*
	FFlyableOctree *StartNode = Data->GetNodeForPoint(Start);
	FFlyableOctree *EndNode = Data->GetNodeForPoint(End);

	if (StartNode == nullptr)
	{
		int32 x = 1;
		while (!StartNode && Start.Z + 250.0f * x < Data->TreeCenter.Z + Data->TreeExtent.Z)
		{
			StartNode = Data->GetNodeForPoint(Start + FVector(0, 0, 250.0f * x));
			x++;
		}
	}

	if (EndNode == nullptr)
	{
		int32 x = 1;
		while (!EndNode && End.Z + 250.0f * x < Data->TreeCenter.Z + Data->TreeExtent.Z)
		{
			EndNode = Data->GetNodeForPoint(End + FVector(0, 0, 250.0f * x));
			x++;
		}
	}

	if (StartNode == nullptr || EndNode == nullptr)
		return FullPath;

	//DrawDebugSphere(GetWorld(), StartNode->TreeCenter, 500.0, 12, FColor::MakeRandomColor(), true);
	//DrawDebugSphere(GetWorld(), EndNode->TreeCenter, 500.0, 12, FColor::MakeRandomColor(), true);

	TArray<FNavNode> OpenList;
	FNavNode MyNode;
	MyNode.F = 0.0f;
	MyNode.G = 0.0f;
	MyNode.Node = StartNode;
	MyNode.Parent = -1;

	OpenList.Add(MyNode);

	TArray<FNavNode> ClosedList;

	while (OpenList.Num() > 0)
	{
		//find the shortest path in our list
		//OpenList.Sort();

		FNavNode Shortest = OpenList[0];
		for (int32 i = 1; i < OpenList.Num(); i++)
		{
			if (OpenList[i].F < Shortest.F)
				Shortest = OpenList[i];
		}

		FNavNode Current = Shortest;// OpenList[0];

		if (Current.Node == EndNode)
		{
			//path is complete!
			break;
		}

		OpenList.Remove(Current);
		//Current.Parent = ClosedList.Num() - 1;
		ClosedList.Add(Current);

		//get a list of all neighbors to current
		TArray<FFlyableOctree*> Neighbors = Current.Node->GetNeighbors();
		Neighbors.Remove(Current.Node);

		for (int32 i = 0; i < Neighbors.Num(); i++)
		{
			FNavNode NewNode;
			NewNode.Node = Neighbors[i];

			//if we're already in the open list, update us
			int32 index = OpenList.Find(NewNode);
			if (index != INDEX_NONE)
			{
				NewNode.G = Current.G + (NewNode.Node->TreeCenter - Current.Node->TreeCenter).Size();
				if (NewNode.G < OpenList[index].G)
				{
					OpenList[index].G = NewNode.G;
					OpenList[index].F = NewNode.G + (OpenList[index].Node->TreeCenter - EndNode->TreeCenter).Size();
					OpenList[index].Parent = ClosedList.Num() - 1;
				}
			}
			else if (ClosedList.Find(NewNode) != INDEX_NONE)
			{
				
			}
			else
			{
				//the distance to get from the start to this node
				NewNode.G = Current.G + (NewNode.Node->TreeCenter - Current.Node->TreeCenter).Size();
				//the distance to get to this node plus the estimated distance still to go
				NewNode.F = NewNode.G + (NewNode.Node->TreeCenter - EndNode->TreeCenter).Size();

				NewNode.Parent = ClosedList.Num() - 1;

				//add us to the open list
				OpenList.Add(NewNode);
			}
		}
	}

	//draw our path!
	if (ClosedList.Num() > 0)
	{
		int32 ParentIndex = ClosedList.Num() - 1;// ClosedList[ClosedList.Num() - 1].Parent;
		FVector pos = EndNode->TreeCenter;
		FullPath.Insert(pos, 0);
		while (ParentIndex > -1)
		{
			////DrawDebugLine(GetWorld(), pos, ClosedList[ParentIndex].Node->TreeCenter, FColor::MakeRandomColor(), true);
			pos = ClosedList[ParentIndex].Node->TreeCenter;
			FullPath.Insert(pos, 0);
			ParentIndex = ClosedList[ParentIndex].Parent;
		}
	}

	//the path we have now is pretty jagged and jerky, smooth it out!
	SmoothPath(FullPath);

	/*if (FullPath.Num() > 0)
	{
		DrawDebugLine(GetWorld(), Start, FullPath[0], FColor::MakeRandomColor(), true);
		for (int32 i = 1; i < FullPath.Num(); i++)
		{
			DrawDebugLine(GetWorld(), FullPath[i-1], FullPath[i], FColor::MakeRandomColor(), true);
		}
	}*/

	//if (ClosedList.Num() > 0)
	//	DrawDebugLine(GetWorld(), ClosedList[ClosedList.Num() - 1].Node->TreeCenter, EndNode->TreeCenter, FColor::MakeRandomColor(), true);

	return FullPath;
}

void AOrionFlyableArea::SmoothPath(TArray<FVector> &FullPath)
{
	if (FullPath.Num() <= 0)
		return;

	TArray<FVector> RealPath;

	RealPath.Add(FullPath[0]);

	int32 i = 0;
	int32 j = 0;
	while (i < FullPath.Num() - 2)
	{
		for (j = i + 2; j < FullPath.Num() && j - i < 5; j++)
		{
			FHitResult Hit;
			FVector start = FullPath[i];
			FVector end = FullPath[j];
			FQuat rot;

			FCollisionQueryParams Params;

			FCollisionObjectQueryParams QueryParams;

			if (GetWorld()->SweepSingle(Hit, start, end, rot, FCollisionShape::MakeSphere(25.0f), Params, QueryParams))
			{
				RealPath.Add(FullPath[j - 1]);
				break;
			}
			else if (j - 1 >= 5)
				RealPath.Add(FullPath[j - 1]);
		}
		i = j - 1;
	}

	//make sure the end is the real end
	if (RealPath.Num() > 0)
	{
		if (RealPath.Last() != FullPath.Last())
			RealPath.Add(FullPath.Last());
	}

	FullPath = RealPath;
}

