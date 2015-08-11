// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSkeletalMeshComponent.h"

FMatrix UOrionSkeletalMeshComponent::GetRenderMatrix() const
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (playerController)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(playerController->Player);

		if (LocalPlayer != NULL && LocalPlayer->ViewportClient != NULL && LocalPlayer->ViewportClient->Viewport != NULL)
		{
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
				LocalPlayer->ViewportClient->Viewport,
				GetWorld()->Scene,
				LocalPlayer->ViewportClient->EngineShowFlags)
				.SetRealtimeUpdate(true));

			FVector ViewLocation;
			FRotator ViewRotation;
			FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);

			float MinZ = SceneView->NearClippingDistance;
			float MaxZ = MinZ;

			// Avoid zero ViewFOV's which cause divide by zero's in projection matrix
			float MatrixFOV = FMath::Max(0.001f, WeaponFOV) * (float)PI / 360.0f;

			FVector2D size = FVector2D(LocalPlayer->ViewportClient->Viewport->GetSizeXY());

			FMatrix projMatrix = FReversedZPerspectiveMatrix(
				MatrixFOV,
				MatrixFOV,
				1.0f,
				FMath::Clamp(size.X / size.Y, 1.0f, 2.0f),
				MinZ,
				MaxZ
				);

			FScaleMatrix ClipSpaceFixScale(FVector(1.0f, 1.0f, 1.0f - 0.0f));
			FTranslationMatrix ClipSpaceFixTranslate(FVector(0.0f, 0.0f, 0.0f));
			projMatrix = projMatrix * ClipSpaceFixScale * ClipSpaceFixTranslate;

			FMatrix adjustedViewProjectMatrix = SceneView->ViewMatrices.ViewMatrix * projMatrix;
			FMatrix inverseOldViewProjectMatrix = SceneView->InvViewProjectionMatrix;
			FMatrix adjTransform = ComponentToWorld.ToMatrixWithScale() * adjustedViewProjectMatrix * inverseOldViewProjectMatrix;

			return adjTransform;
		}
		else
		{
			return Super::GetRenderMatrix();
		}
	}
	else
	{
		return Super::GetRenderMatrix();
	}
}

