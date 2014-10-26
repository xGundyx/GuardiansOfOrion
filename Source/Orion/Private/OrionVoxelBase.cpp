

#include "Orion.h"

const int32 X_NOISE_GEN = 1619;
const int32 Y_NOISE_GEN = 31337;
const int32 Z_NOISE_GEN = 6971;
const int32 SEED_NOISE_GEN = 1013;
const int32 SHIFT_NOISE_GEN = 8;

double Lerp(double x, double y, double a)
{
	return x*(1.0 - a) + y*a;
}

int32 IntValueNoise3D(int32 x, int32 y, int32 z, int32 seed)
{
	// All constants are primes and must remain prime in order for this noise
	// function to work correctly.
	int32 n = (
		X_NOISE_GEN    * x
		+ Y_NOISE_GEN    * y
		+ Z_NOISE_GEN    * z
		+ SEED_NOISE_GEN * seed)
		& 0x7fffffff;
	n = (n >> 13) ^ n;
	return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

double ValueNoise3D(int32 x, int32 y, int32 z, int32 seed)
{
	return 1.0 - ((double)IntValueNoise3D(x, y, z, seed) / 1073741824.0);
}

double SCurve3(double a)
{
	return (a * a * (3.0 - 2.0 * a));
}

double SCurve5(double a)
{
	double a3 = a * a * a;
	double a4 = a3 * a;
	double a5 = a4 * a;
	return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
}

double ValueCoherentNoise3D(double x, double y, double z, int32 seed)//, NoiseQuality noiseQuality)
{
	// Create a unit-length cube aligned along an integer boundary.  This cube
	// surrounds the input point.
	int32 x0 = (x > 0.0 ? (int32)x : (int32)x - 1);
	int32 x1 = x0 + 1;
	int32 y0 = (y > 0.0 ? (int32)y : (int32)y - 1);
	int32 y1 = y0 + 1;
	int32 z0 = (z > 0.0 ? (int32)z : (int32)z - 1);
	int32 z1 = z0 + 1;

	// Map the difference between the coordinates of the input value and the
	// coordinates of the cube's outer-lower-left vertex onto an S-curve.
	double xs = 0, ys = 0, zs = 0;

	xs = SCurve5(x - (double)x0);
	ys = SCurve5(y - (double)y0);
	zs = SCurve5(z - (double)z0);

	// Now calculate the noise values at each vertex of the cube.  To generate
	// the coherent-noise value at the input point, interpolate these eight
	// noise values using the S-curve value as the interpolant (trilinear
	// interpolation.)
	double n0, n1, ix0, ix1, iy0, iy1;

	n0 = ValueNoise3D(x0, y0, z0, seed);
	n1 = ValueNoise3D(x1, y0, z0, seed);
	ix0 = Lerp(n0, n1, xs);
	n0 = ValueNoise3D(x0, y1, z0, seed);
	n1 = ValueNoise3D(x1, y1, z0, seed);
	ix1 = Lerp(n0, n1, xs);
	iy0 = Lerp(ix0, ix1, ys);
	n0 = ValueNoise3D(x0, y0, z1, seed);
	n1 = ValueNoise3D(x1, y0, z1, seed);
	ix0 = Lerp(n0, n1, xs);
	n0 = ValueNoise3D(x0, y1, z1, seed);
	n1 = ValueNoise3D(x1, y1, z1, seed);
	ix1 = Lerp(n0, n1, xs);
	iy1 = Lerp(ix0, ix1, ys);

	return Lerp(iy0, iy1, zs);
}


double IntegerNoise(int32 x, int32 y, int32 z)
{
	int32 n = x + y * 57 + z * 91;
	n = (n << 13) ^ n;
	int32 nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0 - ((double)nn / 1073741824.0);
}

double SCurve(double weight)
{
	return -2 * weight*weight*weight + 3 * weight*weight;
}

double Lerp2D(double v1, double v2, double v3, double v4, double x, double y)
{
	double s = Lerp(v1, v2, x);
	double t = Lerp(v3, v4, y);
	return Lerp(s, t, y);
}

double CoherentNoise(double x, double y, double z)
{
	int32 intX = (int32)(floor(x));
	int32 intY = (int32)(floor(y));
	int32 intZ = (int32)(floor(z));

	double n0 = IntegerNoise(intX, intY, intZ);
	double n1 = IntegerNoise(intX, intY + 1, intZ);
	double n2 = IntegerNoise(intX, intY + 1, intZ + 1);
	double n3 = IntegerNoise(intX, intY, intZ + 1);
	double n4 = IntegerNoise(intX + 1, intY, intZ);
	double n5 = IntegerNoise(intX + 1, intY + 1, intZ);
	double n6 = IntegerNoise(intX + 1, intY + 1, intZ + 1);
	double n7 = IntegerNoise(intX + 1, intY, intZ + 1);

	double w1 = Lerp(n0, n1, 0.5);
	double w2 = Lerp(n1, n2, 0.5);
	double w3 = Lerp(n2, n3, 0.5);
	double w4 = Lerp(n3, n0, 0.5);
	double w5 = Lerp(n4, n5, 0.5);
	double w6 = Lerp(n5, n6, 0.5);
	double w7 = Lerp(n6, n7, 0.5);
	double w8 = Lerp(n7, n4, 0.5);

	double w9 = Lerp2D(w1, w2, w3, w4, 0.5, 0.5);
	double w10 = Lerp2D(w5, w6, w7, w8, 0.5, 0.5);

	//double weight = x - floor(x);
	// double noise = lerp (n0, n1, weight);
	//double noise = Lerp(n0, n1, SCurve(weight));
	//return noise;

	return Lerp(w9, w10, 0.5);
}

AOrionVoxelBase::AOrionVoxelBase(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	for (int32 i = 0; i < CHUNK_SIZE_XY; i++)
	{
		for (int32 j = 0; j < CHUNK_SIZE_XY; j++)
		{
			for (int32 k = 0; k < CHUNK_SIZE_XY; k++)
			{
				FString chunkName = FString::Printf(TEXT("GeneratedMesh%02d"), i*CHUNK_SIZE_XY*CHUNK_SIZE_XY + j*CHUNK_SIZE_XY + k);
				mesh[i*CHUNK_SIZE_XY*CHUNK_SIZE_XY + j*CHUNK_SIZE_XY + k] = PCIP.CreateDefaultSubobject<UOrionGeneratedMeshComponent>(this, FName(*chunkName));

				//FVector Offset = FVector((i - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_X, (j - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_Y, (k - CHUNK_SIZE_Z / 2) * VOXEL_WIDTH * VOXEL_Z);
				//mesh[i*CHUNK_SIZE_XY*CHUNK_SIZE_XY + j*CHUNK_SIZE_XY + k]->SetBoundsData(Offset + FVector(VOXEL_X*VOXEL_WIDTH, VOXEL_Y*VOXEL_WIDTH, VOXEL_Z*VOXEL_WIDTH)*0.5);
			}
		}
	}

	if (!VoxMaterial)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/VoxelTest/VoxMat.VoxMat'"));
		if (Material.Object)
			VoxMaterial = Material.Object;
	}

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.TickGroup = TG_PrePhysics;

	PoolIndex = -1;

	Chunks = nullptr;
}

//the only reason this can tick, is to support async loading
void AOrionVoxelBase::Tick(float DeltaSeconds)
{
	//	GenerateMeshFromChunks();
}

