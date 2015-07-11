// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionTextBlock.h"

void UOrionTextBlock::SetANewFont(FSlateFontInfo NewFontInfo)
{
	Font = NewFontInfo;
	SynchronizeProperties();
}

void UOrionTextBlock::SetANewHeightPercentage(float height)
{
	LineHeightPercentage = height;
	SynchronizeProperties();
}

void UOrionTextBlock::SetANewWrapWidth(float width)
{
	WrapTextAt = width;
	SynchronizeProperties();
}