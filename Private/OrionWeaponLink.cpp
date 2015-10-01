// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionAbility.h"
#include "OrionWeaponLink.h"

void AOrionWeaponLink::StartAiming()
{
	if (Role == ROLE_Authority)
	{
		AOrionAbility *Skill = MyPawn->CurrentSkill;

		//maybe play an error sound?
		if (!Skill || Skill->GetEnergy() <= 0.0f)
			return;

		LinkTarget = MyPawn;
		StartBeamEffects();
	}
}

void AOrionWeaponLink::StopAiming()
{
	AOrionAbility *Skill = MyPawn->CurrentSkill;

	//maybe play an error sound?
	if (!Skill || Skill->GetEnergy() <= 0.0f)
		return;

	if (Role == ROLE_Authority)
	{
		LinkTarget = nullptr;
		StopBeamEffects();
	}
}

void AOrionWeaponLink::StartFire()
{
	//can't fire when punching stuff
	if (WeaponState == WEAP_MELEE)
		return;

	if (CanFire() == false)
		return;

	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}
	else
	{
		if (MyPawn && MyPawn->CurrentSkill && MyPawn->CurrentSkill->IsCloaking())
			MyPawn->CurrentSkill->DepleteEnergy();
	}

	if (WeaponState == WEAP_RELOADING)
		CancelReload();

	if (WeaponState == WEAP_MELEE)
		CancelMelee();

	//see if there is a valid target for us to link to in our field of vision
	if (Role == ROLE_Authority && MyPawn)
	{
		TArray<AActor*> Pawns;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrionCharacter::StaticClass(), Pawns);
		
		FVector pos = MyPawn->GetActorLocation();
		FVector dir = MyPawn->GetActorRotation().Vector();

		AOrionCharacter *Best = nullptr;
		float BestDist = 10000000.0f;

		AOrionAbility *Skill = MyPawn->CurrentSkill;

		//maybe play an error sound?
		if (!Skill || Skill->GetEnergy() <= 0.0f)
			return;

		for (int32 i = 0; i < Pawns.Num(); i++)
		{
			AOrionCharacter *Current = Cast<AOrionCharacter>(Pawns[i]);

			if (!Current || !Current->IsValidLowLevel())
				continue;

			//ignore enemies for now, possible upgrade
			if ((!Current->PlayerState || Current->PlayerState->bIsABot) && !Current->bIsHealableMachine)
				continue;

			if (!CanLink(Cast<ACharacter>(Pawns[i])))
				continue;

			if (FVector::DotProduct(dir, (Current->GetActorLocation() - pos).GetSafeNormal()) < 0.707)
				continue;

			float dist = (Current->GetActorLocation() - pos).Size();

			if (dist < BestDist && dist < 1250.0f)
			{
				Best = Current;
				BestDist = dist;
			}
		}

		if (Best)
		{
			LinkTarget = Best;

			//start effects
			StartBeamEffects();
		}
	}
}

void AOrionWeaponLink::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (Role == ROLE_Authority)
	{
		LinkTarget = nullptr;
		StopBeamEffects();
	}
}


void AOrionWeaponLink::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrionWeaponLink, LinkTarget);
}

void AOrionWeaponLink::OnRep_LinkTarget()
{
	if (LinkTarget != nullptr)
		StartBeamEffects();
	else
		StopBeamEffects();
}

void AOrionWeaponLink::StartBeamEffects()
{
	if (BeamPSC == nullptr && BeamFX != nullptr && SelfBeamFX != nullptr)
	{
		if (LinkTarget == MyPawn)
			BeamPSC = UGameplayStatics::SpawnEmitterAttached(SelfBeamFX, GetWeaponMesh(MyPawn->IsFirstPerson()), "MuzzleFlashSocket");
		else
			BeamPSC = UGameplayStatics::SpawnEmitterAttached(BeamFX, GetWeaponMesh(MyPawn->IsFirstPerson()), "MuzzleFlashSocket");
	}

	if (BeamPSC && LinkTarget)
	{
		FLinearColor col1;
		FLinearColor col2;
		FLinearColor col3;
		FLinearColor col4;

		AOrionCharacter *P = Cast<AOrionCharacter>(LinkTarget);
		AOrionPRI *PRI1 = Cast<AOrionPRI>(LinkTarget->PlayerState);
		AOrionPRI *PRI2 = Cast<AOrionPRI>(MyPawn->PlayerState);
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

		//green for self
		if (LinkTarget == MyPawn)
		{
			col1 = FLinearColor(1.0f, 15.0f, 1.0f, 15.0f) / 15.0f;
			col2 = FLinearColor(1.0f, 15.0f, 1.0f, 15.0f) / 15.0f;
			col3 = FLinearColor(1.0f, 15.0f, 1.0f, 15.0f) / 15.0f;
			col4 = FLinearColor(1.0f, 15.0f, 1.0f, 15.0f) / 15.0f;
		}
		//teal/blue for teammates
		else if ((P && P->bIsHealableMachine) || (GRI && GRI->OnSameTeam(PRI1, PRI2)))
		{
			col1 = FLinearColor(1.0f, 15.0f, 15.0f, 15.0f) / 15.0f;
			col2 = FLinearColor(1.0f, 10.0f, 15.0f, 15.0f) / 15.0f;
			col3 = FLinearColor(1.0f, 15.0f, 10.0f, 15.0f) / 15.0f;
			col4 = FLinearColor(1.0f, 15.0f, 5.0f, 15.0f) / 15.0f;
		}
		//red for enemies
		else if (GRI && !GRI->OnSameTeam(PRI1, PRI2))
		{
			col1 = FLinearColor(15.0f, 1.0f, 1.0f, 15.0f) / 15.0f;
			col2 = FLinearColor(15.0f, 1.0f, 1.0f, 15.0f) / 15.0f;
			col3 = FLinearColor(15.0f, 1.0f, 1.0f, 15.0f) / 15.0f;
			col4 = FLinearColor(15.0f, 1.0f, 1.0f, 15.0f) / 15.0f;
		}

		FVector pos = LinkTarget->GetMesh()->GetSocketLocation("AutoAim");
		BeamPSC->SetVectorParameter(TEXT("LinkBeamEnd"), pos);
		//BeamPSC->SetBeamEndPoint(0, LinkTarget->GetActorLocation());
		BeamPSC->ActivateSystem();

		BeamPSC->SetColorParameter(TEXT("BeamColor1"), col1);
		BeamPSC->SetColorParameter(TEXT("BeamColor2"), col2);
		BeamPSC->SetColorParameter(TEXT("BeamColor3"), col3);
		BeamPSC->SetColorParameter(TEXT("BeamColor4"), col4);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), StartSound, MyPawn->GetActorLocation());
		LoopAudio = UGameplayStatics::SpawnSoundAttached(LoopSound, MyPawn->GetMesh());
	}
}

