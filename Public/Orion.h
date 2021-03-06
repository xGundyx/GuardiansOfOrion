// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#ifndef __ORION_H__
#define __ORION_H__

//#include "Engine.h"
//#include "EngineKismetLibraryClasses.h"
#include "EngineMinimal.h"
#include "EngineUtils.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "OrionCharacter.h"
#include "OrionPlayerController.h"
#include "OrionTCPLink.h"
#include "UMG.h"
#if WITH_EDITOR
#include "UnrealEd.h"
#endif
//#include "OrionAIController.h"
//#include "OrionDroidController.h"
//#include "OrionDinoController.h"
#include "OrionDamageType.h"
#include "OrionGRI.h"
#include "OrionInventory.h"
#include "OrionInventoryManager.h"
#include "OrionImpactEffect.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel6
#define COLLISION_PROJECTILE	ECC_GameTraceChannel1
#define COLLISION_FLYABLE		ECC_GameTraceChannel7

#endif
