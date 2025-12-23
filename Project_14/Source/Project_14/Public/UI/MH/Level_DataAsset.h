// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Level_DataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLevelGuideData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MapClearIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText GuideText;
};

UCLASS()
class PROJECT_14_API ULevel_DataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLevelGuideData> GuideDatas;
};
