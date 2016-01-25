// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionCharacter.h"
#include "OrionAbility.h"
#include "OrionWeaponLink.h"

void AOrionWeaponLink::StartAiming(bool bPlaySound)
{
	if (Role == ROLE_Authority)
	{
		if (LinkTarget != nullptr)
			return;

		AOrionAbility *Skill = MyPawn->CurrentSkill;

		//maybe play an error sound?
		if (!Skill)// || Skill->GetEnergy() <= 0.0f)
			return;

		LinkTarget = MyPawn;
		StartBeamEffects();
	}
}

void AOrionWeaponLink::StopAiming()
{
	AOrionAbility *Skill = MyPawn->CurrentSkill;

	//maybe play an error sound?
	if (!Skill)// || Skill->GetEnergy() <= 0.0f)
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
	else if (MyPawn)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);

		if (PC)
		{
			AOrionInventoryManager *Inv = PC->GetInventoryManager();
			if (Inv && Inv->HasStat(RARESTAT_CLOAKREGEN))
			{
			}
			else
				if (MyPawn && MyPawn->CurrentSkill && MyPawn->CurrentSkill->IsCloaking())
					MyPawn->CurrentSkill->DeactivateSkill(); //DepleteEnergy();
		}
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
		if (!Skill)// || Skill->GetEnergy() <= 0.0f)
			return;

		float MaxDist = 1000.0f;

		if (MyPawn)
		{
			AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
			if (PC)
				MaxDist *= 1.0f + float(PC->GetSkillValue(SKILL_BEAMLENGTH)) / 100.0f;
		}

		bool bLatch = false;

		if (MyPawn)
		{
			AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
			if (PC)
				bLatch = PC->GetSkillValue(SKILL_BEAMLENGTH) == PC->CharacterSkills[SKILL_BEAMLENGTH].MaxPoints * PC->CharacterSkills[SKILL_BEAMLENGTH].Modifier;
		}

		for (int32 i = 0; i < Pawns.Num(); i++)
		{
			AOrionCharacter *Current = Cast<AOrionCharacter>(Pawns[i]);

			if (!Current || !Current->IsValidLowLevel())
				continue;

			//ignore enemies for now, possible upgrade
			if ((!Current->PlayerState || (!bLatch && Current->PlayerState->bIsABot)) && !Current->bIsHealableMachine)
				continue;

			if (!CanLink(Cast<ACharacter>(Pawns[i])))
				continue;

			if (FVector::DotProduct(dir, (Current->GetActorLocation() - pos).GetSafeNormal()) < 0.707)
				continue;

			float dist = (Current->GetActorLocation() - pos).Size();

			if (dist < BestDist && dist < MaxDist)
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
	DOREPLIFETIME(AOrionWeaponLink, ChainLinkTargets);
}

void AOrionWeaponLink::OnRep_ChainLinks()
{
	//cycle through each chainlink, if it already has a matching target, ignore it, if has no target, add it, if target exists, but no chain, remove it
	for (auto iter(ChainLinkTargets.CreateIterator()); iter; iter++)
	{
		AOrionCharacter *Link = *iter;

		FChainLinkTarget Target;
		Target.LinkTarget = Link;
		Target.BeamPSC = nullptr;

		if (ChainBeamPSC.Find(Target) == INDEX_NONE && LinkTarget)
		{
			Target.BeamPSC = UGameplayStatics::SpawnEmitterAttached(BeamFX, LinkTarget->GetMesh(), "AutoAim");

			ActivateChainBeam(Target);

			ChainBeamPSC.Add(Target);
		}
	}
	
	for (auto iter(ChainBeamPSC.CreateIterator()); iter; iter++)
	{
		FChainLinkTarget Target = *iter;

		if (ChainLinkTargets.Find(Target.LinkTarget) == INDEX_NONE)
		{
			if (Target.BeamPSC)
			{
				Target.BeamPSC->DeactivateSystem();
				Target.BeamPSC = nullptr;
			}

			ChainBeamPSC.Remove(Target);
		}
	}
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
	if (GetNetMode() == NM_DedicatedServer)
		return;

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

		if (LoopAudio)
		{
			LoopAudio->Stop();
			LoopAudio = nullptr;
		}

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

		if (LinkTarget)// && LinkTarget == MyPawn)
		{
			AOrionCharacter *P = Cast<AOrionCharacter>(LinkTarget);
			if (P)
			{
				P->LastHealTime = GetWorld()->GetTimeSeconds();
				P->HealTarget = 5.0f;
			}
		}

		if (LinkTarget && LinkTarget != MyPawn)
		{
			HandleChainTargets(DeltaSeconds);

			for (int32 i = 0; i < ChainLinkTargets.Num(); i++)
			{
				HandleLinkTargets(ChainLinkTargets[i], DeltaSeconds);
			}
		}
	}

	if (LinkTarget && MyPawn && MyPawn->GetWeapon() != this)
	{
		OnUnEquip();
	}

	if (LinkTarget == nullptr && ChainBeamPSC.Num() > 0)
		StopAllChainBeams();

	if (!MyPawn || MyPawn->Health <= 0.0f)
		StopAllChainBeams();

	//update our beam if needed
	if (BeamPSC && BeamPSC->bIsActive && LinkTarget)
	{
		FVector pos = LinkTarget->GetMesh()->GetSocketLocation("AutoAim");
		BeamPSC->SetVectorParameter(TEXT("LinkBeamEnd"), pos);
	}

	UpdateChainBeamEffects();
}

