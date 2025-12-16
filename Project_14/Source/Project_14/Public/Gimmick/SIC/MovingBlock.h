// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingBlock.generated.h"

UCLASS()
class PROJECT_14_API AMovingBlock : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:	

	virtual void Tick(float DeltaTime) override;
	// Sets default values for this actor's properties
	AMovingBlock();
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector ExtendedOffset = FVector(300, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bIsExtended = false;  // true = 나오는 중, false = 들어가는 중
	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(Replicated)
	FVector TargetLocation;

	UFUNCTION(BlueprintCallable)
	void ToggleState();

private:
	UFUNCTION(Server, Reliable)
	void Server_ToggleState();

	void ToggleState_Internal();


};
