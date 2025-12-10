// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GateActor.generated.h"

UCLASS()
class PROJECT_14_API AGateActor : public AActor
{
	GENERATED_BODY()

public:
	AGateActor();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void OpenGate();
	void CloseGate();

protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* GateMesh;

	UPROPERTY(EditAnywhere, Category = "Gate|Slide")
	FVector OpenDirection = FVector(0, 0, 1);

	UPROPERTY(EditAnywhere, Category = "Gate|Slide")
	float MoveDistance = 300.f;

	UPROPERTY(EditAnywhere, Category = "Gate|Slide")
	float SlideSpeed = 200.f;

	FVector ClosedPos;
	FVector OpenPos;

	bool bIsOpen = false;
};
