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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:
	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	ADragonBridge();
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere)
	int32 MaxTargetNum = 8;

	UPROPERTY(ReplicatedUsing = OnRep_TargetNum)
	int32 TargetNum = 0;

	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(Replicated)
	FVector TargetLocation;
	UPROPERTY(Replicated)
	bool bIsEnd = false;
	TArray<FVector> Targets;

	UFUNCTION(BlueprintCallable)
	void ToggleState();
private:
	UFUNCTION(Server, Reliable)
	void Server_ToggleState();
	void ToggleState_Internal();
	void NextTarget();
	UFUNCTION()
	void OnRep_TargetNum();
};