// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSkeletalMeshComponent.h"
#include "OrionLobbyPawn.h"

AOrionLobbyPawn::AOrionLobbyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->bOwnerNoSee = true;
	HelmetMesh->bOwnerNoSee = true;
	BodyMesh->bOwnerNoSee = true;
	ArmsMesh->bOwnerNoSee = true;
	LegsMesh->bOwnerNoSee = true;
	Flight1Mesh->bOwnerNoSee = true;
	Flight2Mesh->bOwnerNoSee = true;

	//Arms1PArmorMesh->SetHiddenInGame(true);
	//Arms1PMesh->SetHiddenInGame(true);
}

bool AOrionLobbyPawn::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

void AOrionLobbyPawn::Use()
{
	if (ShouldIgnoreControls())
		return;

	//stuff that only happens client side
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);
	if (PC && IsLocallyControlled())
	{
		PC->EventCheckUse();
	}

	/*if (Role < ROLE_Authority)
	{
		ServerUse();
		return;
	}*/
}

void AOrionLobbyPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAxis("Turn", this, &AOrionCharacter::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AOrionCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AOrionCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AOrionCharacter::LookUpAtRate);

	InputComponent->BindAxis("MoveForward", this, &AOrionCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AOrionCharacter::MoveRight);

	InputComponent->BindAction("Reload", IE_Pressed, this, &AOrionCharacter::Reload);
	////InputComponent->BindAction("Reload", IE_Released, this, &AOrionTopDownPawn::ReleaseReload);

	////InputComponent->BindAction("Melee", IE_Pressed, this, &AOrionCharacter::DoMelee);
	////InputComponent->BindAction("Melee", IE_Released, this, &AOrionCharacter::EndMelee);

	////InputComponent->BindAction("Roll", IE_Pressed, this, &AOrionTopDownPawn::PressRoll);
	////InputComponent->BindAction("Blink", IE_Pressed, this, &AOrionTopDownPawn::CheckForBlink);

	InputComponent->BindAction("Run", IE_Pressed, this, &AOrionCharacter::Sprint);
	InputComponent->BindAction("Run", IE_Released, this, &AOrionCharacter::StopSprint);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AOrionCharacter::OnFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AOrionCharacter::OnStopFire);
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOrionCharacter::TouchStarted);

	InputComponent->BindAction("Aim", IE_Pressed, this, &AOrionCharacter::StartAiming);
	InputComponent->BindAction("Aim", IE_Released, this, &AOrionCharacter::StopAiming);

	InputComponent->BindAction("Use", IE_Pressed, this, &AOrionLobbyPawn::Use);

	////InputComponent->BindAction("BehindView", IE_Pressed, this, &AOrionCharacter::BehindView);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AOrionCharacter::OnPrevWeapon);
	InputComponent->BindAction("LastWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);

	////InputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &AOrionTopDownPawn::OnPrimaryWeapon);
	////InputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &AOrionTopDownPawn::OnSecondaryWeapon);
	////InputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &AOrionTopDownPawn::OnGadget);

	////InputComponent->BindAction("Gamepad_WeaponSlot3", IE_Pressed, this, &AOrionTopDownPawn::OnGadget);

	////InputComponent->BindAction("ActivateSkill", IE_Pressed, this, &AOrionCharacter::TryToActivateSkill);
	////InputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &AOrionCharacter::TossGrenade);

	//chat is only for the controller impaired
	InputComponent->BindAction("Say", IE_Pressed, this, &AOrionLobbyPawn::Say);
	InputComponent->BindAction("TeamSay", IE_Pressed, this, &AOrionLobbyPawn::TeamSay);

	//controller inputs
	InputComponent->BindAxis("Gamepad_MoveForward", this, &AOrionCharacter::MoveForward);
	InputComponent->BindAxis("Gamepad_MoveRight", this, &AOrionCharacter::MoveRight);

	////InputComponent->BindAction("Gamepad_Reload", IE_Pressed, this, &AOrionCharacter::Reload);
	////InputComponent->BindAction("Gamepad_Reload", IE_Released, this, &AOrionTopDownPawn::ReleaseReload);

	////InputComponent->BindAction("Gamepad_Melee", IE_Pressed, this, &AOrionCharacter::DoMelee);
	////InputComponent->BindAction("Gamepad_Melee", IE_Released, this, &AOrionCharacter::EndMelee);

	////InputComponent->BindAction("Gamepad_Roll", IE_Pressed, this, &AOrionTopDownPawn::PressRoll);
	////InputComponent->BindAction("Gamepad_Blink", IE_Pressed, this, &AOrionTopDownPawn::CheckForBlink);

	InputComponent->BindAction("Gamepad_Run", IE_Pressed, this, &AOrionCharacter::GamepadSprint);

	////InputComponent->BindAction("Gamepad_Roll", IE_Pressed, this, &AOrionLobbyPawn::Use);
	////InputComponent->BindAction("Gamepad_Fire", IE_Released, this, &AOrionCharacter::OnStopFire);
	////InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AOrionCharacter::TouchStarted);

	////InputComponent->BindAction("Gamepad_Aim", IE_Pressed, this, &AOrionCharacter::StartAiming);
	////InputComponent->BindAction("Gamepad_Aim", IE_Released, this, &AOrionCharacter::StopAiming);

	////InputComponent->BindAction("Gamepad_Use", IE_Pressed, this, &AOrionCharacter::Use);

	////InputComponent->BindAction("Gamepad_LastWeapon", IE_Pressed, this, &AOrionCharacter::OnNextWeapon);
	//InputComponent->BindAction("Gamepad_PrevWeapon", IE_Pressed, this, &AOrionCharacter::OnPrevWeapon);

	////InputComponent->BindAction("Gamepad_ActivateSkill", IE_Pressed, this, &AOrionCharacter::TryToActivateSkill);
	////InputComponent->BindAction("Gamepad_ThrowGrenade", IE_Pressed, this, &AOrionCharacter::TossGrenade);
}

void AOrionLobbyPawn::Say()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && !PC->bMenuOpen)
		PC->EventSay();
}

void AOrionLobbyPawn::TeamSay()
{
	AOrionPlayerController *PC = Cast<AOrionPlayerController>(Controller);

	if (PC && !PC->bMenuOpen)
		PC->EventTeamSay();
}
