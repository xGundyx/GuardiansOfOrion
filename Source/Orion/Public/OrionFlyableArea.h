// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Volume.h"
#include <unordered_map>
#include <string>
#include "OrionFlyableArea.generated.h"

struct FFlyableOctree
{
	FFlyableOctree *Children[8];
	FFlyableOctree *Parent;

	FVector TreeCenter;
	FVector TreeExtent;

	bool bFlyable;

	std::string Locator;

	static FVector TOffsets[8];

	FFlyableOctree()
	{
		for (int32 i = 0; i < 8; i++)
			Children[i] = nullptr;

		bFlyable = false;

		Parent = nullptr;
	}

	~FFlyableOctree()
	{
		for (int32 i = 0; i < 8; i++)
		{
			if (Children[i])
			{
				delete Children[i];
				Children[i] = nullptr;
			}
		}
	}

	FVector GetRandomPoint()
	{
		if (bFlyable)
			return TreeCenter;

		TArray<int32> Indices;

		for (int32 i = 0; i < 8; i++)
		{
			if (Children[i])
				Indices.Add(i);
		}

		if (Indices.Num() < 1)
			return TreeCenter;

		return Children[Indices[FMath::RandRange(0,Indices.Num()-1)]]->GetRandomPoint();
	}

	FFlyableOctree *GetRoot()
	{
		FFlyableOctree *Current = this;
		while (Current->Parent)
		{
			Current = Current->Parent;
		}

		return Current;
	}

	TArray<FFlyableOctree*> GetNeighborsFromLookup(std::string InverseLookup, int32 sides[4])
	{
		TArray<FFlyableOctree*> Neighbors;

		//build the path to our neighbor from the lookup
		std::string path;

		path = Locator.substr(0, Locator.length() - InverseLookup.length());
		for (size_t i = 0; i < InverseLookup.length(); i++)
		{
			path += InverseLookup.substr(InverseLookup.length() - 1 - i, 1);
		}

		//now that we have our path, trace down and see what we get!
		FFlyableOctree *Current = GetRoot();
		size_t Counter = 1;
		while (path.length() > Counter)
		{
			std::string num = path.substr(Counter, 1);
			int32 index = atoi(num.c_str());
			Counter++;

			if (Current->Children[index])
			{
				Current = Current->Children[index];
			}
			else
				break;
		}

		if (Current)
			Current->GetFlyableNodes(Neighbors, sides);

		return Neighbors;
	}

	void GetFlyableNodes(TArray<FFlyableOctree*> &Nodes, int32 sides[4])
	{
		if (bFlyable)
		{
			if (Nodes.Find(this) == INDEX_NONE)
				Nodes.Add(this);
		}
		else
		{
			for (int32 i = 0; i < 8; i++)
			{
				if (i == sides[0] || i == sides[1] || i == sides[2] || i == sides[3])
				{
					if (Children[i] != nullptr)
						Children[i]->GetFlyableNodes(Nodes, sides);
				}
			}
		}
	}

	TArray<FFlyableOctree*> GetNeighbors()
	{
		TArray<FFlyableOctree*> Neighbors;

		for (int32 i = 0; i < 6; i++)
		{
			size_t counter = 1;
			//using our Locator, find us some neighbors
			int32 a = atoi(Locator.substr(Locator.length() - counter, 1).c_str());
			counter++;
			//int32 b = atoi(Locator.substr(Locator.length() - 2, 1).c_str());

			int32 aa;
			//int32 bb;
			std::string InverseLookup;

			if (i == 0 || i == 1)
				aa = (3 - (a % 4)) % 4 + (a > 3 ? 4 : 0);
			else if (i == 2 || i == 3)
				aa = a + (a % 2 == 0 ? 1 : -1);
			else
				aa = a > 3 ? a - 4 : a + 4;

			InverseLookup = std::to_string(aa);

			while (true)
			{
				bool bIncrease = true;
				if(i == 0)
					bIncrease = aa % 4 < 2;
				else if (i == 1)
					bIncrease = aa % 4 >= 2;
				else if (i == 2)
					bIncrease = aa % 4 == 0 || aa % 4 == 3;
				else if (i == 3)
					bIncrease = aa % 4 == 1 || aa % 4 == 2;
				else if (i == 4)
					bIncrease = aa > 3;
				else if (i == 5)
					bIncrease = aa <= 3;

				if (bIncrease)
				{
					//bb = b;
					//InverseLookup += std::to_string(bb);
					break;
				}
				else
				{
					if (counter > Locator.length())
						break;

					a = atoi(Locator.substr(Locator.length() - counter, 1).c_str());
					counter++;

					if (i == 0 || i == 1)
						aa = (3 - (a % 4)) % 4 + (a > 3 ? 4 : 0);
					else if (i == 2 || i == 3)
						aa = a + (a % 2 == 0 ? 1 : -1);
					else
						aa = a > 3 ? a - 4 : a + 4;

					InverseLookup += std::to_string(aa);
				}
			}

			int32 sides[4]; 

			if (i == 0)
			{
				sides[0] = 2; sides[1] = 3; sides[2] = 6; sides[3] = 7;
			}
			else if (i == 1)
			{
				sides[0] = 0; sides[1] = 1; sides[2] = 4; sides[3] = 5;
			}
			else if (i == 2)
			{
				sides[0] = 1; sides[1] = 2; sides[2] = 5; sides[3] = 6;
			}
			else if (i == 3)
			{
				sides[0] = 0; sides[1] = 3; sides[2] = 4; sides[3] = 7;
			}
			else if (i == 4)
			{
				sides[0] = 0; sides[1] = 1; sides[2] = 2; sides[3] = 3;
			}
			else if (i == 5)
			{
				sides[0] = 4; sides[1] = 5; sides[2] = 6; sides[3] = 7;
			}
			Neighbors += GetNeighborsFromLookup(InverseLookup, sides);
		}

		return Neighbors;
	}

