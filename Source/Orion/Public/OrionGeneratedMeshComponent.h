

#pragma once

#include "Components/MeshComponent.h"
#include "OrionGeneratedMeshComponent.generated.h"

struct FDynamicTerrainVertex
{
	FDynamicTerrainVertex() {}
	FDynamicTerrainVertex(const FVector& InPosition) :
		Position(InPosition),
		TextureCoordinate1(FVector2D::ZeroVector),
		TextureCoordinate2(FVector2D::ZeroVector),
		TextureCoordinate3(FVector2D::ZeroVector),
		TangentX(FVector(1, 0, 0)),
		TangentZ(FVector(0, 0, 1)),
		Color(FColor(255, 255, 255))
	{
		// basis determinant default to +1.0
		TangentZ.Vector.W = 255;
	}

	FDynamicTerrainVertex(const FVector& InPosition, const FVector& InTangentX, const FVector& InTangentZ, const FVector2D& InTexCoord1, const FVector2D& InTexCoord2, const FVector2D& InTexCoord3, const FColor& InColor) :
		Position(InPosition),
		TextureCoordinate1(InTexCoord1),
		TextureCoordinate2(InTexCoord2),
		TextureCoordinate3(InTexCoord3),
		TangentX(InTangentX),
		TangentZ(InTangentZ),
		Color(InColor)
	{
		// basis determinant default to +1.0
		TangentZ.Vector.W = 255;
	}

	void SetTangents(const FVector& InTangentX, const FVector& InTangentY, const FVector& InTangentZ)
	{
		TangentX = InTangentX;
		TangentZ = InTangentZ;
		// store determinant of basis in w component of normal vector
		TangentZ.Vector.W = GetBasisDeterminantSign(InTangentX, InTangentY, InTangentZ) < 0.0f ? 0 : 255;
	}

	FVector GetTangentY()
	{
		return (FVector(TangentZ) ^ FVector(TangentX)) * ((float)TangentZ.Vector.W / 127.5f - 1.0f);
	};

	FVector Position;
	FVector2D TextureCoordinate1;
	FVector2D TextureCoordinate2;
	FVector2D TextureCoordinate3;
	FPackedNormal TangentX;
	FPackedNormal TangentZ;
	FColor Color;
};

USTRUCT()
struct FVoxelVertex
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Vertex)
	FVector Position;

	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector NormalX;

	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector NormalY;

	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector NormalZ;

	//uvs store the index into which texture we want to use here
	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector2D TextureCoordinate1;

	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector2D TextureCoordinate2;

	UPROPERTY(EditAnywhere, Category = Vertex)
		FVector2D TextureCoordinate3;
};

USTRUCT(BlueprintType)
struct FGeneratedMeshTriangle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Triangle)
	FVoxelVertex Vertex0;

	UPROPERTY(EditAnywhere, Category = Triangle)
		FVoxelVertex Vertex1;

	UPROPERTY(EditAnywhere, Category = Triangle)
		FVoxelVertex Vertex2;
};

/** Component that allows you to specify custom triangle mesh geometry */
UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
class UOrionGeneratedMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()
public:
	UOrionGeneratedMeshComponent(const FObjectInitializer& ObejctInitializer);

	/** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
		bool SetGeneratedMeshTriangles(const TArray<FGeneratedMeshTriangle>& Triangles, UMaterial *newMaterial);

	/** Description of collision */
	UPROPERTY(BlueprintReadOnly, Category = "Collision")
	class UBodySetup* ModelBodySetup;

	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.

	// Begin Interface_CollisionDataProvider Interface
	virtual bool GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool InUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override{ return false; }
	// End Interface_CollisionDataProvider Interface

	void SetBoundsData(FVector center, int32 LOD);

	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.

	void UpdateBodySetup();
	void UpdateCollision();
private:

	FVector MyCenter;
	int32 MyLOD;

	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.

	/** */
	TArray<FGeneratedMeshTriangle> GeneratedMeshTris;

	friend class FGeneratedMeshSceneProxy;

	UMaterial *VoxMaterial;
};