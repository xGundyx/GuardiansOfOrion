

#include "Orion.h"
#include "OrionDiamondSquare.h"

float UOrionDiamondSquare::HeightValues[int(DIAMOND_RES) + 1][int(DIAMOND_RES) + 1];
float UOrionDiamondSquare::MaxHeight = 0.0f;

UOrionDiamondSquare::UOrionDiamondSquare(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
}

void UOrionDiamondSquare::Init()
{
	for (int32 i = 0; i < DIAMOND_RES + 1; i++)
	{
		for (int32 j = 0; j < DIAMOND_RES + 1; j++)
			HeightValues[i][j] = -100.0;
	}
}

//seed some values to control a few features (high mountains around the edges
void UOrionDiamondSquare::SeedValues(int32 x, int32 y, int32 Resolution)
{
	float newMax;

	if (Resolution % 2 == 1)
		return;

	/*if (x != 0 && y != 0 && x != DIAMOND_RES && y != DIAMOND_RES)
	{
	SeedValues(x, y, Resolution / 2);
	SeedValues(x + Resolution / 2, y, Resolution / 2);
	SeedValues(x + Resolution / 2, y + Resolution / 2, Resolution / 2);
	SeedValues(x, y + Resolution / 2, Resolution / 2);

	return;
	}*/

	if (Resolution == DIAMOND_RES)
	{
		newMax = HeightValues[x][y] = FMath::FRand()*0.5 + 0.5;
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x][y + Resolution] = FMath::FRand()*0.5 + 0.5;
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x + Resolution][y + Resolution] = FMath::FRand()*0.5 + 0.5;
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x + Resolution][y] = FMath::FRand()*0.5 + 0.5;
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x + Resolution / 2][y] = FMath::FRand();
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x + Resolution][y + Resolution / 2] = FMath::FRand();
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x + Resolution / 2][y + Resolution] = FMath::FRand();
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);

		newMax = HeightValues[x][y + Resolution / 2] = FMath::FRand();
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);
	}

	int32 counter = 2;

	while (counter < DIAMOND_RES)
	{
		for (int32 i = 0; i < counter; i++)
		{
			//bottom row
			newMax = HeightValues[0][i * Resolution / counter + Resolution / (counter * 2)] = (HeightValues[0][i * Resolution / counter] + HeightValues[0][(i + 1) * Resolution / counter]) / 2.0 + ((FMath::FRand() - 0.5)*2.0) / float(counter);
			if (FMath::Abs(newMax) > MaxHeight)
				MaxHeight = FMath::Abs(newMax);

			//top row
			newMax = HeightValues[int(DIAMOND_RES)][i * Resolution / counter + Resolution / (counter * 2)] = (HeightValues[int(DIAMOND_RES)][i * Resolution / counter] + HeightValues[int(DIAMOND_RES)][(i + 1) * Resolution / counter]) / 2.0 + ((FMath::FRand() - 0.5)*2.0) / float(counter);
			if (FMath::Abs(newMax) > MaxHeight)
				MaxHeight = FMath::Abs(newMax);
		}

		for (int32 i = 0; i < counter; i++)
		{
			//left
			newMax = HeightValues[i * Resolution / counter + Resolution / (counter * 2)][0] = (HeightValues[i * Resolution / counter][0] + HeightValues[(i + 1) * Resolution / counter][0]) / 2.0 + ((FMath::FRand() - 0.5)*2.0) / float(counter);
			if (FMath::Abs(newMax) > MaxHeight)
				MaxHeight = FMath::Abs(newMax);

			//right
			newMax = HeightValues[i * Resolution / counter + Resolution / (counter * 2)][int(DIAMOND_RES)] = (HeightValues[i * Resolution / counter][int(DIAMOND_RES)] + HeightValues[(i + 1) * Resolution / counter][int(DIAMOND_RES)]) / 2.0 + ((FMath::FRand() - 0.5)*2.0) / float(counter);
			if (FMath::Abs(newMax) > MaxHeight)
				MaxHeight = FMath::Abs(newMax);
		}

		counter *= 2;
	}

	//if (Resolution == DIAMOND_RES && x == 0 && y == 0)
	//{
	newMax = HeightValues[int(DIAMOND_RES / 2)][int(DIAMOND_RES / 2)] = -FMath::FRand()*0.5;
	if (FMath::Abs(newMax) > MaxHeight)
		MaxHeight = FMath::Abs(newMax);
	//}
}