#if WITH_EDITOR
void AOrionVoxelBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	FName PropertyName = PropertyThatChanged != NULL ? PropertyThatChanged->GetFName() : NAME_None;

	if (PropertyName == FName(TEXT("RecalculateTerrain")))
	{
		RecalculateTerrain = false;
		RebuildTerrain();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AOrionVoxelBase::RebuildTerrain()
{
	/*if (Chunks != nullptr)
	{
	for (int i = 0; i < CHUNK_SIZE_XY; ++i)
	{
	for (int j = 0; j < CHUNK_SIZE_XY; ++j)
	{
	delete[] Chunks[i][j];
	}

	delete[] Chunks[i];
	}
	delete[] Chunks;
	}*/

	if (VoxelTree != nullptr)
		delete VoxelTree;

	//fractal noise using diamond square
	UOrionDiamondSquare::Init();
	UOrionDiamondSquare::SeedValues(0, 0, DIAMOND_RES);
	UOrionDiamondSquare::GenerateHeightValues(0, 0, DIAMOND_RES);

	float MaxHeight = UOrionDiamondSquare::MaxHeight*1.0 + 0.05;
	//allocate memory for all of our chunks
	VoxelTree = ConstructObject<UOrionOctTree>(UOrionOctTree::StaticClass());
	VoxelTree->Width = DIAMOND_RES*VOXEL_WIDTH;

	//init some values
	VoxelTree->Init();

	//Chunks = new FVoxelChunk**[CHUNK_SIZE_XY];
	for (int32 i = 0; i < CHUNK_SIZE_XY; i++)
	{
		//Chunks[i] = new FVoxelChunk*[CHUNK_SIZE_XY];

		for (int32 j = 0; j < CHUNK_SIZE_XY; j++)
		{
			//Chunks[i][j] = new FVoxelChunk[CHUNK_SIZE_Z];

			for (int32 k = 0; k < CHUNK_SIZE_XY; k++)
			{
				//Chunks[i][j][k].bDirty = true;
				//Chunks[i][j][k].bEmpty = true;

				//mesh[i][j][k] = PCIP.CreateDefaultSubobject<UOrionGeneratedMeshComponent>(this, TEXT("GeneratedMesh"));

				for (int32 x = 0; x < VOXEL_X; x += CHUNK_DETAIL)
				{
					for (int32 y = 0; y < VOXEL_Y; y += CHUNK_DETAIL)
					{
						float /*a, b, c, d, e, f,*/ g;

						/*a = UOrionDiamondSquare::HeightValues[int(DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)) + 0][int(DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)) + 0] / MaxHeight;
						b = UOrionDiamondSquare::HeightValues[int(DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)) + 0][int(DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)) + 1] / MaxHeight;
						c = UOrionDiamondSquare::HeightValues[int(DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)) + 1][int(DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)) + 1] / MaxHeight;
						d = UOrionDiamondSquare::HeightValues[int(DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)) + 1][int(DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)) + 0] / MaxHeight;

						float x1 = SCurve5(((DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)) - float(int(DIAMOND_RES*float(VOXEL_X * i + x) / float(CHUNK_SIZE_XY*VOXEL_X)))));// / (float(CHUNK_SIZE_XY*VOXEL_X) / DIAMOND_RES));
						float y1 = SCurve5(((DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)) - float(int(DIAMOND_RES*float(VOXEL_Y * j + y) / float(CHUNK_SIZE_XY*VOXEL_Y)))));// / (float(CHUNK_SIZE_XY*VOXEL_Y) / DIAMOND_RES));

						e = Lerp(a, b, y1);
						f = Lerp(d, c, y1);

						g = Lerp(e, f, x1);*/

						g = UOrionDiamondSquare::HeightValues[int(float(VOXEL_X * i + x))][int(float(VOXEL_Y * j + y))] / MaxHeight;

						for (int32 z = 0; z < VOXEL_Z; z += CHUNK_DETAIL)
						{
							//Perlin Noise (make this into a class of it's own)
							/*float noise = 0.0;
							float zoom = Zoom; //75.0;
							int32 seed = Seed; //93468
							double freq = Frequency; //2.0
							double amplitude = Amplitude; //1.0
							double pers = Persistence; //0.5

							//higher pers = more rough terrain

							for (int32 a = 0; a < 8; a++)
							{
							noise += ValueCoherentNoise3D((x + i*VOXEL_X) * freq / zoom, (y + j*VOXEL_Y) * freq / zoom, (z + k*VOXEL_Z) *freq / zoom, seed) * amplitude;// *(1.0 - ((z + k*VOXEL_Z) / 200.0));
							freq *= 2.0;
							amplitude *= pers;
							}

							//make lower stuff more dense, and higher locations less dense
							float height = z + k*VOXEL_Z;

							Chunks[i][j][k].pVoxels[x][y][z].Density = noise - (height > 64 ? (height - 64) / 32 : 0.0) + (height < 32 ? (32 - height) / 16 : 0.0);*/

							VoxelTree->InsertPoint((FVector(i*VOXEL_X + x, j*VOXEL_Y + y, k*VOXEL_Z + z) * VOXEL_WIDTH) - (FVector(0.5, 0.5, 0.5)*VOXEL_X*CHUNK_SIZE_XY*VOXEL_WIDTH)/* + FVector(0.5, 0.5, 0.5)*VOXEL_WIDTH*/, float(FMath::Max(-1.0, FMath::Min(1.0, ((((g + 1.0) / 2.0) * (CHUNK_SIZE_XY * VOXEL_Z)) - (VOXEL_Z * k + z))))), FVector(0, 0, 0), VOXEL_X*CHUNK_SIZE_XY*VOXEL_WIDTH);

							//g is the density at this x,y position, so that height will be our 0.0 density, lower will be higher
							/*Chunks[i][j][k].pVoxels[x][y][z].Density = float(FMath::Max(-1.0, FMath::Min(1.0, ((((g + 1.0) / 2.0) * (CHUNK_SIZE_Z * VOXEL_Z)) - (VOXEL_Z * k + z)))));// / (float(CHUNK_SIZE_Z*VOXEL_Z) / DIAMOND_RES));

							if (g > 0.6)
							Chunks[i][j][k].pVoxels[x][y][z].Type = VOXEL_ROCK;
							else
							Chunks[i][j][k].pVoxels[x][y][z].Type = VOXEL_GRASS;
							Chunks[i][j][k].bEmpty = false;*/
						}
					}
				}
			}
		}
	}

	/*float CachedDensities[2][(VOXEL_Y / CHUNK_DETAIL) + 1][(VOXEL_Z / CHUNK_DETAIL) + 1];

	for (int32 i = 0; i < CHUNK_SIZE_XY; i++)
	{
	for (int32 j = 0; j < CHUNK_SIZE_XY; j++)
	{
	for (int32 k = 0; k < CHUNK_SIZE_Z; k++)
	{
	bool bDoEdgeX = i < CHUNK_SIZE_XY - 1;
	bool bDoEdgeY = j < CHUNK_SIZE_XY - 1;
	bool bDoEdgeZ = k < CHUNK_SIZE_Z - 1;

	for (int32 x = 0; x < VOXEL_X; x ++)
	{
	if (x % CHUNK_DETAIL == 0)
	{
	for (int32 l = 0; l < 2; l++)
	{
	for (int32 m = 0; m < VOXEL_Y + CHUNK_DETAIL; m += CHUNK_DETAIL)
	{
	for (int32 n = 0; n < VOXEL_Z + CHUNK_DETAIL; n += CHUNK_DETAIL)
	{
	if (x != 0 && l == 0)
	{
	CachedDensities[0][m / CHUNK_DETAIL][n / CHUNK_DETAIL] = CachedDensities[1][m / CHUNK_DETAIL][n / CHUNK_DETAIL];
	continue;
	}

	bool bIsEdgeX = x + CHUNK_DETAIL >= VOXEL_X;
	bool bIsEdgeY = m >= VOXEL_Y;
	bool bIsEdgeZ = n >= VOXEL_Z;

	CachedDensities[l][m / CHUNK_DETAIL][n / CHUNK_DETAIL] = Chunks[(bDoEdgeX && bIsEdgeX) ? i + 1 : i][(bDoEdgeY && bIsEdgeY) ? j + 1 : j][(bDoEdgeZ && bIsEdgeZ) ? k + 1 : k].
	pVoxels[(bIsEdgeX && bDoEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l*CHUNK_DETAIL))][(bIsEdgeY && bDoEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][(bIsEdgeZ && bDoEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density;

	}
	}
	}
	}

	for (int32 y = 0; y < VOXEL_Y; y ++)
	{
	for (int32 z = 0; z < VOXEL_Z; z ++)
	{
	if (x % CHUNK_DETAIL != 0 || y % CHUNK_DETAIL != 0 || z % CHUNK_DETAIL != 0)
	{
	bool bDoEdgeX = i < CHUNK_SIZE_XY - 1;
	bool bDoEdgeY = j < CHUNK_SIZE_XY - 1;
	bool bDoEdgeZ = k < CHUNK_SIZE_Z - 1;

	bool bIsEdgeX = x > VOXEL_X - CHUNK_DETAIL;
	bool bIsEdgeY = y > VOXEL_Y - CHUNK_DETAIL;
	bool bIsEdgeZ = z > VOXEL_Z - CHUNK_DETAIL;

	double xs = 0, ys = 0, zs = 0;

	//xs = SCurve5(float(x % 4) / 4.0);
	//ys = SCurve5(float(y % 4) / 4.0);
	//zs = SCurve5(float(z % 4) / 4.0);

	xs = float(x % CHUNK_DETAIL) / CHUNK_DETAIL;
	ys = float(y % CHUNK_DETAIL) / CHUNK_DETAIL;
	zs = float(z % CHUNK_DETAIL) / CHUNK_DETAIL;

	float a, b, c, d, e, f, g;

	a = Lerp(CachedDensities[0][y / CHUNK_DETAIL][z / CHUNK_DETAIL], CachedDensities[0][(y / CHUNK_DETAIL) + 1][z / CHUNK_DETAIL], ys);
	b = Lerp(CachedDensities[0][y / CHUNK_DETAIL][(z / CHUNK_DETAIL) + 1], CachedDensities[0][(y / CHUNK_DETAIL) + 1][(z / CHUNK_DETAIL) + 1], ys);
	c = Lerp(CachedDensities[1][y / CHUNK_DETAIL][z / CHUNK_DETAIL], CachedDensities[1][(y / CHUNK_DETAIL) + 1][z / CHUNK_DETAIL], ys);
	d = Lerp(CachedDensities[1][y / CHUNK_DETAIL][(z / CHUNK_DETAIL) + 1], CachedDensities[1][(y / CHUNK_DETAIL) + 1][(z / CHUNK_DETAIL) + 1], ys);

	e = Lerp(a, b, zs);
	f = Lerp(c, d, zs);

	g = Lerp(e, f, xs);

	Chunks[i][j][k].pVoxels[x][y][z].Density = g;
	Chunks[i][j][k].pVoxels[x][y][z].Type = EVoxelType(FMath::RandRange(VOXEL_ROCK, VOXEL_SIZE));
	Chunks[i][j][k].bEmpty = false;
	}
	}
	}
	}
	}
	}
	}*/

	//fill in our air/ground values
	VoxelTree->FillIn(FVector(0, 0, 0));

	GenerateMeshFromChunks();
}

//destructor needed
/*AOrionVoxelBase:~AOrionVoxelBase()
{
// Delete the blocks
for (int i = 0; i < CHUNK_SIZE; ++i)
{
for (int j = 0; j < CHUNK_SIZE; ++j)
{
delete[] Chunks[i][j];
}

delete[] Chunks[i];
}
delete[] Chunks;
}*/

FVector AOrionVoxelBase::FindEdgeIntersection(FVector p1, FVector p2, float amount)
{
	return p1 + (p2 - p1)*FMath::Abs(amount);
}

//0 = x
//1 = -x
//2 = y
//3 = -y
//4 = z
//4 = -z
float CalculateGradients(int32 i, int32 j, int32 k, int32 x, int32 y, int32 z, int32 type)
{
	switch (type)
	{
	case 0:

		break;
	}

	return 0.0;
}

UOrionGeneratedMeshComponent* AOrionVoxelBase::GetMeshFromPool()
{
	if (PoolIndex >= CHUNK_SIZE_XY*CHUNK_SIZE_XY*CHUNK_SIZE_XY)
		return nullptr;

	PoolIndex++;

	return mesh[PoolIndex];
}

void AOrionVoxelBase::DrawPatch(UOrionOctTree *theTree, UOrionOctTree *parentTree, FVector center, int32 LOD)
{
	FGeneratedMeshTriangle tri;
	FVector points[8];
	float enabled[8];
	uint8 types[8];
	FVector gradients[8];
	FVertexAndIndex yzPlane[2][VOXEL_Y + 2][VOXEL_Z + 2];
	DualContourCube cubes[VOXEL_X + 1][VOXEL_Y + 1][VOXEL_Z + 1];

	//if we're completely empty, just skip us
	if (theTree->Children[0] == nullptr)
		return;

	for (int32 x = 0; x < VOXEL_X + 1; x++)
	{
		for (int32 l = 0; l < 2; l++)
		{
			for (int32 m = 0; m < VOXEL_Y + 2; m++)
			{
				for (int32 n = 0; n < VOXEL_Z + 2; n++)
				{
					if (x != 0 && l == 0)
					{
						yzPlane[0][m][n] = yzPlane[1][m][n];
						continue;
					}

					bool bIsEdgeX = x + l >= VOXEL_X;
					bool bIsEdgeY = m >= VOXEL_Y;
					bool bIsEdgeZ = n >= VOXEL_Z;

					bool bIsDoubleEdgeX = x + l >= VOXEL_X - 1;
					bool bIsDoubleEdgeY = m >= VOXEL_Y - 1;
					bool bIsDoubleEdgeZ = n >= VOXEL_Z - 1;

					bool bIsTripleEdgeX = x + l >= VOXEL_X + 1;
					bool bIsTripleEdgeY = m >= VOXEL_Y + 1;
					bool bIsTripleEdgeZ = n >= VOXEL_Z + 1;

					bool bIsNegEdgeX = x + l == 0;
					bool bIsNegEdgeY = m == 0;
					bool bIsNegEdgeZ = n == 0;

					yzPlane[l][m][n].pos = center - (FVector(0.5, 0.5, 0.5)*theTree->Width) + FVector((x + l)*VOXEL_WIDTH, m*VOXEL_WIDTH, n*VOXEL_WIDTH)*LOD;

					FVector p = center - (FVector(0.5, 0.5, 0.5)*theTree->Width) + FVector((x + l)*VOXEL_WIDTH, m*VOXEL_WIDTH, n*VOXEL_WIDTH)*LOD;// +FVector(0.5, 0.5, 0.5)*VOXEL_WIDTH;

					yzPlane[l][m][n].density = VoxelTree->GetData(p, FVector(0, 0, 0), LOD);

					yzPlane[l][m][n].type = VOXEL_ROCK;

					yzPlane[l][m][n].gradient = FVector(
						VoxelTree->GetData(p + FVector(1, 0, 0)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD) - VoxelTree->GetData(p + FVector(-1, 0, 0)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD),
						VoxelTree->GetData(p + FVector(0, 1, 0)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD) - VoxelTree->GetData(p + FVector(0, -1, 0)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD),
						VoxelTree->GetData(p + FVector(0, 0, 1)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD) - VoxelTree->GetData(p + FVector(0, 0, -1)*VOXEL_WIDTH*LOD, FVector(0, 0, 0), LOD));

				}
			}
		}

		for (int32 y = 0; y < VOXEL_X + 1; y++)
		{
			for (int32 z = 0; z < VOXEL_X + 1; z++)
			{
				//fill our our points and enableds
				points[0] = yzPlane[0][y][z].pos;
				points[1] = yzPlane[0][y + 1][z].pos;
				points[2] = yzPlane[0][y + 1][z + 1].pos;
				points[3] = yzPlane[0][y][z + 1].pos;
				points[4] = yzPlane[1][y][z].pos;
				points[5] = yzPlane[1][y + 1][z].pos;
				points[6] = yzPlane[1][y + 1][z + 1].pos;
				points[7] = yzPlane[1][y][z + 1].pos;

				enabled[0] = yzPlane[0][y][z].density;
				enabled[1] = yzPlane[0][y + 1][z].density;
				enabled[2] = yzPlane[0][y + 1][z + 1].density;
				enabled[3] = yzPlane[0][y][z + 1].density;
				enabled[4] = yzPlane[1][y][z].density;
				enabled[5] = yzPlane[1][y + 1][z].density;
				enabled[6] = yzPlane[1][y + 1][z + 1].density;
				enabled[7] = yzPlane[1][y][z + 1].density;

				types[0] = yzPlane[0][y][z].type;
				types[1] = yzPlane[0][y + 1][z].type;
				types[2] = yzPlane[0][y + 1][z + 1].type;
				types[3] = yzPlane[0][y][z + 1].type;
				types[4] = yzPlane[1][y][z].type;
				types[5] = yzPlane[1][y + 1][z].type;
				types[6] = yzPlane[1][y + 1][z + 1].type;
				types[7] = yzPlane[1][y][z + 1].type;

				//convert this to our index
				int32 cubeIndex = 0;
				for (int32 n = 0; n < 8; n++)
				{
					if (enabled[n] < 0.0)
						cubeIndex |= (1 << n);
				}

				//if all 8 points are inside or outside, we can ignore it completely
				if (!edgeTable[cubeIndex])
				{
					cubes[x][y][z].point = FVector(0, 0, 0);
					cubes[x][y][z].edgeMask = cubeIndex;
					cubes[x][y][z].bEdge = false;
					continue;
				}

				gradients[0] = yzPlane[0][y][z].gradient;
				gradients[1] = yzPlane[0][y + 1][z].gradient;
				gradients[2] = yzPlane[0][y + 1][z + 1].gradient;
				gradients[3] = yzPlane[0][y][z + 1].gradient;
				gradients[4] = yzPlane[1][y][z].gradient;
				gradients[5] = yzPlane[1][y + 1][z].gradient;
				gradients[6] = yzPlane[1][y + 1][z + 1].gradient;
				gradients[7] = yzPlane[1][y][z + 1].gradient;

				//get our edge intersection points
				FVoxelData EdgeVerts[12];
				if (edgeTable[cubeIndex] & 1) { EdgeVerts[0].pos = FindEdgeIntersection(points[0], points[1], enabled[0] / (enabled[0] - enabled[1])); EdgeVerts[0].type = enabled[0] < enabled[1] ? types[0] : types[1]; }
				if (edgeTable[cubeIndex] & 2) { EdgeVerts[1].pos = FindEdgeIntersection(points[1], points[2], enabled[1] / (enabled[1] - enabled[2])); EdgeVerts[1].type = enabled[1] < enabled[2] ? types[1] : types[2]; }
				if (edgeTable[cubeIndex] & 4) { EdgeVerts[2].pos = FindEdgeIntersection(points[2], points[3], enabled[2] / (enabled[2] - enabled[3])); EdgeVerts[2].type = enabled[2] < enabled[3] ? types[2] : types[3]; }
				if (edgeTable[cubeIndex] & 8) { EdgeVerts[3].pos = FindEdgeIntersection(points[3], points[0], enabled[3] / (enabled[3] - enabled[0])); EdgeVerts[3].type = enabled[3] < enabled[0] ? types[3] : types[0]; }
				if (edgeTable[cubeIndex] & 16) { EdgeVerts[4].pos = FindEdgeIntersection(points[4], points[5], enabled[4] / (enabled[4] - enabled[5])); EdgeVerts[4].type = enabled[4] < enabled[5] ? types[4] : types[5]; }
				if (edgeTable[cubeIndex] & 32) { EdgeVerts[5].pos = FindEdgeIntersection(points[5], points[6], enabled[5] / (enabled[5] - enabled[6])); EdgeVerts[5].type = enabled[5] < enabled[6] ? types[5] : types[6]; }
				if (edgeTable[cubeIndex] & 64) { EdgeVerts[6].pos = FindEdgeIntersection(points[6], points[7], enabled[6] / (enabled[6] - enabled[7])); EdgeVerts[6].type = enabled[6] < enabled[7] ? types[6] : types[7]; }
				if (edgeTable[cubeIndex] & 128) { EdgeVerts[7].pos = FindEdgeIntersection(points[7], points[4], enabled[7] / (enabled[7] - enabled[4])); EdgeVerts[7].type = enabled[7] < enabled[4] ? types[7] : types[4]; }
				if (edgeTable[cubeIndex] & 256) { EdgeVerts[8].pos = FindEdgeIntersection(points[0], points[4], enabled[0] / (enabled[0] - enabled[4])); EdgeVerts[8].type = enabled[0] < enabled[4] ? types[0] : types[4]; }
				if (edgeTable[cubeIndex] & 512) { EdgeVerts[9].pos = FindEdgeIntersection(points[1], points[5], enabled[1] / (enabled[1] - enabled[5])); EdgeVerts[9].type = enabled[1] < enabled[5] ? types[1] : types[5]; }
				if (edgeTable[cubeIndex] & 1024) { EdgeVerts[10].pos = FindEdgeIntersection(points[2], points[6], enabled[2] / (enabled[2] - enabled[6])); EdgeVerts[10].type = enabled[2] < enabled[6] ? types[2] : types[6]; }
				if (edgeTable[cubeIndex] & 2048) { EdgeVerts[11].pos = FindEdgeIntersection(points[3], points[7], enabled[3] / (enabled[3] - enabled[7])); EdgeVerts[11].type = enabled[3] < enabled[7] ? types[3] : types[7]; }

				FVector EdgeNormals[12];
				if (edgeTable[cubeIndex] & 1) EdgeNormals[0] = FindEdgeIntersection(gradients[0], gradients[1], enabled[0] / (enabled[0] - enabled[1])).SafeNormal();
				if (edgeTable[cubeIndex] & 2) EdgeNormals[1] = FindEdgeIntersection(gradients[1], gradients[2], enabled[1] / (enabled[1] - enabled[2])).SafeNormal();
				if (edgeTable[cubeIndex] & 4) EdgeNormals[2] = FindEdgeIntersection(gradients[2], gradients[3], enabled[2] / (enabled[2] - enabled[3])).SafeNormal();
				if (edgeTable[cubeIndex] & 8) EdgeNormals[3] = FindEdgeIntersection(gradients[3], gradients[0], enabled[3] / (enabled[3] - enabled[0])).SafeNormal();
				if (edgeTable[cubeIndex] & 16) EdgeNormals[4] = FindEdgeIntersection(gradients[4], gradients[5], enabled[4] / (enabled[4] - enabled[5])).SafeNormal();
				if (edgeTable[cubeIndex] & 32) EdgeNormals[5] = FindEdgeIntersection(gradients[5], gradients[6], enabled[5] / (enabled[5] - enabled[6])).SafeNormal();
				if (edgeTable[cubeIndex] & 64) EdgeNormals[6] = FindEdgeIntersection(gradients[6], gradients[7], enabled[6] / (enabled[6] - enabled[7])).SafeNormal();
				if (edgeTable[cubeIndex] & 128) EdgeNormals[7] = FindEdgeIntersection(gradients[7], gradients[4], enabled[7] / (enabled[7] - enabled[4])).SafeNormal();
				if (edgeTable[cubeIndex] & 256) EdgeNormals[8] = FindEdgeIntersection(gradients[0], gradients[4], enabled[0] / (enabled[0] - enabled[4])).SafeNormal();
				if (edgeTable[cubeIndex] & 512) EdgeNormals[9] = FindEdgeIntersection(gradients[1], gradients[5], enabled[1] / (enabled[1] - enabled[5])).SafeNormal();
				if (edgeTable[cubeIndex] & 1024) EdgeNormals[10] = FindEdgeIntersection(gradients[2], gradients[6], enabled[2] / (enabled[2] - enabled[6])).SafeNormal();
				if (edgeTable[cubeIndex] & 2048) EdgeNormals[11] = FindEdgeIntersection(gradients[3], gradients[7], enabled[3] / (enabled[3] - enabled[7])).SafeNormal();

				//find our QEF points
				static int32 intersections[12][2] = {
					{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 },
					{ 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
				};

				int32 edgeInfo = edgeTable[cubeIndex];

				FVector points[12];
				FVector points_v[12];
				FVector normals[12];
				FVector massPoint = FVector(0, 0, 0);
				FVector massNormal = FVector(0, 0, 0);
				int32 numIntersections = 0;
				int32 w;

				for (w = 0; w < 12; w++) {
					if (!(edgeInfo & (1 << w)))
						continue;

					massPoint += EdgeVerts[w].pos;
					massNormal += EdgeNormals[w];
					numIntersections++;
				}

				massPoint /= (float)numIntersections;
				massNormal /= (float)numIntersections;

				double matrix[12][3];
				double vector[12];
				int rows = 0;

				for (w = 0; w < 12; w++) {
					if (!(edgeInfo & (1 << w)))
						continue;

					const FVector &normal = EdgeNormals[w];

					matrix[rows][0] = normal.X;
					matrix[rows][1] = normal.Y;
					matrix[rows][2] = normal.Z;

					FVector p = EdgeVerts[w].pos - massPoint;
					vector[rows] = FVector::DotProduct(normal, p);

					rows++;
				}

				FVector newPointV;
				////	QEF::evaluate(matrix, vector, rows, &newPointV);
				newPointV = massPoint;

				cubes[x][y][z].point = newPointV;
				cubes[x][y][z].edgeMask = cubeIndex;
				cubes[x][y][z].bEdge = true;
				cubes[x][y][z].normal = massNormal;
				cubes[x][y][z].bNeg5 = enabled[5] < 0;
				cubes[x][y][z].bNeg6 = enabled[6] < 0;
				cubes[x][y][z].bNeg10 = enabled[2] < 0;
			}
		}
	}

	for (int32 x = 0; x < VOXEL_X; x++)
	{
		for (int32 y = 0; y < VOXEL_Y; y++)
		{
			for (int32 z = 0; z < VOXEL_Z; z++)
			{
				FVector quadPoints[4];

				//only pay attention to cubes that have at least one edge
				if (cubes[x][y][z].bEdge)
				{
					//we only need to look at three edges per cube z y x
					if (edgeTable[cubes[x][y][z].edgeMask] & 32)
					{
						quadPoints[0] = FVector(0, 0, 0);
						quadPoints[1] = FVector(1, 0, 0);
						quadPoints[2] = FVector(1, 1, 0);
						quadPoints[3] = FVector(0, 1, 0);

						AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg5);
					}

					if (edgeTable[cubes[x][y][z].edgeMask] & 64)
					{
						quadPoints[0] = FVector(0, 0, 0);
						quadPoints[1] = FVector(1, 0, 0);
						quadPoints[2] = FVector(1, 0, 1);
						quadPoints[3] = FVector(0, 0, 1);

						AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg6);
					}

					if (edgeTable[cubes[x][y][z].edgeMask] & 1024)
					{
						quadPoints[0] = FVector(0, 0, 0);
						quadPoints[1] = FVector(0, 1, 0);
						quadPoints[2] = FVector(0, 1, 1);
						quadPoints[3] = FVector(0, 0, 1);

						AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg10);
					}
				}
			}
		}
	}

	if (triangles.Num() == 0)
		return;

	//make sure we have a mesh
	if (theTree->mesh == nullptr)
	{
		theTree->mesh = GetMeshFromPool();
	}

	//make sure our mesh bounds are set
	theTree->mesh->SetBoundsData(center + FVector(VOXEL_X*VOXEL_WIDTH, VOXEL_Y*VOXEL_WIDTH, VOXEL_Z*VOXEL_WIDTH)*0.5*LOD, LOD);

	theTree->mesh->SetGeneratedMeshTriangles(triangles, VoxMaterial);
	//Chunks[i][j][k].bDirty = false;
	triangles.Empty();
}

