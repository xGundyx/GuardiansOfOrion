// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Orion.h"
#include "OrionHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MoviePlayer.h"
#include "OrionGameInstance.h"

AOrionHUD::AOrionHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/Textures/Crosshair"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> LoadingTexObj(TEXT("/Game/FrontEnd/GOOImages/Loading/GOO-LoadingScreen-Gamepad-1080p"));
	LoadingTex = LoadingTexObj.Object;
}

void AOrionHUD::BeginPlay()
{		
	/*FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;// true;
	LoadingScreen.bMoviesAreSkippable = false;
	LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	GetMoviePlayer()->PlayMovie();*/
}

void AOrionHUD::DrawHUD()
{
	Super::DrawHUD();

	UOrionGameInstance *GI = Cast<UOrionGameInstance>(GetWorld()->GetGameInstance());

	if (GI && GI->bLoading)
	{
		FCanvasTileItem LoadItem(FVector2D(0.0f, 0.0f), LoadingTex->Resource, FLinearColor::White);
		LoadItem.BlendMode = SE_BLEND_Opaque;
		LoadItem.Size = FVector2D(Canvas->SizeX, Canvas->SizeY);
		//LoadItem.Z = 1000;
		Canvas->DrawItem(LoadItem);
	}

	AOrionPlayerController *PC = Cast<AOrionPlayerController>(GetOwningPlayerController());

	if (PC && (!PC->bThirdPersonCamera || PC->bToggleHUD || PC->OverviewCamera))
		return;

	//if we're on a ship, remove the crosshair
	AOrionCharacter *Pawn = Cast<AOrionCharacter>(PC->GetPawn());

	if (Pawn && Pawn->IsOnShip())
		return;

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