	void InsertCube(FVector Center, float Width)
	{
		if (Center == TreeCenter)
		{
			bFlyable = true;
			return;
		}

		int32 Index = GetChild(Center);

		if (Children[Index] == nullptr)
		{
			//for (int32 i = 0; i < 8; i++)
			{
				Children[Index] = new FFlyableOctree();
				Children[Index]->TreeCenter = TreeCenter + TOffsets[Index] * TreeExtent;
				Children[Index]->TreeExtent = TreeExtent / 2.0f;
				Children[Index]->bFlyable = false;
				Children[Index]->Locator = Locator + std::to_string(Index);
			}
		}
		//else
		//{
		Children[Index]->InsertCube(Center, Width);// Center + (TOffsets[GetChild(Center)] * Width), Width / 2.0);
		//}
	}

	//given a point, find it
	FFlyableOctree *GetNodeForPoint(FVector point)
	{
		//only pay attention to flyable nodes
		if (bFlyable)
		{
			return this;
		}
		else
		{
			if (Children[GetChild(point)])
			{
				FFlyableOctree *Child = Children[GetChild(point)]->GetNodeForPoint(point);

				//if the point lies outside of the node returned, it means it is outside of the mesh, so return null!
				if (point.X < TreeCenter.X - TreeExtent.X ||
					point.X > TreeCenter.X + TreeExtent.X ||
					point.Y < TreeCenter.Y - TreeExtent.Y ||
					point.Y > TreeCenter.Y + TreeExtent.Y ||
					point.Z < TreeCenter.Z - TreeExtent.Z ||
					point.Z > TreeCenter.Z + TreeExtent.Z)
					return nullptr;

				return Child;
			}
		}

		return nullptr;
	}

	int32 GetChild(FVector Center)
	{
		if (Center.X >= TreeCenter.X)
		{
			if (Center.Y >= TreeCenter.Y)
			{
				if (Center.Z >= TreeCenter.Z)
					return 4;
				else
					return 0;
			}
			else
			{
				if (Center.Z >= TreeCenter.Z)
					return 5;
				else
					return 1;
			}
		}
		else
		{
			if (Center.Y >= TreeCenter.Y)
			{
				if (Center.Z >= TreeCenter.Z)
					return 7;
				else
					return 3;
			}
			else
			{
				if (Center.Z >= TreeCenter.Z)
					return 6;
				else
					return 2;
			}
		}
		return 0;
	}
};

struct FNavNode
{
	FFlyableOctree *Node;

	float F;
	float G;

	int32 Parent;

	FNavNode()
	{
		F = 0.0f;
		G = 0.0f;

		Node = nullptr;
		Parent = -1;
	}

	FORCEINLINE void operator=(const FNavNode& B)
	{
		Node = B.Node;
		F = B.F;
		G = B.G;
		Parent = B.Parent;
	}
};

FORCEINLINE bool operator==(const FNavNode& A, const FNavNode& B)
{
	return A.Node == B.Node;
}

FORCEINLINE bool operator< (const FNavNode& A, const FNavNode& B)
{
	return A.F < B.F;
}

FORCEINLINE bool operator> (const FNavNode& A, const FNavNode& B)
{
	return A.F > B.F;
}

inline static bool NodePredicate(const FNavNode& A, const FNavNode& B)
{
	return (A.F < B.F);
}

/**
 * 
 */
UCLASS()
class ORION_API AOrionFlyableArea : public AVolume
{
	GENERATED_BODY()
	
public:
	AOrionFlyableArea(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = Navigation)
		void BuildFlightPaths();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Terrain)
		bool RebuildFlightPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Terrain)
		bool ShowBoxes;

private:
	void AddCube(FVector Center, float Width);
	void DoTrace(FVector Center, float Width);

	FFlyableOctree* Data;

	FVector VolumeCenter;
	FVector VolumeExtent;

	void DrawDebugBoxes(FFlyableOctree *Data);
	void Optimize(FFlyableOctree *pData);

	void FillInParents(FFlyableOctree *pData);

	void SmoothPath(TArray<FVector> &FullPath);

public:
	TArray<FVector> FindPath(FVector Start, FVector End);
	FVector GetRandomPoint();
	bool IsInit();
};
