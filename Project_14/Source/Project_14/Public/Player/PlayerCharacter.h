// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class ALever;

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	WeakJump UMETA(DisplayName = "can Jump but Weak"),
	StrongPush UMETA(DisplayName = "Strong but No Jump")
};

UCLASS()
class PROJECT_14_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterType, EditAnywhere, BlueprintReadWrite)
	ECharacterType CharacterType;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Animation")
	bool bIsPushing = false;

	UFUNCTION()
	void OnRep_CharacterType();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void MouseInput(const FInputActionValue& value);

	void StartPush();
	void StopPush();

	UFUNCTION(Server, Reliable)
	void ServerStartPush();
	UFUNCTION(Server, Reliable)
	void ServerStopPush();

	UFUNCTION(Server, Reliable)
	void Server_MovePushingActor(FVector2D MoveInput);

	void MovePushingActor(const FVector2D& MoveInput);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Push")
	AActor* PushingActor;

	UPROPERTY()
	UPrimitiveComponent* PushingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push")
	float PushSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push")
	float PushWeightMultiplier;
	UPROPERTY(Replicated)
	bool bIsOverlap;
	UPROPERTY()
	TArray<class ALever*> OverlappingLevers;
private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	UFUNCTION(Server, Reliable)
	void Server_Interact();
	UFUNCTION()
	void OnInteractOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnInteractOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};