void AOrionVoxelBase::GenerateMeshFromOctree(UOrionOctTree *theTree, UOrionOctTree *parentTree, FVector center)
{
	for (int32 i = 0; i < 8; i++)
	{
		if (theTree->Width == VOXEL_WIDTH * VOXEL_X * 4)
		{
			//todo: we have our desired resolution, now draw all of the blocks in our children!
			DrawPatch(theTree, parentTree, center, 4);
		}
		else if (theTree->Children[i] != nullptr)
			GenerateMeshFromOctree(theTree->Children[i], theTree, center + UOrionOctTree::TreeOffsets[i] * theTree->Width);
	}
}

void AOrionVoxelBase::GenerateMeshFromChunksDualContour()
{
	FGeneratedMeshTriangle tri;
	FVector points[8];
	float enabled[8];
	uint8 types[8];
	FVector gradients[8];
	FVertexAndIndex xzPlane[2][VOXEL_X + 2][VOXEL_Z + 2];
	FVertexAndIndex yzPlane[2][VOXEL_Y + 2][VOXEL_Z + 2];
	DualContourCube cubes[VOXEL_X + 1][VOXEL_Y + 1][VOXEL_Z + 1];

	//finish octree first
	GenerateMeshFromOctree(VoxelTree, nullptr, FVector(0, 0, 0));
	return;

	for (int32 i = 0; i < CHUNK_SIZE_XY; i++)
	{
		for (int32 j = 0; j < CHUNK_SIZE_XY; j++)
		{
			for (int32 k = 0; k < CHUNK_SIZE_Z; k++)
			{
				if (Chunks[i][j][k].bDirty == false || Chunks[i][j][k].bEmpty)
					continue;

				//location of our cube
				FVector Offset = FVector((i - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_X, (j - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_Y, (k - CHUNK_SIZE_Z / 2) * VOXEL_WIDTH * VOXEL_Z);

				bool bDoEdgeX = i < CHUNK_SIZE_XY - 1;
				bool bDoEdgeY = j < CHUNK_SIZE_XY - 1;
				bool bDoEdgeZ = k < CHUNK_SIZE_Z - 1;

				bool bDoNegEdgeX = i > 0 && i < CHUNK_SIZE_XY;
				bool bDoNegEdgeY = j > 0 && j < CHUNK_SIZE_XY;
				bool bDoNegEdgeZ = k > 0 && k < CHUNK_SIZE_Z;

				for (int32 x = 0; x < VOXEL_X + 1; x++)
				{
					if (x == VOXEL_X && i == CHUNK_SIZE_XY - 1)
						continue;

					for (int32 l = 0; l < 2; l++)
					{
						for (int32 m = 0; m < VOXEL_Y + 2; m++)
						{
							for (int32 n = 0; n < VOXEL_Z + 2; n++)
							{
								if (x != 0 && l == 0)
								{
									yzPlane[0][m][n] = yzPlane[1][m][n];
									continue;
								}

								bool bIsEdgeX = x + l >= VOXEL_X;
								bool bIsEdgeY = m >= VOXEL_Y;
								bool bIsEdgeZ = n >= VOXEL_Z;

								bool bIsDoubleEdgeX = x + l >= VOXEL_X - 1;
								bool bIsDoubleEdgeY = m >= VOXEL_Y - 1;
								bool bIsDoubleEdgeZ = n >= VOXEL_Z - 1;

								bool bIsTripleEdgeX = x + l >= VOXEL_X + 1;
								bool bIsTripleEdgeY = m >= VOXEL_Y + 1;
								bool bIsTripleEdgeZ = n >= VOXEL_Z + 1;

								bool bIsNegEdgeX = x + l == 0;
								bool bIsNegEdgeY = m == 0;
								bool bIsNegEdgeZ = n == 0;

								yzPlane[l][m][n].pos = Offset + FVector((x + l) * VOXEL_WIDTH, m * VOXEL_WIDTH, n * VOXEL_WIDTH);

								yzPlane[l][m][n].density = Chunks[(bDoEdgeX && (bIsEdgeX || bIsTripleEdgeX)) ? i + 1 : i][(bDoEdgeY && (bIsEdgeY || bIsTripleEdgeY)) ? j + 1 : j][(bDoEdgeZ && (bIsEdgeZ || bIsTripleEdgeZ)) ? k + 1 : k].
									pVoxels[((bDoEdgeX && bIsTripleEdgeX) ? (1) : ((bDoEdgeX && bIsEdgeX) ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l)))][((bDoEdgeY && bIsTripleEdgeY) ? (1) : ((bDoEdgeY && bIsEdgeY) ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m)))][((bDoEdgeZ && bIsTripleEdgeZ) ? (1) : ((bDoEdgeZ && bIsEdgeZ) ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n)))].Density;

								yzPlane[l][m][n].type = Chunks[(bDoEdgeX && bIsEdgeX) ? i + 1 : i][(bDoEdgeY && bIsEdgeY) ? j + 1 : j][(bDoEdgeZ && bIsEdgeZ) ? k + 1 : k].
									pVoxels[(bIsEdgeX && bDoEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][(bIsEdgeY && bDoEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][(bIsEdgeZ && bDoEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Type;

								yzPlane[l][m][n].gradient = FVector(Chunks[bDoEdgeX && (bIsDoubleEdgeX || bIsEdgeX) ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[(bDoEdgeX && bIsTripleEdgeX) ? 2 : (bDoEdgeX && bIsEdgeX ? 1 : (bDoEdgeX && bIsDoubleEdgeX ? 0 : x + l + 1))][bDoEdgeY && bIsTripleEdgeY ? 1 : (bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][bDoEdgeZ && bIsTripleEdgeZ ? 1 : (bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density -
									Chunks[bDoEdgeX && bIsTripleEdgeX ? i + 1 : (bDoNegEdgeX && bIsNegEdgeX ? i - 1 : i)][bIsEdgeY && bDoEdgeY ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bIsEdgeX && bIsTripleEdgeX ? 0 : (bDoNegEdgeX && bIsNegEdgeX ? VOXEL_X - 1 : (bIsNegEdgeX ? 0 : x + l - 1))][bDoEdgeY && bIsTripleEdgeY ? 1 : (bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][bDoEdgeZ && bIsTripleEdgeZ ? 1 : (bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density,
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bDoEdgeY && (bIsDoubleEdgeY || bIsEdgeY) ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoEdgeX && bIsTripleEdgeX ? 1 : (bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][(bDoEdgeY && bIsTripleEdgeY) ? 2 : (bDoEdgeY && bIsEdgeY ? 1 : (bDoEdgeY && bIsDoubleEdgeY ? 0 : m + 1))][bDoEdgeZ && bIsTripleEdgeZ ? 1 : (bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density -
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bDoEdgeY && bIsTripleEdgeY ? j + 1 : (bDoNegEdgeY && bIsNegEdgeY ? j - 1 : j)][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoEdgeX && bIsTripleEdgeX ? 1 : (bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][bIsEdgeY && bIsTripleEdgeY ? 0 : (bDoNegEdgeY && bIsNegEdgeY ? VOXEL_Y - 1 : (bIsNegEdgeY ? 0 : m - 1))][bDoEdgeZ && bIsTripleEdgeZ ? 1 : (bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density,
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bDoEdgeZ && (bIsDoubleEdgeZ || bIsEdgeZ) ? k + 1 : k].pVoxels[bDoEdgeX && bIsTripleEdgeX ? 1 : (bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][bDoEdgeY && bIsTripleEdgeY ? 1 : (bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][(bDoEdgeZ && bIsTripleEdgeZ) ? 2 : (bDoEdgeZ && bIsEdgeZ ? 1 : (bDoEdgeZ && bIsDoubleEdgeZ ? 0 : n + 1))].Density -
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bDoEdgeZ && bIsTripleEdgeZ ? k + 1 : (bDoNegEdgeZ && bIsNegEdgeZ ? k - 1 : k)].pVoxels[bDoEdgeX && bIsTripleEdgeX ? 1 : (bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][bDoEdgeY && bIsTripleEdgeY ? 1 : (bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][bIsEdgeZ && bIsTripleEdgeZ ? 0 : (bDoNegEdgeZ && bIsNegEdgeZ ? VOXEL_Z - 1 : (bIsNegEdgeZ ? 0 : n - 1))].Density);
							}
						}
					}

					for (int32 y = 0; y < VOXEL_Y + 1; y++)
					{
						if (y == VOXEL_Y && j == CHUNK_SIZE_XY - 1)
							continue;
						for (int32 z = 0; z < VOXEL_Z + 1; z++)
						{
							if (z == VOXEL_Z && k == CHUNK_SIZE_Z - 1)
								continue;

							//fill our our points and enableds
							points[0] = yzPlane[0][y][z].pos;
							points[1] = yzPlane[0][y + 1][z].pos;
							points[2] = yzPlane[0][y + 1][z + 1].pos;
							points[3] = yzPlane[0][y][z + 1].pos;
							points[4] = yzPlane[1][y][z].pos;
							points[5] = yzPlane[1][y + 1][z].pos;
							points[6] = yzPlane[1][y + 1][z + 1].pos;
							points[7] = yzPlane[1][y][z + 1].pos;

							enabled[0] = yzPlane[0][y][z].density;
							enabled[1] = yzPlane[0][y + 1][z].density;
							enabled[2] = yzPlane[0][y + 1][z + 1].density;
							enabled[3] = yzPlane[0][y][z + 1].density;
							enabled[4] = yzPlane[1][y][z].density;
							enabled[5] = yzPlane[1][y + 1][z].density;
							enabled[6] = yzPlane[1][y + 1][z + 1].density;
							enabled[7] = yzPlane[1][y][z + 1].density;

							types[0] = yzPlane[0][y][z].type;
							types[1] = yzPlane[0][y + 1][z].type;
							types[2] = yzPlane[0][y + 1][z + 1].type;
							types[3] = yzPlane[0][y][z + 1].type;
							types[4] = yzPlane[1][y][z].type;
							types[5] = yzPlane[1][y + 1][z].type;
							types[6] = yzPlane[1][y + 1][z + 1].type;
							types[7] = yzPlane[1][y][z + 1].type;

							//convert this to our index
							int32 cubeIndex = 0;
							for (int32 n = 0; n < 8; n++)
							{
								if (enabled[n] < 0.0)
									cubeIndex |= (1 << n);
							}

							//if all 8 points are inside or outside, we can ignore it completely
							if (!edgeTable[cubeIndex])
							{
								cubes[x][y][z].point = FVector(0, 0, 0);
								cubes[x][y][z].edgeMask = cubeIndex;
								cubes[x][y][z].bEdge = false;
								continue;
							}

							gradients[0] = yzPlane[0][y][z].gradient;
							gradients[1] = yzPlane[0][y + 1][z].gradient;
							gradients[2] = yzPlane[0][y + 1][z + 1].gradient;
							gradients[3] = yzPlane[0][y][z + 1].gradient;
							gradients[4] = yzPlane[1][y][z].gradient;
							gradients[5] = yzPlane[1][y + 1][z].gradient;
							gradients[6] = yzPlane[1][y + 1][z + 1].gradient;
							gradients[7] = yzPlane[1][y][z + 1].gradient;

							//get our edge intersection points
							FVoxelData EdgeVerts[12];
							if (edgeTable[cubeIndex] & 1) { EdgeVerts[0].pos = FindEdgeIntersection(points[0], points[1], enabled[0] / (enabled[0] - enabled[1])); EdgeVerts[0].type = enabled[0] < enabled[1] ? types[0] : types[1]; }
							if (edgeTable[cubeIndex] & 2) { EdgeVerts[1].pos = FindEdgeIntersection(points[1], points[2], enabled[1] / (enabled[1] - enabled[2])); EdgeVerts[1].type = enabled[1] < enabled[2] ? types[1] : types[2]; }
							if (edgeTable[cubeIndex] & 4) { EdgeVerts[2].pos = FindEdgeIntersection(points[2], points[3], enabled[2] / (enabled[2] - enabled[3])); EdgeVerts[2].type = enabled[2] < enabled[3] ? types[2] : types[3]; }
							if (edgeTable[cubeIndex] & 8) { EdgeVerts[3].pos = FindEdgeIntersection(points[3], points[0], enabled[3] / (enabled[3] - enabled[0])); EdgeVerts[3].type = enabled[3] < enabled[0] ? types[3] : types[0]; }
							if (edgeTable[cubeIndex] & 16) { EdgeVerts[4].pos = FindEdgeIntersection(points[4], points[5], enabled[4] / (enabled[4] - enabled[5])); EdgeVerts[4].type = enabled[4] < enabled[5] ? types[4] : types[5]; }
							if (edgeTable[cubeIndex] & 32) { EdgeVerts[5].pos = FindEdgeIntersection(points[5], points[6], enabled[5] / (enabled[5] - enabled[6])); EdgeVerts[5].type = enabled[5] < enabled[6] ? types[5] : types[6]; }
							if (edgeTable[cubeIndex] & 64) { EdgeVerts[6].pos = FindEdgeIntersection(points[6], points[7], enabled[6] / (enabled[6] - enabled[7])); EdgeVerts[6].type = enabled[6] < enabled[7] ? types[6] : types[7]; }
							if (edgeTable[cubeIndex] & 128) { EdgeVerts[7].pos = FindEdgeIntersection(points[7], points[4], enabled[7] / (enabled[7] - enabled[4])); EdgeVerts[7].type = enabled[7] < enabled[4] ? types[7] : types[4]; }
							if (edgeTable[cubeIndex] & 256) { EdgeVerts[8].pos = FindEdgeIntersection(points[0], points[4], enabled[0] / (enabled[0] - enabled[4])); EdgeVerts[8].type = enabled[0] < enabled[4] ? types[0] : types[4]; }
							if (edgeTable[cubeIndex] & 512) { EdgeVerts[9].pos = FindEdgeIntersection(points[1], points[5], enabled[1] / (enabled[1] - enabled[5])); EdgeVerts[9].type = enabled[1] < enabled[5] ? types[1] : types[5]; }
							if (edgeTable[cubeIndex] & 1024) { EdgeVerts[10].pos = FindEdgeIntersection(points[2], points[6], enabled[2] / (enabled[2] - enabled[6])); EdgeVerts[10].type = enabled[2] < enabled[6] ? types[2] : types[6]; }
							if (edgeTable[cubeIndex] & 2048) { EdgeVerts[11].pos = FindEdgeIntersection(points[3], points[7], enabled[3] / (enabled[3] - enabled[7])); EdgeVerts[11].type = enabled[3] < enabled[7] ? types[3] : types[7]; }

							FVector EdgeNormals[12];
							if (edgeTable[cubeIndex] & 1) EdgeNormals[0] = FindEdgeIntersection(gradients[0], gradients[1], enabled[0] / (enabled[0] - enabled[1])).SafeNormal();
							if (edgeTable[cubeIndex] & 2) EdgeNormals[1] = FindEdgeIntersection(gradients[1], gradients[2], enabled[1] / (enabled[1] - enabled[2])).SafeNormal();
							if (edgeTable[cubeIndex] & 4) EdgeNormals[2] = FindEdgeIntersection(gradients[2], gradients[3], enabled[2] / (enabled[2] - enabled[3])).SafeNormal();
							if (edgeTable[cubeIndex] & 8) EdgeNormals[3] = FindEdgeIntersection(gradients[3], gradients[0], enabled[3] / (enabled[3] - enabled[0])).SafeNormal();
							if (edgeTable[cubeIndex] & 16) EdgeNormals[4] = FindEdgeIntersection(gradients[4], gradients[5], enabled[4] / (enabled[4] - enabled[5])).SafeNormal();
							if (edgeTable[cubeIndex] & 32) EdgeNormals[5] = FindEdgeIntersection(gradients[5], gradients[6], enabled[5] / (enabled[5] - enabled[6])).SafeNormal();
							if (edgeTable[cubeIndex] & 64) EdgeNormals[6] = FindEdgeIntersection(gradients[6], gradients[7], enabled[6] / (enabled[6] - enabled[7])).SafeNormal();
							if (edgeTable[cubeIndex] & 128) EdgeNormals[7] = FindEdgeIntersection(gradients[7], gradients[4], enabled[7] / (enabled[7] - enabled[4])).SafeNormal();
							if (edgeTable[cubeIndex] & 256) EdgeNormals[8] = FindEdgeIntersection(gradients[0], gradients[4], enabled[0] / (enabled[0] - enabled[4])).SafeNormal();
							if (edgeTable[cubeIndex] & 512) EdgeNormals[9] = FindEdgeIntersection(gradients[1], gradients[5], enabled[1] / (enabled[1] - enabled[5])).SafeNormal();
							if (edgeTable[cubeIndex] & 1024) EdgeNormals[10] = FindEdgeIntersection(gradients[2], gradients[6], enabled[2] / (enabled[2] - enabled[6])).SafeNormal();
							if (edgeTable[cubeIndex] & 2048) EdgeNormals[11] = FindEdgeIntersection(gradients[3], gradients[7], enabled[3] / (enabled[3] - enabled[7])).SafeNormal();

							//find our QEF points
							static int32 intersections[12][2] = {
								{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 4, 5 }, { 5, 6 },
								{ 6, 7 }, { 7, 4 }, { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
							};

							int32 edgeInfo = edgeTable[cubeIndex];

							FVector points[12];
							FVector points_v[12];
							FVector normals[12];
							FVector massPoint = FVector(0, 0, 0);
							FVector massNormal = FVector(0, 0, 0);
							int32 numIntersections = 0;
							int32 w;

							for (w = 0; w < 12; w++) {
								if (!(edgeInfo & (1 << w)))
									continue;

								massPoint += EdgeVerts[w].pos;
								massNormal += EdgeNormals[w];
								numIntersections++;
							}

							massPoint /= (float)numIntersections;
							massNormal /= (float)numIntersections;

							double matrix[12][3];
							double vector[12];
							int rows = 0;

							for (w = 0; w < 12; w++) {
								if (!(edgeInfo & (1 << w)))
									continue;

								const FVector &normal = EdgeNormals[w];

								matrix[rows][0] = normal.X;
								matrix[rows][1] = normal.Y;
								matrix[rows][2] = normal.Z;

								FVector p = EdgeVerts[w].pos - massPoint;
								vector[rows] = FVector::DotProduct(normal, p);

								rows++;
							}

							FVector newPointV;
							QEF::evaluate(matrix, vector, rows, &newPointV);
							newPointV = massPoint;

							cubes[x][y][z].point = newPointV;
							cubes[x][y][z].edgeMask = cubeIndex;
							cubes[x][y][z].bEdge = true;
							cubes[x][y][z].normal = massNormal;
							cubes[x][y][z].bNeg5 = enabled[5] < 0;
							cubes[x][y][z].bNeg6 = enabled[6] < 0;
							cubes[x][y][z].bNeg10 = enabled[2] < 0;
						}
					}
				}

				//cycle through our cubes and draw an neccessary quads
				for (int32 x = 0; x < VOXEL_X - ((i >= CHUNK_SIZE_XY - 1) ? 1 : 0); x++)
				{
					for (int32 y = 0; y < VOXEL_Y - ((j >= CHUNK_SIZE_XY - 1) ? 1 : 0); y++)
					{
						for (int32 z = 0; z < VOXEL_Z - ((k >= CHUNK_SIZE_Z - 1) ? 1 : 0); z++)
						{
							FVector quadPoints[4];

							//only pay attention to cubes that have at least one edge
							if (cubes[x][y][z].bEdge)
							{
								//we only need to look at three edges per cube z y x
								if (edgeTable[cubes[x][y][z].edgeMask] & 32)
								{
									quadPoints[0] = FVector(0, 0, 0);
									quadPoints[1] = FVector(1, 0, 0);
									quadPoints[2] = FVector(1, 1, 0);
									quadPoints[3] = FVector(0, 1, 0);

									AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg5);
								}

								if (edgeTable[cubes[x][y][z].edgeMask] & 64)
								{
									quadPoints[0] = FVector(0, 0, 0);
									quadPoints[1] = FVector(1, 0, 0);
									quadPoints[2] = FVector(1, 0, 1);
									quadPoints[3] = FVector(0, 0, 1);

									AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg6);
								}

								if (edgeTable[cubes[x][y][z].edgeMask] & 1024)
								{
									quadPoints[0] = FVector(0, 0, 0);
									quadPoints[1] = FVector(0, 1, 0);
									quadPoints[2] = FVector(0, 1, 1);
									quadPoints[3] = FVector(0, 0, 1);

									AddTris(x, y, z, cubes, quadPoints, cubes[x][y][z].bNeg10);
								}
							}
						}
					}
				}

				//mesh[i][j][k]->SetGeneratedMeshTriangles(triangles, VoxMaterial);
				Chunks[i][j][k].bDirty = false;

				triangles.Empty();

				//pass our edge cubes onto the next chunk for easy access
			}
		}
	}

	//mesh->SetGeneratedMeshTriangles(triangles,VoxMaterial);
}

void AOrionVoxelBase::AddTris(int32 x, int32 y, int32 z, DualContourCube cubes[VOXEL_X + 1][VOXEL_Y + 1][VOXEL_Z + 1], FVector quadPoints[4], BOOLEAN bInvert)
{
	FGeneratedMeshTriangle tri;

	for (int32 n = 0; n < 2; n++)
	{
		if (bInvert)
		{
			tri.Vertex0.Position = cubes[x + int(quadPoints[0].X)][y + int(quadPoints[0].Y)][z + int(quadPoints[0].Z)].point;
			tri.Vertex1.Position = cubes[x + int(quadPoints[1 + n].X)][y + int(quadPoints[1 + n].Y)][z + int(quadPoints[1 + n].Z)].point;
			tri.Vertex2.Position = cubes[x + int(quadPoints[2 + n].X)][y + int(quadPoints[2 + n].Y)][z + int(quadPoints[2 + n].Z)].point;

			tri.Vertex0.NormalZ = -cubes[x + int(quadPoints[0].X)][y + int(quadPoints[0].Y)][z + int(quadPoints[0].Z)].normal;
			tri.Vertex1.NormalZ = -cubes[x + int(quadPoints[1 + n].X)][y + int(quadPoints[1 + n].Y)][z + int(quadPoints[1 + n].Z)].normal;
			tri.Vertex2.NormalZ = -cubes[x + int(quadPoints[2 + n].X)][y + int(quadPoints[2 + n].Y)][z + int(quadPoints[2 + n].Z)].normal;
		}
		else
		{
			tri.Vertex0.Position = cubes[x + int(quadPoints[0].X)][y + int(quadPoints[0].Y)][z + int(quadPoints[0].Z)].point;
			tri.Vertex2.Position = cubes[x + int(quadPoints[1 + n].X)][y + int(quadPoints[1 + n].Y)][z + int(quadPoints[1 + n].Z)].point;
			tri.Vertex1.Position = cubes[x + int(quadPoints[2 + n].X)][y + int(quadPoints[2 + n].Y)][z + int(quadPoints[2 + n].Z)].point;

			tri.Vertex0.NormalZ = -cubes[x + int(quadPoints[0].X)][y + int(quadPoints[0].Y)][z + int(quadPoints[0].Z)].normal;
			tri.Vertex2.NormalZ = -cubes[x + int(quadPoints[1 + n].X)][y + int(quadPoints[1 + n].Y)][z + int(quadPoints[1 + n].Z)].normal;
			tri.Vertex1.NormalZ = -cubes[x + int(quadPoints[2 + n].X)][y + int(quadPoints[2 + n].Y)][z + int(quadPoints[2 + n].Z)].normal;
		}

		//const FVector Edge01 = (tri.Vertex1.Position - tri.Vertex0.Position);
		//const FVector Edge02 = (tri.Vertex2.Position - tri.Vertex0.Position);

		tri.Vertex0.NormalX = FVector(tri.Vertex0.NormalZ.Z, tri.Vertex0.NormalZ.X, -tri.Vertex0.NormalZ.Y);
		tri.Vertex0.NormalY = (tri.Vertex0.NormalZ ^ tri.Vertex0.NormalX).SafeNormal();

		tri.Vertex1.NormalX = FVector(tri.Vertex1.NormalZ.Z, tri.Vertex1.NormalZ.X, -tri.Vertex1.NormalZ.Y);
		tri.Vertex1.NormalY = (tri.Vertex1.NormalZ ^ tri.Vertex1.NormalX).SafeNormal();

		tri.Vertex2.NormalX = FVector(tri.Vertex2.NormalZ.Z, tri.Vertex2.NormalZ.X, -tri.Vertex2.NormalZ.Y);
		tri.Vertex2.NormalY = (tri.Vertex2.NormalZ ^ tri.Vertex2.NormalX).SafeNormal();

		/*tri.Vertex0.NormalZ = (Edge02 ^ Edge01).SafeNormal();
		tri.Vertex0.NormalX = Edge01.SafeNormal();
		tri.Vertex0.NormalY = (tri.Vertex0.NormalZ ^ tri.Vertex0.NormalX).SafeNormal();

		tri.Vertex1.NormalZ = (Edge02 ^ Edge01).SafeNormal();
		tri.Vertex1.NormalX = Edge01.SafeNormal();
		tri.Vertex1.NormalY = (tri.Vertex1.NormalZ ^ tri.Vertex1.NormalX).SafeNormal();

		tri.Vertex2.NormalZ = (Edge02 ^ Edge01).SafeNormal();
		tri.Vertex2.NormalX = Edge01.SafeNormal();
		tri.Vertex2.NormalY = (tri.Vertex2.NormalZ ^ tri.Vertex2.NormalX).SafeNormal();*/

		tri.Vertex0.TextureCoordinate1 = FVector2D(0, 1.0);
		tri.Vertex0.TextureCoordinate2 = FVector2D(0, 0.0);
		tri.Vertex0.TextureCoordinate3 = FVector2D(0, 0.0);

		tri.Vertex1.TextureCoordinate1 = FVector2D(0, 0.0);
		tri.Vertex1.TextureCoordinate2 = FVector2D(0, 1.0);
		tri.Vertex1.TextureCoordinate3 = FVector2D(0, 0.0);

		tri.Vertex2.TextureCoordinate1 = FVector2D(0, 0.0);
		tri.Vertex2.TextureCoordinate2 = FVector2D(0, 0.0);
		tri.Vertex2.TextureCoordinate3 = FVector2D(0, 1.0);

		triangles.Add(tri);
	}
}

void AOrionVoxelBase::GenerateMeshFromChunks()
{
	TArray<FGeneratedMeshTriangle> triangles;
	FGeneratedMeshTriangle tri;
	FVector points[8];
	float enabled[8];
	uint8 types[8];
	FVector gradients[8];
	FVertexAndIndex xzPlane[2][VOXEL_X + 1][VOXEL_Z + 1];
	FVertexAndIndex yzPlane[2][VOXEL_Y + 1][VOXEL_Z + 1];

	GenerateMeshFromChunksDualContour();
	return;

	for (int32 i = 0; i < CHUNK_SIZE_XY; i++)
	{
		for (int32 j = 0; j < CHUNK_SIZE_XY; j++)
		{
			for (int32 k = 0; k < CHUNK_SIZE_Z; k++)
			{
				if (Chunks[i][j][k].bDirty == false || Chunks[i][j][k].bEmpty)
					continue;

				//location of our cube
				FVector Offset = FVector((i - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_X, (j - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_Y, (k - CHUNK_SIZE_Z / 2) * VOXEL_WIDTH * VOXEL_Z);

				bool bDoEdgeX = i < CHUNK_SIZE_XY - 1;
				bool bDoEdgeY = j < CHUNK_SIZE_XY - 1;
				bool bDoEdgeZ = k < CHUNK_SIZE_Z - 1;

				bool bDoNegEdgeX = i > 0 && i < CHUNK_SIZE_XY;
				bool bDoNegEdgeY = j > 0 && j < CHUNK_SIZE_XY;
				bool bDoNegEdgeZ = k > 0 && k < CHUNK_SIZE_Z;

				for (int32 x = 0; x < VOXEL_X; x++)
				{
					for (int32 l = 0; l < 2; l++)
					{
						for (int32 m = 0; m < VOXEL_Y + 1; m++)
						{
							for (int32 n = 0; n < VOXEL_Z + 1; n++)
							{
								if (x != 0 && l == 0)
								{
									yzPlane[0][m][n] = yzPlane[1][m][n];
									continue;
								}

								bool bIsEdgeX = x + 1 >= VOXEL_X;
								bool bIsEdgeY = m >= VOXEL_Y;
								bool bIsEdgeZ = n >= VOXEL_Z;

								bool bIsDoubleEdgeX = x + 1 >= VOXEL_X - 1;
								bool bIsDoubleEdgeY = m >= VOXEL_Y - 1;
								bool bIsDoubleEdgeZ = n >= VOXEL_Z - 1;

								bool bIsNegEdgeX = x + l == 0;
								bool bIsNegEdgeY = m == 0;
								bool bIsNegEdgeZ = n == 0;

								yzPlane[l][m][n].pos = Offset + FVector((x + l) * VOXEL_WIDTH, m * VOXEL_WIDTH, n * VOXEL_WIDTH);

								yzPlane[l][m][n].density = Chunks[(bDoEdgeX && bIsEdgeX) ? i + 1 : i][(bDoEdgeY && bIsEdgeY) ? j + 1 : j][(bDoEdgeZ && bIsEdgeZ) ? k + 1 : k].
									pVoxels[(bIsEdgeX && bDoEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][(bIsEdgeY && bDoEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][(bIsEdgeZ && bDoEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Density;

								yzPlane[l][m][n].type = Chunks[(bDoEdgeX && bIsEdgeX) ? i + 1 : i][(bDoEdgeY && bIsEdgeY) ? j + 1 : j][(bDoEdgeZ && bIsEdgeZ) ? k + 1 : k].
									pVoxels[(bIsEdgeX && bDoEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l))][(bIsEdgeY && bDoEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m))][(bIsEdgeZ && bDoEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n))].Type;

								yzPlane[l][m][n].gradient = FVector(Chunks[bDoEdgeX && (bIsDoubleEdgeX || bIsEdgeX) ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoEdgeX && bIsEdgeX ? 1 : (bDoEdgeX && bIsDoubleEdgeX ? 0 : x + l + 1)][bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m)][bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n)].Density -
									Chunks[bDoNegEdgeX && bIsNegEdgeX ? i - 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoNegEdgeX && bIsNegEdgeX ? VOXEL_X - 1 : (bIsNegEdgeX ? 0 : x + l - 1)][bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m)][bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n)].Density,
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bDoEdgeY && (bIsDoubleEdgeY || bIsEdgeY) ? j + 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l)][bDoEdgeY && bIsEdgeY ? 1 : (bDoEdgeY && bIsDoubleEdgeY ? 0 : m + 1)][bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n)].Density -
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bDoNegEdgeY && bIsNegEdgeY ? j - 1 : j][bIsEdgeZ && bDoEdgeZ ? k + 1 : k].pVoxels[bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l)][bDoNegEdgeY && bIsNegEdgeY ? VOXEL_Y - 1 : (bIsNegEdgeY ? 0 : m - 1)][bDoEdgeZ && bIsEdgeZ ? 0 : (bIsEdgeZ ? VOXEL_Z - 1 : n)].Density,
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bDoEdgeZ && (bIsDoubleEdgeZ || bIsEdgeZ) ? k + 1 : k].pVoxels[bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l)][bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m)][bDoEdgeZ && bIsEdgeZ ? 1 : (bDoEdgeZ && bIsDoubleEdgeZ ? 0 : n + 1)].Density -
									Chunks[bDoEdgeX && bIsEdgeX ? i + 1 : i][bIsEdgeY && bDoEdgeY ? j + 1 : j][bDoNegEdgeZ && bIsNegEdgeZ ? k - 1 : k].pVoxels[bDoEdgeX && bIsEdgeX ? 0 : (bIsEdgeX ? VOXEL_X - 1 : x + l)][bDoEdgeY && bIsEdgeY ? 0 : (bIsEdgeY ? VOXEL_Y - 1 : m)][bDoNegEdgeZ && bIsNegEdgeZ ? VOXEL_Z - 1 : (bIsNegEdgeZ ? 0 : n - 1)].Density);
							}
						}
					}

					for (int32 y = 0; y < VOXEL_Y; y++)
					{
						/*for (int32 l = 0; l < 2; l++)
						{
						for (int32 m = 0; m < VOXEL_X + 1; m++)
						{
						for (int32 n = 0; n < VOXEL_Z + 1; n++)
						{
						if (y != 0 && l == 0)
						{
						xzPlane[0][m][n] = xzPlane[1][m][n];
						continue;
						}

						bool bIsEdgeX = m >= VOXEL_X;
						bool bIsEdgeY = y >= VOXEL_Y;
						bool bIsEdgeZ = n >= VOXEL_Z;

						bool bIsDoubleEdgeX = m >= VOXEL_X + 1;
						bool bIsDoubleEdgeY = y >= VOXEL_Y + 1;
						bool bIsDoubleEdgeZ = n >= VOXEL_Z + 1;

						bool bIsNegEdgeX = m == 0;
						bool bIsNegEdgeY = y == 0;
						bool bIsNegEdgeZ = n == 0;

						xzPlane[0][m][n].pos = Offset + FVector(m * VOXEL_WIDTH, 0, n * VOXEL_WIDTH);

						xzPlane[0][m][n].density = Chunks[i + (bIsDoubleEdgeX || bIsEdgeX) ? 1 : 0][j + (bIsDoubleEdgeY || bIsEdgeY) ? 1 : 0][k + (bIsDoubleEdgeZ || bIsEdgeZ) ? 1 : 0].
						pVoxels[bIsDoubleEdgeX ? (1) : (bIsEdgeX ? 0 : x)][bIsDoubleEdgeY ? (1) : (bIsEdgeY ? 0 : m)][bIsDoubleEdgeZ ? (1) : (bIsEdgeZ ? 0 : n)].Density;

						xzPlane[0][m][n].gradient = FVector(0, 0, 1);

						}
						}
						}*/

						for (int32 z = 0; z < VOXEL_Z; z++)
						{
							/*bool bIsEdgeX = x >= VOXEL_X - 1;
							bool bIsEdgeY = y >= VOXEL_Y - 1;
							bool bIsEdgeZ = z >= VOXEL_Z - 1;

							bool bIsDoubleEdgeX = x >= VOXEL_X - 2;
							bool bIsDoubleEdgeY = y >= VOXEL_Y - 2;
							bool bIsDoubleEdgeZ = z >= VOXEL_Z - 2;

							bool bIsNegEdgeX = x == 0;
							bool bIsNegEdgeY = y == 0;
							bool bIsNegEdgeZ = z == 0;

							//marching cube algorithm
							//find our 8 points
							points[0] = Offset + FVector((x + 0) * VOXEL_WIDTH, (y + 0) * VOXEL_WIDTH, (z + 0) * VOXEL_WIDTH);
							enabled[0] = Chunks[i][j][k].pVoxels[x + 0][y + 0][z + 0].Density;
							//types[0] = Chunks[i][j][k].pVoxels[x + 0][y + 0][z + 0].Type;
							points[1] = Offset + FVector((x + 0) * VOXEL_WIDTH, (y + 1) * VOXEL_WIDTH, (z + 0) * VOXEL_WIDTH);
							enabled[1] = Chunks[i][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[x + 0][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][z + 0].Density;
							//types[1] = Chunks[i].pVoxels[x + 0][y + 1][z + 0].Type;
							points[2] = Offset + FVector((x + 0) * VOXEL_WIDTH, (y + 1) * VOXEL_WIDTH, (z + 1) * VOXEL_WIDTH);
							enabled[2] = Chunks[i][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x + 0][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density;
							//types[2] = Chunks[i].pVoxels[x + 0][y + 1][z + 1].Type;
							points[3] = Offset + FVector((x + 0) * VOXEL_WIDTH, (y + 0) * VOXEL_WIDTH, (z + 1) * VOXEL_WIDTH);
							enabled[3] = Chunks[i][j][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x + 0][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density;
							//types[3] = Chunks[i].pVoxels[x + 0][y + 0][z + 1].Type;
							points[4] = Offset + FVector((x + 1) * VOXEL_WIDTH, (y + 0) * VOXEL_WIDTH, (z + 0) * VOXEL_WIDTH);
							enabled[4] = Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][y + 0][z + 0].Density;
							//types[4] = Chunks[i].pVoxels[x + 1][y + 0][z + 0].Type;
							points[5] = Offset + FVector((x + 1) * VOXEL_WIDTH, (y + 1) * VOXEL_WIDTH, (z + 0) * VOXEL_WIDTH);
							enabled[5] = Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][z + 0].Density;
							//types[5] = Chunks[i].pVoxels[x + 1][y + 1][z + 0].Type;
							points[6] = Offset + FVector((x + 1) * VOXEL_WIDTH, (y + 1) * VOXEL_WIDTH, (z + 1) * VOXEL_WIDTH);
							enabled[6] = Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density;
							//types[6] = Chunks[i].pVoxels[x + 1][y + 1][z + 1].Type;
							points[7] = Offset + FVector((x + 1) * VOXEL_WIDTH, (y + 0) * VOXEL_WIDTH, (z + 1) * VOXEL_WIDTH);
							enabled[7] = Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density;
							//types[7] = Chunks[i].pVoxels[x + 1][y + 0][z + 1].Type;*/

							//fill our our points and enableds
							points[0] = yzPlane[0][y][z].pos;
							points[1] = yzPlane[0][y + 1][z].pos;
							points[2] = yzPlane[0][y + 1][z + 1].pos;
							points[3] = yzPlane[0][y][z + 1].pos;
							points[4] = yzPlane[1][y][z].pos;
							points[5] = yzPlane[1][y + 1][z].pos;
							points[6] = yzPlane[1][y + 1][z + 1].pos;
							points[7] = yzPlane[1][y][z + 1].pos;

							enabled[0] = yzPlane[0][y][z].density;
							enabled[1] = yzPlane[0][y + 1][z].density;
							enabled[2] = yzPlane[0][y + 1][z + 1].density;
							enabled[3] = yzPlane[0][y][z + 1].density;
							enabled[4] = yzPlane[1][y][z].density;
							enabled[5] = yzPlane[1][y + 1][z].density;
							enabled[6] = yzPlane[1][y + 1][z + 1].density;
							enabled[7] = yzPlane[1][y][z + 1].density;

							types[0] = yzPlane[0][y][z].type;
							types[1] = yzPlane[0][y + 1][z].type;
							types[2] = yzPlane[0][y + 1][z + 1].type;
							types[3] = yzPlane[0][y][z + 1].type;
							types[4] = yzPlane[1][y][z].type;
							types[5] = yzPlane[1][y + 1][z].type;
							types[6] = yzPlane[1][y + 1][z + 1].type;
							types[7] = yzPlane[1][y][z + 1].type;

							//convert this to our index
							int32 cubeIndex = 0;
							for (int32 n = 0; n < 8; n++)
							{
								if (enabled[n] < 0.0)
									cubeIndex |= (1 << n);
							}

							//if all 8 points are inside or outside, we can ignore it completely
							if (!edgeTable[cubeIndex])
								continue;

							gradients[0] = yzPlane[0][y][z].gradient;
							gradients[1] = yzPlane[0][y + 1][z].gradient;
							gradients[2] = yzPlane[0][y + 1][z + 1].gradient;
							gradients[3] = yzPlane[0][y][z + 1].gradient;
							gradients[4] = yzPlane[1][y][z].gradient;
							gradients[5] = yzPlane[1][y + 1][z].gradient;
							gradients[6] = yzPlane[1][y + 1][z + 1].gradient;
							gradients[7] = yzPlane[1][y][z + 1].gradient;

							/*gradients[0] = FVector(enabled[4] - (Chunks[i - (bIsNegEdgeX ? (bDoNegEdgeX ? 1 : 0) : 0)][j][k].pVoxels[bIsNegEdgeX ? (bDoNegEdgeX ? VOXEL_X - 1 : 0) : x - 1][y + 0][z + 0].Density),
							enabled[1] - (Chunks[i][j - (bIsNegEdgeY ? (bDoNegEdgeY ? 1 : 0) : 0)][k].pVoxels[x][bIsNegEdgeY ? (bDoNegEdgeY ? VOXEL_Y - 1 : 0) : y - 1][z + 0].Density),
							enabled[3] - (Chunks[i][j][k - (bIsNegEdgeZ ? (bDoNegEdgeZ ? 1 : 0) : 0)].pVoxels[x][y + 0][bIsNegEdgeZ ? (bDoNegEdgeZ ? VOXEL_Z - 1 : 0) : z - 1].Density));

							gradients[1] = FVector(enabled[5] - (Chunks[i - (bIsNegEdgeX ? (bDoNegEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[bIsNegEdgeX ? (bDoNegEdgeX ? VOXEL_X - 1 : 0) : x - 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][z + 0].Density),
							(Chunks[i][j + (bIsDoubleEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[x][bIsEdgeY ? (bDoEdgeY ? 1 : VOXEL_Y - 1) : (bIsDoubleEdgeY ? (bDoEdgeY ? 0 : VOXEL_Y - 1) : y + 2)][z + 0].Density) - enabled[0],
							enabled[2] - (Chunks[i][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k - (bIsNegEdgeZ ? (bDoNegEdgeZ ? 1 : 0) : 0)].pVoxels[x][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsNegEdgeZ ? (bDoNegEdgeZ ? VOXEL_Z - 1 : 0) : z - 1].Density));

							gradients[2] = FVector(enabled[6] - (Chunks[i - (bIsNegEdgeX ? (bDoNegEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsNegEdgeX ? (bDoNegEdgeX ? VOXEL_X - 1 : 0) : x - 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density),
							(Chunks[i][j + (bIsDoubleEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x][bIsEdgeY ? (bDoEdgeY ? 1 : VOXEL_Y - 1) : (bIsDoubleEdgeY ? (bDoEdgeY ? 0 : VOXEL_Y - 1) : y + 2)][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density) - enabled[3],
							(Chunks[i][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsDoubleEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 1 : VOXEL_Z - 1) : (bIsDoubleEdgeZ ? (bDoEdgeZ ? 0 : VOXEL_Z - 1) : z + 2)].Density) - enabled[1]);

							gradients[3] = FVector(enabled[7] - (Chunks[i - (bIsNegEdgeX ? (bDoNegEdgeX ? 1 : 0) : 0)][j][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsNegEdgeX ? (bDoNegEdgeX ? VOXEL_X - 1 : 0) : x - 1][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density),
							enabled[2] - (Chunks[i][j - (bIsNegEdgeY ? (bDoNegEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x][bIsNegEdgeY ? (bDoNegEdgeY ? VOXEL_Y - 1 : 0) : y - 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density),
							(Chunks[i][j][k + (bIsDoubleEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[x][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 1 : VOXEL_Z - 1) : (bIsDoubleEdgeZ ? (bDoEdgeZ ? 0 : VOXEL_Z - 1) : z + 2)].Density) - enabled[0]);

							gradients[4] = FVector((Chunks[i + (bIsDoubleEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 1 : VOXEL_X - 1) : (bIsDoubleEdgeX ? (bDoEdgeX ? 0 : VOXEL_X - 1) : x + 2)][y + 0][z + 0].Density) - enabled[0],
							enabled[5] - (Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j - (bIsNegEdgeY ? (bDoNegEdgeY ? 1 : 0) : 0)][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsNegEdgeY ? (bDoNegEdgeY ? VOXEL_Y - 1 : 0) : y - 1][z + 0].Density),
							enabled[7] - (Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k - (bIsNegEdgeZ ? (bDoNegEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][y][bIsNegEdgeZ ? (bDoNegEdgeZ ? VOXEL_Z - 1 : 0) : z - 1].Density));

							gradients[5] = FVector((Chunks[i + (bIsDoubleEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 1 : VOXEL_X - 1) : (bIsDoubleEdgeX ? (bDoEdgeX ? 0 : VOXEL_X - 1) : x + 2)][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][z + 0].Density) - enabled[1],
							(Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsDoubleEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 1 : VOXEL_Y - 1) : (bIsDoubleEdgeY ? (bDoEdgeY ? 0 : VOXEL_Y - 1) : y + 2)][z + 0].Density) - enabled[4],
							enabled[6] - (Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k - (bIsNegEdgeZ ? (bDoNegEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsNegEdgeZ ? (bDoNegEdgeZ ? VOXEL_Z - 1 : 0) : z - 1].Density));

							gradients[6] = FVector((Chunks[i + (bIsDoubleEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 1 : VOXEL_X - 1) : (bIsDoubleEdgeX ? (bDoEdgeX ? 0 : VOXEL_X - 1) : x + 2)][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density) - enabled[2],
							(Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsDoubleEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 1 : VOXEL_Y - 1) : (bIsDoubleEdgeY ? (bDoEdgeY ? 0 : VOXEL_Y - 1) : y + 2)][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density) - enabled[7],
							(Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j + (bIsEdgeY ? (bDoEdgeY ? 1 : 0) : 0)][k + (bIsDoubleEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsEdgeY ? (bDoEdgeY ? 0 : y + 0) : y + 1][bIsEdgeZ ? (bDoEdgeZ ? 1 : VOXEL_Z - 1) : (bIsDoubleEdgeZ ? (bDoEdgeZ ? 0 : VOXEL_Z - 1) : z + 2)].Density) - enabled[5]);

							gradients[7] = FVector((Chunks[i + (bIsDoubleEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 1 : VOXEL_X - 1) : (bIsDoubleEdgeX ? (bDoEdgeX ? 0 : VOXEL_X - 1) : x + 2)][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 0 : y + 0) : z + 1].Density) - enabled[3],
							enabled[6] - (Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j - (bIsNegEdgeY ? (bDoNegEdgeY ? 1 : 0) : 0)][k + (bIsEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][bIsNegEdgeY ? (bDoNegEdgeY ? VOXEL_Y - 1 : 0) : y - 1][bIsEdgeZ ? (bDoEdgeZ ? 0 : z + 0) : z + 1].Density),
							(Chunks[i + (bIsEdgeX ? (bDoEdgeX ? 1 : 0) : 0)][j][k + (bIsDoubleEdgeZ ? (bDoEdgeZ ? 1 : 0) : 0)].pVoxels[bIsEdgeX ? (bDoEdgeX ? 0 : x + 0) : x + 1][y + 0][bIsEdgeZ ? (bDoEdgeZ ? 1 : VOXEL_Z - 1) : (bIsDoubleEdgeZ ? (bDoEdgeZ ? 0 : VOXEL_Z - 1) : z + 2)].Density - enabled[4]));
							*/

							//get our edge intersection points
							FVoxelData EdgeVerts[12];
							if (edgeTable[cubeIndex] & 1) { EdgeVerts[0].pos = FindEdgeIntersection(points[0], points[1], enabled[0] / (enabled[0] - enabled[1])); EdgeVerts[0].type = enabled[0] < enabled[1] ? types[0] : types[1]; }
							if (edgeTable[cubeIndex] & 2) { EdgeVerts[1].pos = FindEdgeIntersection(points[1], points[2], enabled[1] / (enabled[1] - enabled[2])); EdgeVerts[1].type = enabled[1] < enabled[2] ? types[1] : types[2]; }
							if (edgeTable[cubeIndex] & 4) { EdgeVerts[2].pos = FindEdgeIntersection(points[2], points[3], enabled[2] / (enabled[2] - enabled[3])); EdgeVerts[2].type = enabled[2] < enabled[3] ? types[2] : types[3]; }
							if (edgeTable[cubeIndex] & 8) { EdgeVerts[3].pos = FindEdgeIntersection(points[3], points[0], enabled[3] / (enabled[3] - enabled[0])); EdgeVerts[3].type = enabled[3] < enabled[0] ? types[3] : types[0]; }
							if (edgeTable[cubeIndex] & 16) { EdgeVerts[4].pos = FindEdgeIntersection(points[4], points[5], enabled[4] / (enabled[4] - enabled[5])); EdgeVerts[4].type = enabled[4] < enabled[5] ? types[4] : types[5]; }
							if (edgeTable[cubeIndex] & 32) { EdgeVerts[5].pos = FindEdgeIntersection(points[5], points[6], enabled[5] / (enabled[5] - enabled[6])); EdgeVerts[5].type = enabled[5] < enabled[6] ? types[5] : types[6]; }
							if (edgeTable[cubeIndex] & 64) { EdgeVerts[6].pos = FindEdgeIntersection(points[6], points[7], enabled[6] / (enabled[6] - enabled[7])); EdgeVerts[6].type = enabled[6] < enabled[7] ? types[6] : types[7]; }
							if (edgeTable[cubeIndex] & 128) { EdgeVerts[7].pos = FindEdgeIntersection(points[7], points[4], enabled[7] / (enabled[7] - enabled[4])); EdgeVerts[7].type = enabled[7] < enabled[4] ? types[7] : types[4]; }
							if (edgeTable[cubeIndex] & 256) { EdgeVerts[8].pos = FindEdgeIntersection(points[0], points[4], enabled[0] / (enabled[0] - enabled[4])); EdgeVerts[8].type = enabled[0] < enabled[4] ? types[0] : types[4]; }
							if (edgeTable[cubeIndex] & 512) { EdgeVerts[9].pos = FindEdgeIntersection(points[1], points[5], enabled[1] / (enabled[1] - enabled[5])); EdgeVerts[9].type = enabled[1] < enabled[5] ? types[1] : types[5]; }
							if (edgeTable[cubeIndex] & 1024) { EdgeVerts[10].pos = FindEdgeIntersection(points[2], points[6], enabled[2] / (enabled[2] - enabled[6])); EdgeVerts[10].type = enabled[2] < enabled[6] ? types[2] : types[6]; }
							if (edgeTable[cubeIndex] & 2048) { EdgeVerts[11].pos = FindEdgeIntersection(points[3], points[7], enabled[3] / (enabled[3] - enabled[7])); EdgeVerts[11].type = enabled[3] < enabled[7] ? types[3] : types[7]; }

							FVector EdgeNormals[12];
							if (edgeTable[cubeIndex] & 1) EdgeNormals[0] = FindEdgeIntersection(gradients[0], gradients[1], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 2) EdgeNormals[1] = FindEdgeIntersection(gradients[1], gradients[2], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 4) EdgeNormals[2] = FindEdgeIntersection(gradients[2], gradients[3], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 8) EdgeNormals[3] = FindEdgeIntersection(gradients[3], gradients[0], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 16) EdgeNormals[4] = FindEdgeIntersection(gradients[4], gradients[5], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 32) EdgeNormals[5] = FindEdgeIntersection(gradients[5], gradients[6], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 64) EdgeNormals[6] = FindEdgeIntersection(gradients[6], gradients[7], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 128) EdgeNormals[7] = FindEdgeIntersection(gradients[7], gradients[4], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 256) EdgeNormals[8] = FindEdgeIntersection(gradients[0], gradients[4], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 512) EdgeNormals[9] = FindEdgeIntersection(gradients[1], gradients[5], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 1024) EdgeNormals[10] = FindEdgeIntersection(gradients[2], gradients[6], 0.5).SafeNormal();
							if (edgeTable[cubeIndex] & 2048) EdgeNormals[11] = FindEdgeIntersection(gradients[3], gradients[7], 0.5).SafeNormal();

							//fill out our triangles
							for (int32 n = 0; triTable[cubeIndex][n] != -1; n += 3)
							{
								tri.Vertex0.Position = EdgeVerts[triTable[cubeIndex][n + 2]].pos;
								tri.Vertex1.Position = EdgeVerts[triTable[cubeIndex][n + 1]].pos;
								tri.Vertex2.Position = EdgeVerts[triTable[cubeIndex][n + 0]].pos;

								/*const FVector Edge01 = (tri.Vertex1.Position - tri.Vertex0.Position);
								const FVector Edge02 = (tri.Vertex2.Position - tri.Vertex0.Position);

								tri.Vertex0.NormalZ = (Edge02 ^ Edge01).SafeNormal();
								tri.Vertex0.NormalX = Edge01.SafeNormal();
								tri.Vertex0.NormalY = (tri.Vertex0.NormalZ ^ tri.Vertex0.NormalX).SafeNormal();

								tri.Vertex1.NormalZ = (Edge02 ^ Edge01).SafeNormal();
								tri.Vertex1.NormalX = Edge01.SafeNormal();
								tri.Vertex1.NormalY = (tri.Vertex1.NormalZ ^ tri.Vertex1.NormalX).SafeNormal();

								tri.Vertex2.NormalZ = (Edge02 ^ Edge01).SafeNormal();
								tri.Vertex2.NormalX = Edge01.SafeNormal();
								tri.Vertex2.NormalY = (tri.Vertex2.NormalZ ^ tri.Vertex2.NormalX).SafeNormal();*/

								tri.Vertex0.NormalZ = -EdgeNormals[triTable[cubeIndex][n + 2]];
								tri.Vertex0.NormalX = FVector(tri.Vertex0.NormalZ.Z, tri.Vertex0.NormalZ.X, -tri.Vertex0.NormalZ.Y);
								tri.Vertex0.NormalY = (tri.Vertex0.NormalZ ^ tri.Vertex0.NormalX).SafeNormal();// FVector(tri.Vertex0.NormalZ.Y, -tri.Vertex0.NormalZ.X, tri.Vertex0.NormalZ.Z);

								tri.Vertex1.NormalZ = -EdgeNormals[triTable[cubeIndex][n + 1]];
								tri.Vertex1.NormalX = FVector(tri.Vertex1.NormalZ.Z, tri.Vertex1.NormalZ.X, -tri.Vertex1.NormalZ.Y);
								tri.Vertex1.NormalY = (tri.Vertex1.NormalZ ^ tri.Vertex1.NormalX).SafeNormal();// FVector(tri.Vertex0.NormalZ.Y, -tri.Vertex1.NormalZ.X, tri.Vertex1.NormalZ.Z);

								tri.Vertex2.NormalZ = -EdgeNormals[triTable[cubeIndex][n + 0]];
								tri.Vertex2.NormalX = FVector(tri.Vertex2.NormalZ.Z, tri.Vertex2.NormalZ.X, -tri.Vertex2.NormalZ.Y);
								tri.Vertex2.NormalY = (tri.Vertex2.NormalZ ^ tri.Vertex2.NormalX).SafeNormal();//  FVector(tri.Vertex0.NormalZ.Y, -tri.Vertex2.NormalZ.X, tri.Vertex2.NormalZ.Z);

								tri.Vertex0.TextureCoordinate1 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 2]].type) - 1.0, 1.0);
								tri.Vertex0.TextureCoordinate2 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 1]].type) - 1.0, 0.0);
								tri.Vertex0.TextureCoordinate3 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 0]].type) - 1.0, 0.0);

								tri.Vertex1.TextureCoordinate1 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 2]].type) - 1.0, 0.0);
								tri.Vertex1.TextureCoordinate2 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 1]].type) - 1.0, 1.0);
								tri.Vertex1.TextureCoordinate3 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 0]].type) - 1.0, 0.0);

								tri.Vertex2.TextureCoordinate1 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 2]].type) - 1.0, 0.0);
								tri.Vertex2.TextureCoordinate2 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 1]].type) - 1.0, 0.0);
								tri.Vertex2.TextureCoordinate3 = FVector2D(float(EdgeVerts[triTable[cubeIndex][n + 0]].type) - 1.0, 1.0);

								//tri.Vertex0.TextureCoordinate = FVector2D(EdgeVerts[triTable[cubeIndex][n + 2]].index1, EdgeVerts[triTable[cubeIndex][n + 2]].index2);
								//tri.Vertex1.TextureCoordinate = FVector2D(EdgeVerts[triTable[cubeIndex][n + 1]].index1, EdgeVerts[triTable[cubeIndex][n + 1]].index2);
								//tri.Vertex2.TextureCoordinate = FVector2D(EdgeVerts[triTable[cubeIndex][n + 0]].index1, EdgeVerts[triTable[cubeIndex][n + 0]].index2);

								triangles.Add(tri);
							}
						}
					}
				}

				//mesh[i][j][k]->SetGeneratedMeshTriangles(triangles, VoxMaterial);
				Chunks[i][j][k].bDirty = false;

				triangles.Empty();

				//return;
			}
		}
	}

	//mesh->SetGeneratedMeshTriangles(triangles,VoxMaterial);
}

