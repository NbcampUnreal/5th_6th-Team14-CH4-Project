// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartDoor.generated.h"

UCLASS()
class PROJECT_14_API AStartDoor : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
public:

	virtual void Tick(float DeltaTime) override;
	AStartDoor();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector ExtendedOffset = FVector(300, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(Replicated)
	bool bHasPushActivator = false;

	UPROPERTY(Replicated)
	bool bHasJumpActivator = false;

	UFUNCTION(BlueprintCallable)
	void TogglePush();
	UFUNCTION(BlueprintCallable)
	void ToggleJump();
private:
	UFUNCTION(Server, Reliable)
	void Server_TogglePush();
	UFUNCTION(Server, Reliable)
	void Server_ToggleJump();

	void TogglePush_Internal();
	void ToggleJump_Internal();


};
