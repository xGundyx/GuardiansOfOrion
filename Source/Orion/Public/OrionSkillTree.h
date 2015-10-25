// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "OrionSkillTree.generated.h"

USTRUCT(BlueprintType)
struct FSkillTreeEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 MaxPoints;		//max points this entry can have
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 Points;			//points put into this skill
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		UTexture2D *Icon;			//texture icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TEnumAsByte<ESkillTreeUnlocks> Category;		//index
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString Title;			//title
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString Desc;			//desc
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString Upgrade;		//upgrade string
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 Upgrade1;			//number to multiply per upgrade
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 Upgrade2;			//number to multiply per secondary upgrade

	FSkillTreeEntry()
	{
		MaxPoints = 0;
		Points = 0;
		Icon = nullptr;
		Category = SKILL_NONE;
		Title = TEXT("");
		Desc = TEXT("");
		Upgrade = TEXT("");
		Upgrade1 = 0;
		Upgrade2 = 0;
	}
};

USTRUCT(BlueprintType)
struct FSkillCategory
{
	GENERATED_USTRUCT_BODY()

	//each row has 3 skills max associated with it
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FSkillTreeEntry Skill1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FSkillTreeEntry Skill2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FSkillTreeEntry Skill3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		int32 MaxPoints;
};

USTRUCT(BlueprintType)
struct FSkillGroup
{
	GENERATED_USTRUCT_BODY()

	//each character has 3 categories
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TArray<FSkillCategory> SkillCategory1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TArray<FSkillCategory> SkillCategory2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TArray<FSkillCategory> SkillCategory3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString SkillTitle1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString SkillTitle2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString SkillTitle3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		FString ClassType;
};

/**
 * 
 */
UCLASS(Blueprintable)
class ORION_API UOrionSkillTree : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Skill)
		TArray<FSkillGroup> Skills;

	UFUNCTION(BlueprintCallable, Category = Skill)
		void SetPointValue(int32 ClassIndex, int32 Category, int32 Index, int32 Value);

	UFUNCTION(BlueprintCallable, Category = Skill)
		int32 GetPointValue(int32 ClassIndex, int32 Category, int32 Index);
};
