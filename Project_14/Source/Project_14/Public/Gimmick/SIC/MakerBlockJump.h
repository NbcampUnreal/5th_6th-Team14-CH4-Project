// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MakerBlockJump.generated.h"

UCLASS()
class PROJECT_14_API AMakerBlockJump : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Sets default values for this actor's properties
	AMakerBlockJump();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	int32 TargetIndex = 0;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;
	UPROPERTY(EditAnywhere)
	bool bIsLastTarget;


	virtual void Tick(float DeltaTime) override;

};