// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionStats.h"


UOrionStats::UOrionStats(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//initialize our stats arrray
	aStats.Empty();

	aStats.Add(FPlayerStats(STAT_RAPTORKILL, STATTYPE_INT, TEXT("RAPTOR KILLS")));
	aStats.Add(FPlayerStats(STAT_TREXKILL, STATTYPE_INT, TEXT("T-REX KILLS")));
}


