#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonBridgeJump.generated.h"

UCLASS()
class PROJECT_14_API ADragonBridgeJump : public AActor
{
	GENERATED_BODY()

public:
	ADragonBridgeJump();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, Category = "Movement")
	int32 MaxTargetNum = 10;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 200.f;

	UPROPERTY(Replicated)
	FVector StartLocation;

	UPROPERTY(ReplicatedUsing = OnRep_TargetNum)
	int32 TargetNum = 0;

	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(Replicated)
	bool bIsEnd = false;

	TArray<FVector> Targets;

	// 시작 중복 방지
	UPROPERTY()
	bool bHasStarted = false;

	// 정지 상태
	UPROPERTY(Replicated)
	bool bIsStopped = false;

public:
	// 기존: 다리 시작
	UFUNCTION(BlueprintCallable)
	void ToggleState();

	// 신규: 다리 즉시 정지
	UFUNCTION(BlueprintCallable)
	void StopBridge();

private:
	// 실제 시작 로직
	void ToggleState_Internal();

	// 실제 정지 로직
	void StopBridge_Internal();

	// 다음 목표 지점
	void NextTarget();

	// Stop RPC
	UFUNCTION(Server, Reliable)
	void Server_StopBridge();

	// 모든 클라에 정지 반영
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyStop();

	UFUNCTION()
	void OnRep_TargetNum();
};