void AOrionWeaponLink::StopAllChainBeams()
{
	if (LoopAudio)
	{
		LoopAudio->Stop();
		LoopAudio = nullptr;
	}

	//if (Role == ROLE_Authority)
	ChainLinkTargets.Empty();

	for (auto iter(ChainBeamPSC.CreateIterator()); iter; iter++)
	{
		FChainLinkTarget Target = *iter;

		if (Target.BeamPSC)
		{
			Target.BeamPSC->DeactivateSystem();
			Target.BeamPSC = nullptr;
		}

		//ChainBeamPSC.Remove(Target);
	}

	ChainBeamPSC.Empty();
}

void AOrionWeaponLink::HandleChainTargets(float DeltaSeconds)
{
	if (GetWorld()->GetTimeSeconds() - LastChainTime < 1.0f)
		return;

	LastChainTime = GetWorld()->GetTimeSeconds();

	if (!MyPawn)
		return;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
	
	if (PC)
	{
		bool bLatch = PC->GetSkillValue(SKILL_BEAMLENGTH) == PC->CharacterSkills[SKILL_BEAMLENGTH].MaxPoints * PC->CharacterSkills[SKILL_BEAMLENGTH].Modifier;

		if (PC->GetSkillValue(SKILL_REGENCHAIN) > 0)
		{
			//validate any chain targets we may already have
			for (auto iter(ChainLinkTargets.CreateIterator()); iter; iter++)
			{
				bool bRemove = false;

				AOrionCharacter *P = *iter;

				//make sure it's still close enough to the linktarget
				if (!P || !P->IsValidLowLevel() || (LinkTarget->GetActorLocation() - P->GetActorLocation()).Size() > 500.0f)
					bRemove = true;
				//make sure the linktarget is still alive
				else if (P->Health <= 0.0f && !P->bDowned)
					bRemove = true;

				if (bRemove)
				{
					if (GetNetMode() != NM_DedicatedServer)
					{
						FChainLinkTarget Temp;
						Temp.LinkTarget = P;
						int32 index = ChainBeamPSC.Find(Temp);

						if (index >= 0)
						{
							FChainLinkTarget Target;

							Target.LinkTarget = P;

							if (ChainBeamPSC[index].BeamPSC)
							{
								ChainBeamPSC[index].BeamPSC->DeactivateSystem();
								ChainBeamPSC[index].BeamPSC = nullptr;
							}

							ChainBeamPSC.Remove(Target);
						}
					}

					ChainLinkTargets.Remove(P);
				}
			}

			//if we are at our max chain targets, just exit
			if (ChainLinkTargets.Num() >= 5)
				return;

			//search for all pawns in a radius around the linktarget
			for (TActorIterator<AOrionCharacter> It(GetWorld(), AOrionCharacter::StaticClass()); It; ++It)
			{
				AOrionCharacter* Pawn = *It;

				if (Pawn == MyPawn || Pawn == LinkTarget)
					continue;

				if ((Pawn->GetActorLocation() - LinkTarget->GetActorLocation()).Size() > 400.0f)
					continue;

				if (Pawn->Health <= 0.0f && !Pawn->bDowned)
					continue;

				if (!CanLink(Pawn))
					continue;

				if (Pawn->PlayerState == nullptr || (!bLatch && Pawn->PlayerState->bIsABot))
					continue;

				if (ChainLinkTargets.Find(Pawn) == INDEX_NONE)
				{
					ChainLinkTargets.Add(Pawn);

					if (GetNetMode() != NM_DedicatedServer)
					{
						FChainLinkTarget Target;

						Target.LinkTarget = Pawn;
						Target.BeamPSC = UGameplayStatics::SpawnEmitterAttached(BeamFX, LinkTarget->GetMesh(), "AutoAim");

						ActivateChainBeam(Target);

						ChainBeamPSC.AddUnique(Target);
					}

					if (ChainLinkTargets.Num() >= 5)
						return;
				}
			}
		}
	}
}

