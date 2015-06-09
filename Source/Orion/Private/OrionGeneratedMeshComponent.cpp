

#include "Orion.h"
#include "RHIDefinitions.h"
#include "OrionGeneratedMeshComponent.h"

/** Vertex Buffer */
class FGeneratedMeshVertexBuffer : public FVertexBuffer
{
public:
	TArray<FDynamicTerrainVertex> Vertices;

	virtual void InitRHI()
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FDynamicTerrainVertex), BUF_Static, CreateInfo);

		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FDynamicTerrainVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicTerrainVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}

};

/** Index Buffer */
class FGeneratedMeshIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI()
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);

		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

/** Vertex Factory */
class FGeneratedMeshVertexFactory : public FLocalVertexFactory
{
public:

	FGeneratedMeshVertexFactory()
	{}


	/** Initialization */
	void Init(const FGeneratedMeshVertexBuffer* VertexBuffer)
	{
		check(!IsInRenderingThread());

		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
			InitGeneratedMeshVertexFactory,
			FGeneratedMeshVertexFactory*, VertexFactory, this,
			const FGeneratedMeshVertexBuffer*, VertexBuffer, VertexBuffer,
			{
			// Initialize the vertex factory's stream components.
			DataType NewData;
			NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicTerrainVertex, Position, VET_Float3);
			NewData.TextureCoordinates.Add(
				FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicTerrainVertex, TextureCoordinate1), sizeof(FDynamicTerrainVertex), VET_Float2)
				);
			NewData.TextureCoordinates.Add(
				FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicTerrainVertex, TextureCoordinate2), sizeof(FDynamicTerrainVertex), VET_Float2)
				);
			NewData.TextureCoordinates.Add(
				FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicTerrainVertex, TextureCoordinate3), sizeof(FDynamicTerrainVertex), VET_Float2)
				);
			NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicTerrainVertex, TangentX, VET_PackedNormal);
			NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicTerrainVertex, TangentZ, VET_PackedNormal);
			NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicTerrainVertex, Color, VET_Color);
			VertexFactory->SetData(NewData);
		});
	}
};



//////////////////////////////////////////////////////////////////////////

UOrionGeneratedMeshComponent::UOrionGeneratedMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UOrionGeneratedMeshComponent::SetGeneratedMeshTriangles(const TArray<FGeneratedMeshTriangle>& Triangles, UMaterial *newMaterial)
{
	GeneratedMeshTris = Triangles;
	VoxMaterial = newMaterial;

	UpdateCollision();

	// Need to recreate scene proxy to send it over
	MarkRenderStateDirty();

	return true;
}