void AOrionVoxelBase::DoHit(FVector pos, float Amount)
{
	//if (Chunks.Num() <= 0)
	//	return;

	//find the voxel closest to the hitpos
	FVector worldPos = FVector(0, 0, 0);// GetActorLocation();
	FRotator worldRot = FRotator(0, 0, 0);// GetActorRotation();

	FVector X = FVector(1, 0, 0);// GetActorForwardVector();
	FVector Y = FVector(0, 1, 0);// GetActorRightVector();
	FVector Z = FVector(0, 0, 1);// GetActorUpVector();

	FVector hitPos;

	hitPos.X = FVector::DotProduct(pos - worldPos, X) / VOXEL_WIDTH;
	hitPos.Y = FVector::DotProduct(pos - worldPos, Y) / VOXEL_WIDTH;
	hitPos.Z = FVector::DotProduct(pos - worldPos, Z) / VOXEL_WIDTH;

	int32 nX, nY, nZ;

	//FVector Offset = FVector((i - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_X, (j - CHUNK_SIZE_XY / 2) * VOXEL_WIDTH * VOXEL_Y, (k - CHUNK_SIZE_Z / 2) * VOXEL_WIDTH * VOXEL_Z);

	FVector Center = FVector(-CHUNK_SIZE_XY * VOXEL_X / 2, -CHUNK_SIZE_XY * VOXEL_Y / 2, -CHUNK_SIZE_Z * VOXEL_Z / 2);

	nX = int32(FMath::RoundToFloat(hitPos.X) + 0.0) - (int32(Center.X));// *VOXEL_X;
	nY = int32(FMath::RoundToFloat(hitPos.Y) + 0.0) - (int32(Center.Y));// *VOXEL_Y;
	nZ = int32(FMath::RoundToFloat(hitPos.Z) + 0.0) - (int32(Center.Z));// *VOXEL_Z;

	//Chunks[nX / CHUNK_SIZE_XY][nY / CHUNK_SIZE_XY][nZ / CHUNK_SIZE_Z].pVoxels[nX % CHUNK_SIZE_XY][nY % CHUNK_SIZE_XY][nZ % CHUNK_SIZE_Z].Density;

	//don't do the edges until we start adding more chunks
	//if (nX < 1 || nY < 1 || nZ < 1 || nX >= VOXEL_X - 1 || nY >= VOXEL_Y - 1 || nZ >= VOXEL_Z - 1)
	//	return;

	/*if (Chunks[0][0][0].pVoxels[nX][nY][nZ].Type == VOXEL_NONE)
	{
	FVector Center = FVector(nX, nY, nZ)*VOXEL_WIDTH;// -FVector(0, (VOXEL_Y / 2 - 3)*VOXEL_WIDTH, (VOXEL_Z / 2 - 4)*VOXEL_WIDTH);

	FVector closestNeighbor = Center;
	FVector tempNeighbor;
	float BestDist;
	float tempDist;

	BestDist = 10000.0;
	if (Chunks[0][0][0].pVoxels[nX + 1][nY][nZ].Type>0) { closestNeighbor = Center + FVector(VOXEL_WIDTH, 0, 0); BestDist = (closestNeighbor - Center).SizeSquared(); }
	if (Chunks[0][0][0].pVoxels[nX - 1][nY][nZ].Type>0) { tempNeighbor = Center + FVector(-VOXEL_WIDTH, 0, 0); tempDist = (tempNeighbor - Center).SizeSquared(); if (tempDist < BestDist) { BestDist = tempDist; closestNeighbor = tempNeighbor; } }
	if (Chunks[0][0][0].pVoxels[nX][nY + 1][nZ].Type>0) { tempNeighbor = Center + FVector(0, VOXEL_WIDTH, 0); tempDist = (tempNeighbor - Center).SizeSquared(); if (tempDist < BestDist) { BestDist = tempDist; closestNeighbor = tempNeighbor; } }
	if (Chunks[0][0][0].pVoxels[nX][nY - 1][nZ].Type>0) { tempNeighbor = Center + FVector(0, -VOXEL_WIDTH, 0); tempDist = (tempNeighbor - Center).SizeSquared(); if (tempDist < BestDist) { BestDist = tempDist; closestNeighbor = tempNeighbor; } }
	if (Chunks[0][0][0].pVoxels[nX][nY][nZ + 1].Type>0) { tempNeighbor = Center + FVector(0, 0, VOXEL_WIDTH); tempDist = (tempNeighbor - Center).SizeSquared(); if (tempDist < BestDist) { BestDist = tempDist; closestNeighbor = tempNeighbor; } }
	if (Chunks[0][0][0].pVoxels[nX][nY][nZ - 1].Type>0) { tempNeighbor = Center + FVector(0, 0, -VOXEL_WIDTH); tempDist = (tempNeighbor - Center).SizeSquared(); if (tempDist < BestDist) { BestDist = tempDist; closestNeighbor = tempNeighbor; } }

	nX = closestNeighbor.X / VOXEL_WIDTH;
	nY = closestNeighbor.Y / VOXEL_WIDTH;
	nZ = closestNeighbor.Z / VOXEL_WIDTH;
	}
	//else return;

	if (nX < 1 || nY < 1 || nZ < 1 || nX >= VOXEL_X - 1 || nY >= VOXEL_Y - 1 || nZ >= VOXEL_Z - 1)
	return;*/

	//damage done to a voxel is based on the distance from it
	FVector voxCenter = FVector((nX / VOXEL_X) * VOXEL_WIDTH + (nX % VOXEL_X) * VOXEL_WIDTH,
		(nY / VOXEL_Y) * VOXEL_WIDTH + (nY % VOXEL_Y) * VOXEL_WIDTH,
		(nZ / VOXEL_Z) * VOXEL_WIDTH + (nZ % VOXEL_Z) * VOXEL_WIDTH);

	Chunks[nX / VOXEL_X][nY / VOXEL_Y][nZ / VOXEL_Z].pVoxels[nX % VOXEL_X][nY % VOXEL_Y][nZ % VOXEL_Z].Density -= 0.05f;// FMath::Max(0.0f, VOXEL_WIDTH * 1.5f - (VOXEL_WIDTH * 1.5f / (voxCenter - pos).Size())) * 0.15f;

	//subtract all it's neighbors?
	/*for (int32 x = -1; x <= 1; x++)
	{
	for (int32 y = -1; y <= 1; y++)
	{
	for (int32 z = -1; z <= 1; z++)
	{
	if (x == y == z == 0)
	continue;

	int32 rX = (nX % VOXEL_X) - x;
	rX = FMath::Clamp(rX, 0, VOXEL_X*CHUNK_SIZE_XY - 1);

	int32 rY = (nY % VOXEL_Y) - y;
	rY = FMath::Clamp(rY, 0, VOXEL_Y*CHUNK_SIZE_XY - 1);

	int32 rZ = (nZ % VOXEL_Z) - z;
	rZ = FMath::Clamp(rZ, 0, VOXEL_Z*CHUNK_SIZE_Z - 1);

	FVector vCenter = voxCenter + FVector(x, y, z)*VOXEL_WIDTH;

	Chunks[nX / VOXEL_X][nY / VOXEL_Y][nZ / VOXEL_Z].pVoxels[rX % VOXEL_X][rY % VOXEL_Y][rZ % VOXEL_Z].Density -= FMath::Max(0.0f, VOXEL_WIDTH * 1.5f - (VOXEL_WIDTH * 1.5f / (vCenter - pos).Size())) * 0.15f;
	}
	}
	}*/

	Chunks[nX / VOXEL_X][nY / VOXEL_Y][nZ / VOXEL_Z].pVoxels[nX % VOXEL_X][nY % VOXEL_Y][nZ % VOXEL_Z].Density = FMath::Max(-1.0f, Chunks[nX / VOXEL_X][nY / VOXEL_Y][nZ / VOXEL_Z].pVoxels[nX % VOXEL_X][nY % VOXEL_Y][nZ % VOXEL_Z].Density);

	//Chunks[0][0][0].pVoxels[nX][nY][nZ].Type = VOXEL_NONE;
	Chunks[nX / VOXEL_X][nY / VOXEL_Y][nZ / VOXEL_Z].bDirty = true;

	//update neighbors if needed
	if (nX % VOXEL_X == 0 && nX > 0)
		Chunks[(nX / VOXEL_X) - 1][nY / VOXEL_Y][nZ / VOXEL_Z].bDirty = true;
	else if (nX % VOXEL_X == VOXEL_X - 1 && nX < VOXEL_X * (CHUNK_SIZE_XY - 1))
		Chunks[(nX / VOXEL_X) + 1][nY / VOXEL_Y][nZ / VOXEL_Z].bDirty = true;
	else if (nY % VOXEL_Y == 0 && nY > 0)
		Chunks[nX / VOXEL_X][(nY / VOXEL_Y) - 1][nZ / VOXEL_Z].bDirty = true;
	else if (nY % VOXEL_Y == VOXEL_Y - 1 && nX < VOXEL_Y * (CHUNK_SIZE_XY - 1))
		Chunks[nX / VOXEL_X][(nY / VOXEL_Y) + 1][nZ / VOXEL_Z].bDirty = true;
	else if (nZ % VOXEL_Z == 0 && nZ > 0)
		Chunks[nX / VOXEL_X][nY / VOXEL_Y][(nZ / VOXEL_Z) - 1].bDirty = true;
	else if (nZ % VOXEL_Z == VOXEL_Z - 1 && nZ < VOXEL_Z * (CHUNK_SIZE_Z - 1))
		Chunks[nX / VOXEL_X][nY / VOXEL_Y][(nZ / VOXEL_Z) + 1].bDirty = true;

	//UOrionGeneratedMeshComponent* myMesh = Cast<UOrionGeneratedMeshComponent>(RootComponent);

	GenerateMeshFromChunks();// myMesh);
}

