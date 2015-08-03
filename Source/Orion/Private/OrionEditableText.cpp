// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionEditableText.h"

void UOrionEditableText::SetANewFont(FSlateFontInfo NewFontInfo)
{
	Font_DEPRECATED = NewFontInfo;
	SynchronizeProperties();
}