FPrimitiveSceneProxy* UOrionGeneratedMeshComponent::CreateSceneProxy()
{

	/** Scene proxy defined only inside the scope of this one function */
	class FGeneratedMeshSceneProxy : public FPrimitiveSceneProxy
	{
	public:

		FGeneratedMeshSceneProxy(UOrionGeneratedMeshComponent* Component)
			: FPrimitiveSceneProxy(Component)
			, MaterialRelevance(Component->GetMaterialRelevance(ERHIFeatureLevel::SM4))
		{
			//const FColor VertexColor(255, 255, 255);

			// Add each triangle to the vertex/index buffer
			for (int TriIdx = 0; TriIdx<Component->GeneratedMeshTris.Num(); TriIdx++)
			{
				FGeneratedMeshTriangle& Tri = Component->GeneratedMeshTris[TriIdx];

				const FVector Edge01 = (Tri.Vertex1.Position - Tri.Vertex0.Position);
				const FVector Edge02 = (Tri.Vertex2.Position - Tri.Vertex0.Position);

				FVector FlatZ = ((Edge02 ^ Edge01).GetSafeNormal() + 1.0) / 2.0 * 255;
				FColor VertexColor(FlatZ.X, FlatZ.Y, FlatZ.Z);

				FVector TangentX = Tri.Vertex0.NormalX;
				FVector TangentY = Tri.Vertex0.NormalY;
				FVector TangentZ = Tri.Vertex0.NormalZ;

				FDynamicTerrainVertex Vert0;
				Vert0.Position = Tri.Vertex0.Position;
				Vert0.Color = VertexColor;
				Vert0.SetTangents(TangentX, TangentY, TangentZ);
				Vert0.TextureCoordinate1 = Tri.Vertex0.TextureCoordinate1;
				Vert0.TextureCoordinate2 = Tri.Vertex0.TextureCoordinate2;
				Vert0.TextureCoordinate3 = Tri.Vertex0.TextureCoordinate3;
				int32 VIndex = VertexBuffer.Vertices.Add(Vert0);
				IndexBuffer.Indices.Add(VIndex);

				TangentX = Tri.Vertex1.NormalX;
				TangentZ = Tri.Vertex1.NormalZ;
				TangentY = Tri.Vertex1.NormalY;

				FDynamicTerrainVertex Vert1;
				Vert1.Position = Tri.Vertex1.Position;
				Vert1.Color = VertexColor;
				Vert1.SetTangents(TangentX, TangentY, TangentZ);
				Vert1.TextureCoordinate1 = Tri.Vertex1.TextureCoordinate1;
				Vert1.TextureCoordinate2 = Tri.Vertex1.TextureCoordinate2;
				Vert1.TextureCoordinate3 = Tri.Vertex1.TextureCoordinate3;
				VIndex = VertexBuffer.Vertices.Add(Vert1);
				IndexBuffer.Indices.Add(VIndex);

				TangentX = Tri.Vertex2.NormalX;
				TangentZ = Tri.Vertex2.NormalZ;
				TangentY = Tri.Vertex2.NormalY;

				FDynamicTerrainVertex Vert2;
				Vert2.Position = Tri.Vertex2.Position;
				Vert2.Color = VertexColor;
				Vert2.SetTangents(TangentX, TangentY, TangentZ);
				Vert2.TextureCoordinate1 = Tri.Vertex2.TextureCoordinate1;
				Vert2.TextureCoordinate2 = Tri.Vertex2.TextureCoordinate2;
				Vert2.TextureCoordinate3 = Tri.Vertex2.TextureCoordinate3;
				VIndex = VertexBuffer.Vertices.Add(Vert2);
				IndexBuffer.Indices.Add(VIndex);
			}

			// Init vertex factory
			VertexFactory.Init(&VertexBuffer);

			// Enqueue initialization of render resource
			BeginInitResource(&VertexBuffer);
			BeginInitResource(&IndexBuffer);
			BeginInitResource(&VertexFactory);

			// Grab material
			Material = Component->GetMaterial(0);
			if (Material == NULL)
			{
				Material = Component->VoxMaterial;
			}

			Component->UpdateCollision();

			Component->GeneratedMeshTris.Empty();

			//Component->SetMobility(EComponentMobility::Stationary);
		}

		virtual ~FGeneratedMeshSceneProxy()
		{
			VertexBuffer.ReleaseResource();
			IndexBuffer.ReleaseResource();
			VertexFactory.ReleaseResource();
		}

		virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GeneratedMeshSceneProxy_DrawDynamicElements);

			const bool bWireframe = View->Family->EngineShowFlags.Wireframe;

			FColoredMaterialRenderProxy WireframeMaterialInstance(
				WITH_EDITOR ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				//GEngine->WireframeMaterial->GetRenderProxy(IsSelected()),
				FLinearColor(0, 0.5f, 1.f)
				);

			FMaterialRenderProxy* MaterialProxy = NULL;
			if (bWireframe)
			{
				MaterialProxy = &WireframeMaterialInstance;
			}
			else if (Material)
			{
				MaterialProxy = Material->GetRenderProxy(IsSelected());
			}
			else
				return;

			// Draw the mesh.
			FMeshBatch Mesh;
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = bWireframe;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;
			BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, true);
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			PDI->DrawMesh(Mesh);
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View);
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bDynamicRelevance = true;
			MaterialRelevance.SetPrimitiveViewRelevance(Result);
			return Result;
		}

		virtual bool CanBeOccluded() const override
		{
			return !MaterialRelevance.bDisableDepthTest;
		}

		virtual uint32 GetMemoryFootprint(void) const { return(sizeof(*this) + GetAllocatedSize()); }

		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:

		UMaterialInterface* Material;
		FGeneratedMeshVertexBuffer VertexBuffer;
		FGeneratedMeshIndexBuffer IndexBuffer;
		FGeneratedMeshVertexFactory VertexFactory;

		FMaterialRelevance MaterialRelevance;
	};


	//Only create if have enough tris
	if (GeneratedMeshTris.Num() > 0)
	{
		return new FGeneratedMeshSceneProxy(this);
	}
	else
	{
		return nullptr;
	}
}