//----------------------------------------------------------------------------

#define MAXROWS 12
#define EPSILON 1e-5

//----------------------------------------------------------------------------

void QEF::evaluate(
	double mat[][3], double *vec, int rows,
	FVector *point)
{
	// perform singular value decomposition on matrix mat
	// into u, v and d.
	// u is a matrix of rows x 3 (same as mat);
	// v is a square matrix 3 x 3 (for 3 columns in mat);
	// d is vector of 3 values representing the diagonal
	// matrix 3 x 3 (for 3 colums in mat).
	double u[MAXROWS][3], v[3][3], d[3];
	computeSVD(mat, u, v, d, rows);

	// solve linear system given by mat and vec using the
	// singular value decomposition of mat into u, v and d.
	if (d[2] < 0.1)
		d[2] = 0.0;
	if (d[1] < 0.1)
		d[1] = 0.0;
	if (d[0] < 0.1)
		d[0] = 0.0;

	double x[3];
	solveSVD(u, v, d, vec, x, rows);

	*point = FVector((float)x[0], (float)x[1], (float)x[2]);
}

//----------------------------------------------------------------------------

void QEF::computeSVD(
	double mat[][3],                // matrix (rows x 3)
	double u[][3],                  // matrix (rows x 3)
	double v[3][3],                 // matrix (3x3)
	double d[3],                    // vector (1x3)
	int rows)
{
	memcpy(u, mat, rows * 3 * sizeof(double));

	double *tau_u = d;
	double tau_v[2];

	factorize(u, tau_u, tau_v, rows);

	unpack(u, v, tau_u, tau_v, rows);

	diagonalize(u, v, tau_u, tau_v, rows);

	singularize(u, v, tau_u, rows);
}