void AOrionWeaponLink::StopBeamEffects()
{
	if (BeamPSC)
	{
		if (EndSound)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), EndSound, MyPawn->GetActorLocation());

		BeamPSC->DeactivateSystem();
		BeamPSC = nullptr;
	}

	if (LoopAudio)
	{
		LoopAudio->Stop();
		LoopAudio = nullptr;
	}
}

void AOrionWeaponLink::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		ValidateLinkTarget(DeltaSeconds);
		HandleTarget(DeltaSeconds);

		if (LinkTarget && LinkTarget == MyPawn)
		{
			AOrionCharacter *P = Cast<AOrionCharacter>(LinkTarget);
			if (P)
			{
				P->LastHealTime = GetWorld()->GetTimeSeconds();
				P->HealTarget = 5.0f;
			}
		}
	}

	//update our beam if needed
	if (BeamPSC && BeamPSC->bIsActive && LinkTarget)
	{
		FVector pos = LinkTarget->GetMesh()->GetSocketLocation("AutoAim");
		BeamPSC->SetVectorParameter(TEXT("LinkBeamEnd"), pos);
	}
}

float AOrionWeaponLink::OnUnEquip()
{
	float ret = Super::OnUnEquip();

	StopBeamEffects();

	return ret;
}

bool AOrionWeaponLink::CanLink(ACharacter *Target)
{
	AOrionCharacter *P = Cast<AOrionCharacter>(Target);

	if (!Target)
		return false;
	if (Target == MyPawn)
		return true;
	else if (P && P->bIsHealableMachine)
		return true;
	else if (!Target || !Target->IsValidLowLevel())
		return false;
	else if (P && (P->Health <= 0 || P->bFatality))
		return false;

	return true;
}

void AOrionWeaponLink::ValidateLinkTarget(float DeltaSeconds)
{
	bool bExit = false;

	if (!LinkTarget || !MyPawn)
		return;

	//update energy and turn off if out of energy
	AOrionAbility *Skill = MyPawn->CurrentSkill;

	if (Skill)
	{
		Skill->SetEnergy(FMath::Max(0.0f, Skill->GetEnergy() - DeltaSeconds * 10.0f));
		Skill->TimeSinceLastActive = GetWorld()->TimeSeconds - (Skill->RechargeDelay - 5.0f);

		if (Skill->GetEnergy() <= 0.0f)
			bExit = true;
	}

	if (!MyPawn || !MyPawn->IsValidLowLevel())
		bExit = true;
	//if link target is dead, being fatalitied, etc, reset
	else if (!CanLink(LinkTarget))
		bExit = true;
	else if ((LinkTarget->GetActorLocation() - MyPawn->GetActorLocation()).Size() > 1350.0f)
		bExit = true;

	if (bExit)
	{
		LinkTarget = nullptr;
		StopBeamEffects();
	}
}

void AOrionWeaponLink::HandleTarget(float DeltaSeconds)
{
	if (!LinkTarget || !MyPawn)
		return;

	AOrionCharacter *P = Cast<AOrionCharacter>(LinkTarget);
	AOrionPRI *PRI1 = Cast<AOrionPRI>(LinkTarget->PlayerState);
	AOrionPRI *PRI2 = Cast<AOrionPRI>(MyPawn->PlayerState);
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	//if the target is the generator, heal it up
	if (P && P->bIsHealableMachine)
	{
		if (P && P->Health < P->HealthMax)
			P->Health += DeltaSeconds * P->HealthMax / 20.0f;
	}
	//if the target is a teammate, heal them up
	else if (GRI && GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P && P->Health < P->HealthMax)
			P->Health += DeltaSeconds * P->HealthMax / 10.0f;
		else if (P && P->Shield < P->ShieldMax)
			P->Shield += DeltaSeconds * P->ShieldMax / 10.0f;
	}
	//if the target is an enemy, hurt/slow them
	else if (GRI && !GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P)
		{
			P->Health -= DeltaSeconds * 150.0f;
			if (P->Health <= 0)
			{
				FPointDamageEvent PointDmg;

				PointDmg.DamageTypeClass = UOrionDamageType::StaticClass();
				PointDmg.Damage = 150.0f;

				P->Die(150.0f, PointDmg, MyPawn->Controller, MyPawn);
			}
		}
	}
}