int32 UOrionGeneratedMeshComponent::GetNumMaterials() const
{
	return 1;
}

void UOrionGeneratedMeshComponent::SetBoundsData(FVector center, int32 LOD)
{
	MyCenter = center;
	MyLOD = LOD;
}

FBoxSphereBounds UOrionGeneratedMeshComponent::CalcBounds(const FTransform & LocalToWorld) const
{
	FBoxSphereBounds NewBounds;
	NewBounds.Origin = MyCenter;// FVector::ZeroVector;
	NewBounds.BoxExtent = FVector(1, 1, 1)* VOXEL_X * VOXEL_WIDTH * MyLOD;// FVector(HALF_WORLD_MAX, HALF_WORLD_MAX, HALF_WORLD_MAX);
	NewBounds.SphereRadius = FMath::Sqrt(3.0f * FMath::Square(VOXEL_X * VOXEL_WIDTH * MyLOD));
	return NewBounds;
}


bool UOrionGeneratedMeshComponent::GetPhysicsTriMeshData(struct FTriMeshCollisionData* CollisionData, bool InUseAllTriData)
{
	FTriIndices Triangle;

	for (int32 i = 0; i<GeneratedMeshTris.Num(); i++) {
		const FGeneratedMeshTriangle& tri = GeneratedMeshTris[i];

		Triangle.v0 = CollisionData->Vertices.Add(tri.Vertex0.Position);
		Triangle.v1 = CollisionData->Vertices.Add(tri.Vertex1.Position);
		Triangle.v2 = CollisionData->Vertices.Add(tri.Vertex2.Position);

		CollisionData->Indices.Add(Triangle);
		//CollisionData->MaterialIndices.Add(i);
	}

	CollisionData->bFlipNormals = true;

	return true;
}

bool UOrionGeneratedMeshComponent::ContainsPhysicsTriMeshData(bool InUseAllTriData) const
{
	return (GeneratedMeshTris.Num() > 0);
}

void UOrionGeneratedMeshComponent::UpdateBodySetup() {
	if (ModelBodySetup == NULL)	{
		ModelBodySetup = NewObject<UBodySetup>(this, UBodySetup::StaticClass());// ConstructObject<UBodySetup>(UBodySetup::StaticClass(), this);
		ModelBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		ModelBodySetup->bMeshCollideAll = true;
	}
}

void UOrionGeneratedMeshComponent::UpdateCollision() {
	if (bPhysicsStateCreated) {
		DestroyPhysicsState();
		UpdateBodySetup();
		CreatePhysicsState();

		ModelBodySetup->InvalidatePhysicsData(); //Will not work in Packaged build
		//Epic needs to add support for this
		ModelBodySetup->CreatePhysicsMeshes();
	}
}

UBodySetup* UOrionGeneratedMeshComponent::GetBodySetup() {
	UpdateBodySetup();
	return ModelBodySetup;
}


