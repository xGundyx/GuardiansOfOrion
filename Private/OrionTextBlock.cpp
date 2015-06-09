// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionTextBlock.h"

void UOrionTextBlock::SetANewFont(FSlateFontInfo NewFontInfo)
{
	Font = NewFontInfo;
	SynchronizeProperties();
}