void AOrionWeaponLink::UpdateChainBeamEffects()
{
	for (int32 i = 0; i < ChainBeamPSC.Num(); i++)
	{
		if (ChainBeamPSC[i].BeamPSC && ChainBeamPSC[i].BeamPSC->bIsActive && ChainBeamPSC[i].LinkTarget)
		{
			FVector pos = ChainBeamPSC[i].LinkTarget->GetMesh()->GetSocketLocation("AutoAim");
			ChainBeamPSC[i].BeamPSC->SetVectorParameter(TEXT("LinkBeamEnd"), pos);
		}
	}
}

void AOrionWeaponLink::ActivateChainBeam(FChainLinkTarget Target)
{
	if (Target.BeamPSC && Target.LinkTarget)
	{
		FLinearColor col1;
		FLinearColor col2;
		FLinearColor col3;
		FLinearColor col4;

		AOrionCharacter *P = Cast<AOrionCharacter>(Target.LinkTarget);
		AOrionPRI *PRI1 = Cast<AOrionPRI>(Target.LinkTarget->PlayerState);
		AOrionPRI *PRI2 = Cast<AOrionPRI>(MyPawn->PlayerState);
		AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

		//teal/blue for teammates
		if ((P && P->bIsHealableMachine) || (GRI && GRI->OnSameTeam(PRI1, PRI2)))
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

		FVector pos = Target.LinkTarget->GetMesh()->GetSocketLocation("AutoAim");
		Target.BeamPSC->SetVectorParameter(TEXT("LinkBeamEnd"), pos);
		//BeamPSC->SetBeamEndPoint(0, LinkTarget->GetActorLocation());
		Target.BeamPSC->ActivateSystem();

		Target.BeamPSC->SetColorParameter(TEXT("BeamColor1"), col1);
		Target.BeamPSC->SetColorParameter(TEXT("BeamColor2"), col2);
		Target.BeamPSC->SetColorParameter(TEXT("BeamColor3"), col3);
		Target.BeamPSC->SetColorParameter(TEXT("BeamColor4"), col4);
	}
}

void AOrionWeaponLink::HandleLinkTargets(AOrionCharacter *Target, float DeltaSeconds)
{
	if (!Target || !LinkTarget || !MyPawn)
		return;

	AOrionCharacter *P = Cast<AOrionCharacter>(Target);
	AOrionPRI *PRI1 = Cast<AOrionPRI>(Target->PlayerState);
	AOrionPRI *PRI2 = Cast<AOrionPRI>(MyPawn->PlayerState);
	AOrionGRI *GRI = Cast<AOrionGRI>(GetWorld()->GameState);

	float Rate = 1.0f;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
	if (PC)
		Rate = 1.0f + float(PC->GetSkillValue(SKILL_FASTERHEALING)) / 100.0f;

	float nPlayers = 1;

	if (GRI)
		nPlayers = GRI->PlayerList.Num();

	AOrionInventoryManager *Inv = nullptr;

	if (PC)
	{
		Inv = PC->GetInventoryManager();
		if (Inv && Inv->HasStat(RARESTAT_SUPERHEALER))
			Rate *= 5.0f;
	}

	//if the target is the generator, heal it up
	if (P && P->bIsHealableMachine)
	{
		if (P && P->Health < P->HealthMax)
			P->Health = FMath::Min(P->HealthMax, P->Health + DeltaSeconds * Rate * P->HealthMax / 40.0f * (nPlayers == 1 ? 2.0f : 1.0f));
	}
	//if the target is a teammate, heal them up
	else if (GRI && GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P && P->Health < P->HealthMax)
		{
			P->Health = FMath::Min(P->HealthMax, P->Health + DeltaSeconds * Rate * P->HealthMax / 20.0f);

			if (P->bDowned && P->Health >= P->HealthMax)
				P->Revived();
		}
		else if (P && P->Shield < P->ShieldMax)
			P->Shield = FMath::Min(P->ShieldMax, P->Shield + DeltaSeconds * Rate * P->ShieldMax / 20.0f);
	}
	//if the target is an enemy, hurt/slow them
	else if (GRI && !GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P && P->Health > 0)
		{
			P->Health -= DeltaSeconds * 75.0f;
			if (P->Health <= 0)
			{
				FPointDamageEvent PointDmg;

				PointDmg.DamageTypeClass = UOrionDamageType::StaticClass();
				PointDmg.Damage = 75.0f;
				PointDmg.ShotDirection = FVector(0.0f);

				P->Die(150.0f, PointDmg, MyPawn->Controller, MyPawn);

				ApplyDamageDebuff(P);
			}
		}
	}
}

float AOrionWeaponLink::OnUnEquip()
{
	float ret = Super::OnUnEquip();

	StopAllChainBeams();
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
	else if (P && !P->bDowned && (P->Health <= 0 || P->bFatality))
		return false;

	return true;
}

