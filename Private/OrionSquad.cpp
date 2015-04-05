// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSquad.h"

UOrionSquad::UOrionSquad()
{
	Members.Empty();
	Leader = nullptr;
}

void UOrionSquad::AddSquadMember(AOrionAIController *NewMember)
{
	if (NewMember == nullptr)
		return;

	if (Leader == nullptr)
		Leader = NewMember;

	NewMember->Squad = this;

	Members.Add(NewMember);
	
	//give this member some starting instructions (if any)
	InitMember(NewMember);
}

void UOrionSquad::RemoveSquadMember(AOrionAIController *MemberToRemove)
{
	if (MemberToRemove == nullptr)
		return;

	Members.Remove(MemberToRemove);

	if (Leader == MemberToRemove)
		FindNewLeader();

	//check for bad controllers?
}

void UOrionSquad::FindNewLeader()
{
	if (Members.Num() > 0)
		Leader = Members[0];
	else
		Leader = nullptr;
}

void UOrionSquad::SetEnemy(APawn *pEnemy)
{
	for (int32 i = 0; i < Members.Num(); i++)
	{
		if (Members[i] == Leader)
			continue;

		Members[i]->SetEnemy(pEnemy);
	}
}

void UOrionSquad::InitMember(AOrionAIController *NewMember)
{
	if (Leader && Leader != NewMember)
	{
		//tell them if we have an enemy to fight 
		if (Leader->GetEnemy())
			NewMember->SetEnemy(Leader->GetEnemy());
	}
}

FVector UOrionSquad::GetLeaderDestination()
{
	return SquadDestination;
}

void UOrionSquad::SetSquadDestination(FVector NewDestination)
{
	SquadDestination = NewDestination;
}

