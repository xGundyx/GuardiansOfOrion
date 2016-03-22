// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionSkillTree.h"

void UOrionSkillTree::SetPointValue(int32 ClassIndex, int32 Category, int32 Index, int32 Value)
{
	switch (Category)
	{
	case 0:
		switch (Index % 3)
		{
		case 0:
			Skills[ClassIndex].SkillCategory1[Index / 3].Skill1.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory1[Index / 3].Skill1.MaxPoints);
			break;
		case 1:
			Skills[ClassIndex].SkillCategory1[Index / 3].Skill2.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory1[Index / 3].Skill2.MaxPoints);
			break;
		case 2:
			Skills[ClassIndex].SkillCategory1[Index / 3].Skill3.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory1[Index / 3].Skill3.MaxPoints);
			break;
		}
		break;
	case 1:
		switch (Index % 3)
		{
		case 0:
			Skills[ClassIndex].SkillCategory2[Index / 3].Skill1.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory2[Index / 3].Skill1.MaxPoints);
			break;
		case 1:
			Skills[ClassIndex].SkillCategory2[Index / 3].Skill2.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory2[Index / 3].Skill2.MaxPoints);
			break;
		case 2:
			Skills[ClassIndex].SkillCategory2[Index / 3].Skill3.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory2[Index / 3].Skill3.MaxPoints);
			break;
		}
		break;
	case 2:
		switch (Index % 3)
		{
		case 0:
			Skills[ClassIndex].SkillCategory3[Index / 3].Skill1.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory3[Index / 3].Skill1.MaxPoints);
			break;
		case 1:
			Skills[ClassIndex].SkillCategory3[Index / 3].Skill2.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory3[Index / 3].Skill2.MaxPoints);
			break;
		case 2:
			Skills[ClassIndex].SkillCategory3[Index / 3].Skill3.Points = FMath::Min(Value, Skills[ClassIndex].SkillCategory3[Index / 3].Skill3.MaxPoints);
			break;
		}
		break;
	}
}

int32 UOrionSkillTree::GetPointValue(int32 ClassIndex, int32 Category, int32 Index)
{
	FSkillCategory Skill;

	switch (Category)
	{
	case 0:
		Skill = Skills[ClassIndex].SkillCategory1[Index / 3];
		break;
	case 1:
		Skill = Skills[ClassIndex].SkillCategory2[Index / 3];
		break;
	case 2:
		Skill = Skills[ClassIndex].SkillCategory3[Index / 3];
		break;
	}

	FSkillTreeEntry Entry;

	switch (Index % 3)
	{
	case 0:
		Entry = Skill.Skill1;
		break;
	case 1:
		Entry = Skill.Skill2;
		break;
	case 2:
		Entry = Skill.Skill3;
		break;
	}

	return FMath::Min(Entry.MaxPoints, Entry.Points);
}