//----------------------------------------------------------------------------

void QEF::factorize(
	double mat[][3],                // matrix (rows x 3)
	double tau_u[3],                // vector, (1x3)
	double tau_v[2],                // vector, (1x2)
	int rows)
{
	int y;

	// bidiagonal factorization of (rows x 3) matrix into :-
	// tau_u, a vector of 1x3 (for 3 columns in the matrix)
	// tau_v, a vector of 1x2 (one less column than the matrix)

	for (int i = 0; i < 3; ++i) {

		// set up a vector to reference into the matrix
		// from mat(i,i) to mat(m,i), that is, from the
		// i'th column of the i'th row and down all the way
		// through that column
		double *ptrs[MAXROWS];
		int num_ptrs = rows - i;
		for (int q = 0; q < num_ptrs; ++q)
			ptrs[q] = &mat[q + i][i];

		// perform householder transformation on this vector
		double tau = factorize_hh(ptrs, num_ptrs);
		tau_u[i] = tau;

		// all computations below this point are performed
		// only for the first two columns:  i=0 or i=1
		if (i + 1 < 3) {

			// perform householder transformation on the matrix
			// mat(i,i+1) to mat(m,n), that is, on the sub-matrix
			// that begins in the (i+1)'th column of the i'th
			// row and extends to the end of the matrix at (m,n)
			if (tau != 0.0) {
				for (int x = i + 1; x < 3; ++x) {
					double wx = mat[i][x];
					for (y = i + 1; y < rows; ++y)
						wx += mat[y][x] * (*ptrs[y - i]);
					double tau_wx = tau * wx;
					mat[i][x] -= tau_wx;
					for (y = i + 1; y < rows; ++y)
						mat[y][x] -= tau_wx * (*ptrs[y - i]);
				}
			}

			// perform householder transformation on i'th row
			// (remember at this point, i is either 0 or 1)

			// set up a vector to reference into the matrix
			// from mat(i,i+1) to mat(i,n), that is, from the
			// (i+1)'th column of the i'th row and all the way
			// through to the end of that row
			ptrs[0] = &mat[i][i + 1];
			if (i == 0) {
				ptrs[1] = &mat[i][i + 2];
				num_ptrs = 2;
			}
			else // i == 1
				num_ptrs = 1;

			// perform householder transformation on this vector
			tau = factorize_hh(ptrs, num_ptrs);
			tau_v[i] = tau;

			// perform householder transformation on the sub-matrix
			// mat(i+1,i+1) to mat(m,n), that is, on the sub-matrix
			// that begins in the (i+1)'th column of the (i+1)'th
			// row and extends to the end of the matrix at (m,n)
			if (tau != 0.0) {
				for (y = i + 1; y < rows; ++y) {
					double wy = mat[y][i + 1];
					if (i == 0)
						wy += mat[y][i + 2] * (*ptrs[1]);
					double tau_wy = tau * wy;
					mat[y][i + 1] -= tau_wy;
					if (i == 0)
						mat[y][i + 2] -= tau_wy * (*ptrs[1]);
				}
			}

		}  // if (i + 1 < 3)
	}
}

