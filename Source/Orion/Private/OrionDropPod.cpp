// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionDropPod.h"

AOrionDropPod::AOrionDropPod(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DropPodMesh = ObjectInitializer.CreateOptionalDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Pod"));
	DropPodMesh->AlwaysLoadOnClient = true;
	DropPodMesh->AlwaysLoadOnServer = true;
	DropPodMesh->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
	DropPodMesh->bCastDynamicShadow = true;
	DropPodMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	//DropPodMesh->bChartDistanceFactor = true;
	DropPodMesh->bGenerateOverlapEvents = false;
	DropPodMesh->Mobility = EComponentMobility::Type::Movable;

	RootComponent = DropPodMesh;

	SpawnCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("SpawnCamera"));
	SpawnCameraComponent->AttachParent = DropPodMesh;

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.bCanEverTick = true;

	bHasLanded = false;
}


