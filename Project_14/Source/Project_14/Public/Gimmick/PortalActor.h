#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class UBoxComponent;
class ACharacter;
class UNiagaraComponent;
class APlayerState; 

UCLASS()
class PROJECT_14_API APortalActor : public AActor
{
	GENERATED_BODY()

public:
	APortalActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerTeleport(ACharacter* Character);

	bool TeleportCharacter(ACharacter* Character);

	void RegisterPortalPass(ACharacter* Character);

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PortalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Portal|FX")
	UNiagaraComponent* PortalFX;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Portal")
	APortalActor* LinkedPortal;

	UPROPERTY(EditAnywhere, Category = "Portal")
	float TeleportCooldown = 0.5f;

	TMap<TWeakObjectPtr<ACharacter>, float> LastTeleportTime;

	
	UPROPERTY(EditAnywhere, Category = "Portal|Clear")
	bool bCountsForClear = true;

	// 몇 명이 통과하면 클리어로 인정할지 
	UPROPERTY(EditAnywhere, Category = "Portal|Clear")
	int32 RequiredPlayersToClear = 2;

	// 반복 클리어를 허용할지 
	UPROPERTY(EditAnywhere, Category = "Portal|Clear")
	bool bAllowRepeatClear = false;

	// 이미 이 포탈에서 클리어 처리했는지
	bool bClearedThisPortal = false;

	//  통과한 플레이어 저장(중복 방지) 
	UPROPERTY()
	TSet<TObjectPtr<APlayerState>> PassedPlayers;
};