void UOrionDiamondSquare::GenerateHeightValues(int32 x, int32 y, int32 Resolution)
{
	if (Resolution % 2 != 0)
		return;

	float newMax;

	//change this to interpolate instead of average

	//find the center of our square
	if (HeightValues[x + Resolution / 2][y + Resolution / 2] < -99.9)
	{
		newMax = HeightValues[x + Resolution / 2][y + Resolution / 2] = (HeightValues[x][y] + HeightValues[x][y + Resolution] + HeightValues[x + Resolution][y + Resolution] + HeightValues[x + Resolution][y]) / 4.0;// +((FMath::FRand() - 0.5)*2.0) * float(Resolution) / DIAMOND_RES, 2.0;
		if (FMath::Abs(newMax) > MaxHeight)
			MaxHeight = FMath::Abs(newMax);
	}

	FVector2D offset[4];

	offset[0] = FVector2D(0.5, 0.0);
	offset[1] = FVector2D(1.0, 0.5);
	offset[2] = FVector2D(0.5, 1.0);
	offset[3] = FVector2D(0.0, 0.5);

	//the 4 diamond points
	for (int32 i = 0; i < 4; i++)
	{
		FVector2D center = FVector2D(x + offset[i].X * Resolution, y + offset[i].Y * Resolution);

		FVector2D offset2[4];

		offset2[0] = FVector2D(0.5, 0.0);
		offset2[1] = FVector2D(0.0, 0.5);
		offset2[2] = FVector2D(-0.5, 0.0);
		offset2[3] = FVector2D(0.0, -0.5);

		if (HeightValues[int(center.X)][int(center.Y)] < -99.9)
		{
			int32 counter = 0;
			float total = 0.0;

			for (int32 j = 0; j < 4; j++)
			{
				if (int(center.X + offset2[j].X * Resolution) > DIAMOND_RES || int(center.X + offset2[j].X * Resolution) < 0)
					continue;

				if (int(center.Y + offset2[j].Y * Resolution) > DIAMOND_RES || int(center.Y + offset2[j].Y * Resolution) < 0)
					continue;

				newMax = HeightValues[int(center.X + offset2[j].X * Resolution)][int(center.Y + offset2[j].Y * Resolution)];

				if (newMax < -99.9)
					continue;

				counter++;
				total += newMax;
			}

			if (counter > 0)
			{
				newMax = HeightValues[int(center.X)][int(center.Y)] = total / float(counter) + ((FMath::FRand() - 0.5)*2.0) * float(Resolution) / DIAMOND_RES;
				if (FMath::Abs(newMax) > MaxHeight)
					MaxHeight = FMath::Abs(newMax);
			}
			else
				HeightValues[int(center.X)][int(center.Y)] = 0.0;
		}
	}

	GenerateHeightValues(x, y, Resolution / 2);
	GenerateHeightValues(x + Resolution / 2, y, Resolution / 2);
	GenerateHeightValues(x + Resolution / 2, y + Resolution / 2, Resolution / 2);
	GenerateHeightValues(x, y + Resolution / 2, Resolution / 2);

	return;

	if (Resolution == DIAMOND_RES)
	{
		for (int32 k = 0; k < 2; k++)
		{
			for (int32 i = 0; i < DIAMOND_RES; i++)
			{
				for (int32 j = 0; j < DIAMOND_RES; j++)
				{
					HeightValues[i][j] = (HeightValues[i][j] + HeightValues[i + 1][j] + HeightValues[i + 1][j + 1] + HeightValues[i][j + 1]) / 4.0;
				}
			}
		}
	}
}





