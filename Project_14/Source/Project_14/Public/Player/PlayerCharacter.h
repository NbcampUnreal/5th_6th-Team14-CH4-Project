// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	ECharacterType CharacterType;

	virtual void BeginPlay() override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void PushObject(const FInputActionValue& value);

	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprint(const FInputActionValue& value);
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprint(const FInputActionValue& value);

	void EnablePushPhysics();
	void DisablePushPhysics();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push")
	bool bIsPushing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push")
	float PushDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push")
	float PushPower = 1500.0f;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;

	void ApplyPushToHit(const FHitResult& Hit);
};