void AOrionWeaponLink::ValidateLinkTarget(float DeltaSeconds)
{
	bool bExit = false;

	if (LinkTarget && !LinkTarget->IsValidLowLevel())
		LinkTarget = nullptr;

	if (!LinkTarget || !MyPawn)
		return;

	//update energy and turn off if out of energy
	AOrionAbility *Skill = MyPawn->CurrentSkill;

	if (Skill)
	{
		/*float Rate = 4.0f;

		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		if (PC)
			Rate -= 4.0f * float(PC->GetSkillValue(SKILL_REGENENERGY)) / 100.0f;

		Skill->SetEnergy(FMath::Max(0.0f, Skill->GetEnergy() - DeltaSeconds * Rate));
		Skill->TimeSinceLastActive = GetWorld()->TimeSeconds - (Skill->RechargeDelay - 5.0f);

		if (Skill->GetEnergy() <= 0.0f)
			bExit = true;*/
	}

	float MaxDist = 1100.0f;

	if (MyPawn)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
		if (PC)
			MaxDist *= 1.0f + float(PC->GetSkillValue(SKILL_BEAMLENGTH)) / 100.0f;
	}

	if (!MyPawn || !MyPawn->IsValidLowLevel())
		bExit = true;
	//if link target is dead, being fatalitied, etc, reset
	else if (!CanLink(LinkTarget))
		bExit = true;
	else if ((LinkTarget->GetActorLocation() - MyPawn->GetActorLocation()).Size() > MaxDist)
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

	float Rate = 1.0f;

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);
	if (PC)
		Rate = 1.0f + float(PC->GetSkillValue(SKILL_FASTERHEALING)) / 100.0f;

	float nPlayers = 1;

	if (GRI)
		nPlayers = GRI->PlayerList.Num();

	AOrionInventoryManager *Inv = nullptr;

	if (PC)
	{
		Inv = PC->GetInventoryManager();
		if (Inv && Inv->HasStat(RARESTAT_SUPERHEALER))
			Rate *= 5.0f;
	}

	//if the target is the generator, heal it up
	if (P && P->bIsHealableMachine)
	{
		if (PC)
			Rate += float(PC->GetSkillValue(SKILL_REGENENERGY)) / 100.0f;

		if (P && P->Health < P->HealthMax)
			P->Health = FMath::Min(P->HealthMax, P->Health + DeltaSeconds * Rate * P->HealthMax / 20.0f * (nPlayers == 1 ? 2.0f : 1.0f));
	}
	//if the target is a teammate, heal them up
	else if (GRI && GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P && P->Health < P->HealthMax)
		{
			P->Health = FMath::Min(P->HealthMax, P->Health + DeltaSeconds * Rate * P->HealthMax / 10.0f);

			if (P->Health >= P->HealthMax)
				P->Revived();
		}
		else if (P && P->Shield < P->ShieldMax)
			P->Shield = FMath::Min(P->ShieldMax, P->Shield + DeltaSeconds * Rate * P->ShieldMax / 10.0f);
	}
	//if the target is an enemy, hurt/slow them
	else if (GRI && !GRI->OnSameTeam(PRI1, PRI2))
	{
		if (P && P->Health > 0)
		{
			P->Health -= DeltaSeconds * 150.0f;
			if (P->Health <= 0)
			{
				FPointDamageEvent PointDmg;

				PointDmg.DamageTypeClass = UOrionDamageType::StaticClass();
				PointDmg.Damage = 150.0f;
				PointDmg.ShotDirection = FVector(0.0f);

				P->Die(150.0f, PointDmg, MyPawn->Controller, MyPawn);

				ApplyDamageDebuff(P);
			}
		}
	}
}

void AOrionWeaponLink::ApplyDamageDebuff(AOrionCharacter *DamagedPawn)
{
	if (MyPawn)
	{
		AOrionPlayerController *PC = Cast<AOrionPlayerController>(MyPawn->Controller);

		if (PC)
		{
			AOrionInventoryManager *Inv = PC->GetInventoryManager();
			AOrionPRI *PRI = Cast<AOrionPRI>(PC->PlayerState);

			if (Inv && PRI)
			{
				if (FireBuff && Inv->HasStat(RARESTAT_REGENFIRE)) DamagedPawn->AddBuff(FireBuff, PC, PRI->GetTeamIndex());
				if (IceBuff && Inv->HasStat(RARESTAT_REGENFREEZE)) DamagedPawn->AddBuff(IceBuff, PC, PRI->GetTeamIndex());
			}
		}
	}
}

void AOrionWeaponLink::Destroyed()
{
	StopAllChainBeams();

	StopBeamEffects();

	Super::Destroyed();
}