

#include "Orion.h"
#include "OrionOctTree.h"

FVector UOrionOctTree::TreeOffsets[8];

UOrionOctTree::UOrionOctTree(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	Density = -100.0;

	Children[0] = nullptr;
	mesh = nullptr;
}

void UOrionOctTree::Init()
{
	TreeOffsets[0] = FVector(0.25, 0.25, -0.25);
	TreeOffsets[1] = FVector(0.25, -0.25, -0.25);
	TreeOffsets[2] = FVector(-0.25, -0.25, -0.25);
	TreeOffsets[3] = FVector(-0.25, 0.25, -0.25);
	TreeOffsets[4] = FVector(0.25, 0.25, 0.25);
	TreeOffsets[5] = FVector(0.25, -0.25, 0.25);
	TreeOffsets[6] = FVector(-0.25, -0.25, 0.25);
	TreeOffsets[7] = FVector(-0.25, 0.25, 0.25);
}

void UOrionOctTree::FillIn(FVector pos)
{
	FVector c = FVector(-0.5, -0.5, -0.5)*VOXEL_WIDTH*CHUNK_SIZE_XY*VOXEL_X;

	if (Density <= -99.9)
	{
		float g = 0.0;

		g = UOrionDiamondSquare::HeightValues[int((pos - c).X / VOXEL_WIDTH)][int((pos - c).Y / VOXEL_WIDTH)] / (UOrionDiamondSquare::MaxHeight*1.0 + 0.05);
		Density = float(FMath::Max(-1.0, FMath::Min(1.0, ((((g + 1.0) / 2.0) * (CHUNK_SIZE_XY * VOXEL_Z)) - ((pos - c).Z / VOXEL_WIDTH)))));
	}
	else
		return;

	if (Children[0] != nullptr)
	{
		for (int32 i = 0; i < 8; i++)
		{
			Children[i]->FillIn(pos + TreeOffsets[i] * Width);
		}
	}
}

void UOrionOctTree::InsertPoint(FVector pos, float den, FVector center, float width)
{
	if (den == -1.0 || den == 1.0)
		return;
	//if(den == Density)
	//	return;

	//1 and -1 are just fillers, no adding nodes allowed!
	/*if (den == 1.0 || den == -1.0)
	{
	if (Density <= -99.9)
	{
	Density = den;
	}
	else if (den != Density)
	{
	if (width == VOXEL_WIDTH)
	{
	Density = den;
	}

	for (int32 i = 0; i < 8; i++)
	{
	if (Children[i] != nullptr)
	Children[i]->InsertPoint(pos, den, center + (TreeOffsets[i] * width), width / 2.0);
	}
	}
	}
	else
	{*/
	//if we get to the end with our data, set it up!
	if (width == VOXEL_WIDTH)
	{
		Density = den;
	}
	else
	{
		if (Children[0] == nullptr)
		{
			/*if (den == -1.0 || den == 1.0)
			{
			Density = den;
			return;
			}*/

			//if (width == VOXEL_X * VOXEL_WIDTH)
			//	DrawDebugBox(GWorld, center, FVector(width, width, width)*0.5, FColor(FMath::FRandRange(0, 255), FMath::FRandRange(0, 255), FMath::FRandRange(0, 255), 255), true);

			for (int32 i = 0; i < 8; i++)
			{
				Children[i] = ConstructObject<UOrionOctTree>(UOrionOctTree::StaticClass());
				Children[i]->Density = -100.0;
				//Children[i]->Position = center + (TreeOffsets[i] * width);
				Children[i]->Width = width / 2.0;
			}
		}

		//go down a level
		Children[GetChild(pos, center)]->InsertPoint(pos, den, center + (TreeOffsets[GetChild(pos, center)] * width), width / 2.0);
		//Children[GetChild(Position, center)]->InsertPoint(Position, Density, center + (TreeOffsets[GetChild(pos, center)] * width), width / 2.0);
	}
	//}
}

int32 UOrionOctTree::GetChild(FVector pos, FVector center)
{
	if (pos.X >= center.X)
	{
		if (pos.Y >= center.Y)
		{
			if (pos.Z >= center.Z)
				return 4;
			else
				return 0;
		}
		else
		{
			if (pos.Z >= center.Z)
				return 5;
			else
				return 1;
		}
	}
	else
	{
		if (pos.Y >= center.Y)
		{
			if (pos.Z >= center.Z)
				return 7;
			else
				return 3;
		}
		else
		{
			if (pos.Z >= center.Z)
				return 6;
			else
				return 2;
		}
	}
	return 0;
}

float UOrionOctTree::GetData(FVector pos, FVector center, int32 LOD)
{
	if (Width == VOXEL_WIDTH * LOD)
		return Density;

	if (Children[0] != nullptr)
	{
		return Children[GetChild(pos, center)]->GetData(pos, center + (TreeOffsets[GetChild(pos, center)] * Width), LOD);
	}
	else
		return Density;
}

void UOrionOctTree::RemovePoint(FVector pos)
{

}


