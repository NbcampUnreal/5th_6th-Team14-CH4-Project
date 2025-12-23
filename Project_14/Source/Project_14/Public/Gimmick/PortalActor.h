#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalActor.generated.h"

class UBoxComponent;
class ACharacter;
class UNiagaraComponent;


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

	void TeleportCharacter(ACharacter* Character);

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

};