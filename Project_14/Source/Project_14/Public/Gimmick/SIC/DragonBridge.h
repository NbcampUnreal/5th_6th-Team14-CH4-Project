// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonBridge.generated.h"

UCLASS()
class PROJECT_14_API ADragonBridge : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	ADragonBridge();
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere)
	FVector StartLocation;

	UPROPERTY(EditAnywhere)
	FVector TargetLocation;

	UPROPERTY()
	TArray<FVector> Targets;

	int32 TargetNum = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxTargetNum = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 200.0f;
	bool bIsEnd;

	UFUNCTION(BlueprintCallable)
	void ToggleState();
	UFUNCTION()
	void NextTarget();

};