//----------------------------------------------------------------------------

double QEF::factorize_hh(double *ptrs[], int n)
{
	double tau = 0.0;

	if (n > 1) {
		double xnorm;
		if (n == 2)
			xnorm = fabs(*ptrs[1]);
		else {
			double scl = 0.0;
			double ssq = 1.0;
			for (int i = 1; i < n; ++i) {
				double x = fabs(*ptrs[i]);
				if (x != 0.0) {
					if (scl < x) {
						ssq = 1.0 + ssq * (scl / x) * (scl / x);
						scl = x;
					}
					else
						ssq += (x / scl) * (x / scl);
				}
			}
			xnorm = scl * sqrt(ssq);
		}

		if (xnorm != 0.0) {
			double alpha = *ptrs[0];
			double beta = sqrt(alpha * alpha + xnorm * xnorm);
			if (alpha >= 0.0)
				beta = -beta;
			tau = (beta - alpha) / beta;

			double scl = 1.0 / (alpha - beta);
			*ptrs[0] = beta;
			for (int i = 1; i < n; ++i)
				*ptrs[i] *= scl;
		}
	}

	return tau;
}

//----------------------------------------------------------------------------

void QEF::unpack(
	double u[][3],                  // matrix (rows x 3)
	double v[3][3],                 // matrix (3x3)
	double tau_u[3],                // vector, (1x3)
	double tau_v[2],                // vector, (1x2)
	int rows)
{
	int i, y;

	// reset v to the identity matrix
	v[0][0] = v[1][1] = v[2][2] = 1.0;
	v[0][1] = v[0][2] = v[1][0] = v[1][2] = v[2][0] = v[2][1] = 0.0;

	for (i = 1; i >= 0; --i) {
		double tau = tau_v[i];

		// perform householder transformation on the sub-matrix
		// v(i+1,i+1) to v(m,n), that is, on the sub-matrix of v
		// that begins in the (i+1)'th column of the (i+1)'th row
		// and extends to the end of the matrix at (m,n).  the
		// householder vector used to perform this is the vector
		// from u(i,i+1) to u(i,n)
		if (tau != 0.0) {
			for (int x = i + 1; x < 3; ++x) {
				double wx = v[i + 1][x];
				for (y = i + 1 + 1; y < 3; ++y)
					wx += v[y][x] * u[i][y];
				double tau_wx = tau * wx;
				v[i + 1][x] -= tau_wx;
				for (y = i + 1 + 1; y < 3; ++y)
					v[y][x] -= tau_wx * u[i][y];
			}
		}
	}

	// copy superdiagonal of u into tau_v
	for (i = 0; i < 2; ++i)
		tau_v[i] = u[i][i + 1];

	// below, same idea for u:  householder transformations
	// and the superdiagonal copy

	for (i = 2; i >= 0; --i) {
		// copy superdiagonal of u into tau_u
		double tau = tau_u[i];
		tau_u[i] = u[i][i];

		// perform householder transformation on the sub-matrix
		// u(i,i) to u(m,n), that is, on the sub-matrix of u that
		// begins in the i'th column of the i'th row and extends
		// to the end of the matrix at (m,n).  the householder
		// vector used to perform this is the i'th column of u,
		// that is, u(0,i) to u(m,i)
		if (tau == 0.0) {
			u[i][i] = 1.0;
			if (i < 2) {
				u[i][2] = 0.0;
				if (i < 1)
					u[i][1] = 0.0;
			}
			for (y = i + 1; y < rows; ++y)
				u[y][i] = 0.0;
		}
		else {
			for (int x = i + 1; x < 3; ++x) {
				double wx = 0.0;
				for (y = i + 1; y < rows; ++y)
					wx += u[y][x] * u[y][i];
				double tau_wx = tau * wx;
				u[i][x] = -tau_wx;
				for (y = i + 1; y < rows; ++y)
					u[y][x] -= tau_wx * u[y][i];
			}
			for (y = i + 1; y < rows; ++y)
				u[y][i] = u[y][i] * -tau;
			u[i][i] = 1.0 - tau;
		}
	}
}

