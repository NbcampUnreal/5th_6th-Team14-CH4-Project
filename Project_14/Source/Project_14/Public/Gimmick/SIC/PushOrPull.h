// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/MovingPlatform.h"
#include "PushOrPull.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API APushOrPull : public AMovingPlatform
{
	GENERATED_BODY()

	void MoveActor(float DeltaTime);
	void RotateActor(float DeltaTime);
};
