// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MakerBlock.generated.h"

UCLASS()
class PROJECT_14_API AMakerBlock : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Sets default values for this actor's properties
	AMakerBlock();
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