//----------------------------------------------------------------------------

void QEF::diagonalize(
	double u[][3],                  // matrix (rows x 3)
	double v[3][3],                 // matrix (3x3)
	double tau_u[3],                // vector, (1x3)
	double tau_v[2],                // vector, (1x2)
	int rows)
{
	int i, j;

	chop(tau_u, tau_v, 3);

	// progressively reduce the matrices into diagonal form

	int b = 3 - 1;
	while (b > 0) {
		if (tau_v[b - 1] == 0.0)
			--b;
		else {
			int a = b - 1;
			while (a > 0 && tau_v[a - 1] != 0.0)
				--a;
			int n = b - a + 1;

			double u1[MAXROWS][3];
			double v1[3][3];
			for (j = a; j <= b; ++j) {
				for (i = 0; i < rows; ++i)
					u1[i][j - a] = u[i][j];
				for (i = 0; i < 3; ++i)
					v1[i][j - a] = v[i][j];
			}

			qrstep(u1, v1, &tau_u[a], &tau_v[a], rows, n);

			for (j = a; j <= b; ++j) {
				for (i = 0; i < rows; ++i)
					u[i][j] = u1[i][j - a];
				for (i = 0; i < 3; ++i)
					v[i][j] = v1[i][j - a];
			}

			chop(&tau_u[a], &tau_v[a], n);
		}
	}
}

//----------------------------------------------------------------------------

void QEF::chop(double *a, double *b, int n)
{
	double ai = a[0];
	for (int i = 0; i < n - 1; ++i) {
		double bi = b[i];
		double ai1 = a[i + 1];
		if (fabs(bi) < EPSILON * (fabs(ai) + fabs(ai1)))
			b[i] = 0.0;
		ai = ai1;
	}
}

//----------------------------------------------------------------------------

void QEF::qrstep(
	double u[][3],                  // matrix (rows x cols)
	double v[][3],                  // matrix (3 x cols)
	double tau_u[],                 // vector (1 x cols)
	double tau_v[],                 // vector (1 x cols - 1)
	int rows, int cols)
{
	int i;

	if (cols == 2) {
		qrstep_cols2(u, v, tau_u, tau_v, rows);
		return;
	}

	if (cols == 1) {
		char *bomb = 0;
		*bomb = 0;
	}

	// handle zeros on the diagonal or at its end
	for (i = 0; i < cols - 1; ++i)
	if (tau_u[i] == 0.0) {
		qrstep_middle(u, tau_u, tau_v, rows, cols, i);
		return;
	}
	if (tau_u[cols - 1] == 0.0) {
		qrstep_end(v, tau_u, tau_v, cols);
		return;
	}

	// perform qr reduction on the diagonal and off-diagonal

	double mu = qrstep_eigenvalue(tau_u, tau_v, cols);
	double y = tau_u[0] * tau_u[0] - mu;
	double z = tau_u[0] * tau_v[0];

	double ak = 0.0;
	double bk = 0.0;
	double zk;
	double ap = tau_u[0];
	double bp = tau_v[0];
	double aq = tau_u[1];
	double bq = tau_v[1];

	for (int k = 0; k < cols - 1; ++k) {
		double c, s;

		// perform Givens rotation on V

		computeGivens(y, z, &c, &s);

		for (i = 0; i < 3; ++i) {
			double vip = v[i][k];
			double viq = v[i][k + 1];
			v[i][k] = vip * c - viq * s;
			v[i][k + 1] = vip * s + viq * c;
		}

		// perform Givens rotation on B

		double bk1 = bk * c - z * s;
		double ap1 = ap * c - bp * s;
		double bp1 = ap * s + bp * c;
		double zp1 = aq * -s;
		double aq1 = aq * c;

		if (k > 0)
			tau_v[k - 1] = bk1;

		ak = ap1;
		bk = bp1;
		zk = zp1;
		ap = aq1;

		if (k < cols - 2)
			bp = tau_v[k + 1];
		else
			bp = 0.0;

		y = ak;
		z = zk;

		// perform Givens rotation on U

		computeGivens(y, z, &c, &s);

		for (i = 0; i < rows; ++i) {
			double uip = u[i][k];
			double uiq = u[i][k + 1];
			u[i][k] = uip * c - uiq * s;
			u[i][k + 1] = uip * s + uiq * c;
		}

		// perform Givens rotation on B

		double ak1 = ak * c - zk * s;
		bk1 = bk * c - ap * s;
		double zk1 = bp * -s;

		ap1 = bk * s + ap * c;
		bp1 = bp * c;

		tau_u[k] = ak1;

		ak = ak1;
		bk = bk1;
		zk = zk1;
		ap = ap1;
		bp = bp1;

		if (k < cols - 2)
			aq = tau_u[k + 2];
		else
			aq = 0.0;

		y = bk;
		z = zk;
	}

	tau_v[cols - 2] = bk;
	tau_u[cols - 1] = ap;
}

//----------------------------------------------------------------------------

void QEF::qrstep_middle(
	double u[][3],                  // matrix (rows x cols)
	double tau_u[],                 // vector (1 x cols)
	double tau_v[],                 // vector (1 x cols - 1)
	int rows, int cols, int col)
{
	double x = tau_v[col];
	double y = tau_u[col + 1];
	for (int j = col; j < cols - 1; ++j) {
		double c, s;

		// perform Givens rotation on U

		computeGivens(y, -x, &c, &s);
		for (int i = 0; i < rows; ++i) {
			double uip = u[i][col];
			double uiq = u[i][j + 1];
			u[i][col] = uip * c - uiq * s;
			u[i][j + 1] = uip * s + uiq * c;
		}

		// perform transposed Givens rotation on B

		tau_u[j + 1] = x * s + y * c;
		if (j == col)
			tau_v[j] = x * c - y * s;

		if (j < cols - 2) {
			double z = tau_v[j + 1];
			tau_v[j + 1] *= c;
			x = z * -s;
			y = tau_u[j + 2];
		}
	}
}

//----------------------------------------------------------------------------

void QEF::qrstep_end(
	double v[][3],                  // matrix (3 x 3)
	double tau_u[],                 // vector (1 x 3)
	double tau_v[],                 // vector (1 x 2)
	int cols)
{
	double x = tau_u[1];
	double y = tau_v[1];

	for (int k = 1; k >= 0; --k) {
		double c, s;

		// perform Givens rotation on V

		computeGivens(x, y, &c, &s);

		for (int i = 0; i < 3; ++i) {
			double vip = v[i][k];
			double viq = v[i][2];
			v[i][k] = vip * c - viq * s;
			v[i][2] = vip * s + viq * c;
		}

		// perform Givens rotation on B

		tau_u[k] = x * c - y * s;
		if (k == 1)
			tau_v[k] = x * s + y * c;
		if (k > 0) {
			double z = tau_v[k - 1];
			tau_v[k - 1] *= c;

			x = tau_u[k - 1];
			y = z * s;
		}
	}
}

//----------------------------------------------------------------------------

double QEF::qrstep_eigenvalue(
	double tau_u[],                 // vector (1 x 3)
	double tau_v[],                 // vector (1 x 2)
	int cols)
{
	double ta = tau_u[1] * tau_u[1] + tau_v[0] * tau_v[0];
	double tb = tau_u[2] * tau_u[2] + tau_v[1] * tau_v[1];
	double tab = tau_u[1] * tau_v[1];
	double dt = (ta - tb) / 2.0;
	double mu;
	if (dt >= 0.0)
		mu = tb - (tab * tab) / (dt + sqrt(dt * dt + tab * tab));
	else
		mu = tb + (tab * tab) / (sqrt(dt * dt + tab * tab) - dt);
	return mu;
}

//----------------------------------------------------------------------------

void QEF::qrstep_cols2(
	double u[][3],                  // matrix (rows x 2)
	double v[][3],                  // matrix (3 x 2)
	double tau_u[],                 // vector (1 x 2)
	double tau_v[],                 // vector (1 x 1)
	int rows)
{
	int i;
	double tmp;

	// eliminate off-diagonal element in [ 0  tau_v0 ]
	//                                   [ 0  tau_u1 ]
	// to make [ tau_u[0]  0 ]
	//         [ 0         0 ]

	if (tau_u[0] == 0.0) {
		double c, s;

		// perform transposed Givens rotation on B
		// multiplied by X = [ 0 1 ]
		//                   [ 1 0 ]

		computeGivens(tau_v[0], tau_u[1], &c, &s);

		tau_u[0] = tau_v[0] * c - tau_u[1] * s;
		tau_v[0] = tau_v[0] * s + tau_u[1] * c;
		tau_u[1] = 0.0;

		// perform Givens rotation on U

		for (i = 0; i < rows; ++i) {
			double uip = u[i][0];
			double uiq = u[i][1];
			u[i][0] = uip * c - uiq * s;
			u[i][1] = uip * s + uiq * c;
		}

		// multiply V by X, effectively swapping first two columns

		for (i = 0; i < 3; ++i) {
			tmp = v[i][0];
			v[i][0] = v[i][1];
			v[i][1] = tmp;
		}
	}

	// eliminate off-diagonal element in [ tau_u0  tau_v0 ]
	//                                   [ 0       0      ]

	else if (tau_u[1] == 0.0) {
		double c, s;

		// perform Givens rotation on B

		computeGivens(tau_u[0], tau_v[0], &c, &s);

		tau_u[0] = tau_u[0] * c - tau_v[0] * s;
		tau_v[0] = 0.0;

		// perform Givens rotation on V

		for (i = 0; i < 3; ++i) {
			double vip = v[i][0];
			double viq = v[i][1];
			v[i][0] = vip * c - viq * s;
			v[i][1] = vip * s + viq * c;
		}
	}

	// make colums orthogonal,

	else {
		double c, s;

		// perform Schur rotation on B

		computeSchur(tau_u[0], tau_v[0], tau_u[1], &c, &s);

		double a11 = tau_u[0] * c - tau_v[0] * s;
		double a21 = -tau_u[1] * s;
		double a12 = tau_u[0] * s + tau_v[0] * c;
		double a22 = tau_u[1] * c;

		// perform Schur rotation on V

		for (i = 0; i < 3; ++i) {
			double vip = v[i][0];
			double viq = v[i][1];
			v[i][0] = vip * c - viq * s;
			v[i][1] = vip * s + viq * c;
		}

		// eliminate off diagonal elements

		if ((a11 * a11 + a21 * a21) < (a12 * a12 + a22 * a22)) {

			// multiply B by X

			tmp = a11;
			a11 = a12;
			a12 = tmp;
			tmp = a21;
			a21 = a22;
			a22 = tmp;

			// multiply V by X, effectively swapping first
			// two columns

			for (i = 0; i < 3; ++i) {
				tmp = v[i][0];
				v[i][0] = v[i][1];
				v[i][1] = tmp;
			}
		}

		// perform transposed Givens rotation on B

		computeGivens(a11, a21, &c, &s);

		tau_u[0] = a11 * c - a21 * s;
		tau_v[0] = a12 * c - a22 * s;
		tau_u[1] = a12 * s + a22 * c;

		// perform Givens rotation on U

		for (i = 0; i < rows; ++i) {
			double uip = u[i][0];
			double uiq = u[i][1];
			u[i][0] = uip * c - uiq * s;
			u[i][1] = uip * s + uiq * c;
		}
	}
}

//----------------------------------------------------------------------------

void QEF::computeGivens(
	double a, double b, double *c, double *s)
{
	if (b == 0.0) {
		*c = 1.0;
		*s = 0.0;
	}
	else if (fabs(b) > fabs(a)) {
		double t = -a / b;
		double s1 = 1.0 / sqrt(1 + t * t);
		*s = s1;
		*c = s1 * t;
	}
	else {
		double t = -b / a;
		double c1 = 1.0 / sqrt(1 + t * t);
		*c = c1;
		*s = c1 * t;
	}
}

//----------------------------------------------------------------------------

void QEF::computeSchur(
	double a1, double a2, double a3,
	double *c, double *s)
{
	double apq = a1 * a2 * 2.0;

	if (apq == 0.0) {
		*c = 1.0;
		*s = 0.0;
	}
	else {
		double t;
		double tau = (a2 * a2 + (a3 + a1) * (a3 - a1)) / apq;
		if (tau >= 0.0)
			t = 1.0 / (tau + sqrt(1.0 + tau * tau));
		else
			t = -1.0 / (sqrt(1.0 + tau * tau) - tau);
		*c = 1.0 / sqrt(1.0 + t * t);
		*s = t * (*c);
	}
}

//----------------------------------------------------------------------------

void QEF::singularize(
	double u[][3],                  // matrix (rows x 3)
	double v[3][3],                 // matrix (3x3)
	double d[3],                    // vector, (1x3)
	int rows)
{
	int i, j, y;

	// make singularize values positive

	for (j = 0; j < 3; ++j)
	if (d[j] < 0.0) {
		for (i = 0; i < 3; ++i)
			v[i][j] = -v[i][j];
		d[j] = -d[j];
	}

	// sort singular values in decreasing order

	for (i = 0; i < 3; ++i) {
		double d_max = d[i];
		int i_max = i;
		for (j = i + 1; j < 3; ++j)
		if (d[j] > d_max) {
			d_max = d[j];
			i_max = j;
		}

		if (i_max != i) {
			// swap eigenvalues
			double tmp = d[i];
			d[i] = d[i_max];
			d[i_max] = tmp;

			// swap eigenvectors
			for (y = 0; y < rows; ++y) {
				tmp = u[y][i];
				u[y][i] = u[y][i_max];
				u[y][i_max] = tmp;
			}
			for (y = 0; y < 3; ++y) {
				tmp = v[y][i];
				v[y][i] = v[y][i_max];
				v[y][i_max] = tmp;
			}
		}
	}
}

//----------------------------------------------------------------------------

void QEF::solveSVD(
	double u[][3],                  // matrix (rows x 3)
	double v[3][3],                 // matrix (3x3)
	double d[3],                    // vector (1x3)
	double b[],                     // vector (1 x rows)
	double x[3],                    // vector (1x3)
	int rows)
{
	static double zeroes[3] = { 0.0, 0.0, 0.0 };

	int i, j;

	// compute vector w = U^T * b

	double w[3];
	memcpy(w, zeroes, sizeof(w));
	for (i = 0; i < rows; ++i) {
		if (b[i] != 0.0)
		for (j = 0; j < 3; ++j)
			w[j] += b[i] * u[i][j];
	}

	// introduce non-zero singular values in d into w

	for (i = 0; i < 3; ++i) {
		if (d[i] != 0.0)
			w[i] /= d[i];
	}

	// compute result vector x = V * w

	for (i = 0; i < 3; ++i) {
		double tmp = 0.0;
		for (j = 0; j < 3; ++j)
			tmp += w[j] * v[i][j];
		x[i] = tmp;
	}
}


