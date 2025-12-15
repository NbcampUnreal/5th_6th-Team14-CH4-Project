#include "Player/PlayerCharacter.h"
#include "Player/PlayerCtr.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 400.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 2.0f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	bReplicates = true;
	bAlwaysRelevant = true;
	GetCharacterMovement()->SetIsReplicated(true);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bEnablePhysicsInteraction = false;
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CharacterType);
	DOREPLIFETIME(APlayerCharacter, bIsPushing);
}

void APlayerCharacter::OnRep_CharacterType()
{
	// 타입에 따른 능력 초기화 (애님 / 이동 제한 등)
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (APlayerCtr* PlayerController = Cast<APlayerCtr>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::StartJump
				);
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::StartSprint

				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::StopSprint
				);
			}

			if (PlayerController->InteractAction && CharacterType == ECharacterType::StrongPush)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::EnablePushPhysics
				);

				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Completed,
					this,
					&APlayerCharacter::DisablePushPhysics
				);

				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Ongoing,
					this,
					&APlayerCharacter::PushObject
				);
			}

			if (PlayerController->MouseInteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MouseInteractAction,
					ETriggerEvent::Triggered,
					this,
					&APlayerCharacter::MouseInput
				);
			}
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{

	if (!Controller) return;

	FVector2D MoveInput = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MoveInput.X);
		AddMovementInput(RightDirection, MoveInput.Y);
	}
}

void APlayerCharacter::StartJump(const FInputActionValue& value)
{
	if (CharacterType == ECharacterType::WeakJump)
	{
		if (value.Get<bool>())
		{
			Jump();
		}
	}
}

void APlayerCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		FRotator ControlRot = Controller->GetControlRotation();

		ControlRot.Yaw += LookInput.X;
		ControlRot.Pitch = FMath::ClampAngle(ControlRot.Pitch - LookInput.Y, -20.f, 10.f);
		ControlRot.Roll = 0.0f;
		Controller->SetControlRotation(ControlRot);
	}
}

void APlayerCharacter::MouseInput(const FInputActionValue& value)
{

}

void APlayerCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::EnablePushPhysics()
{
	if (CharacterType != ECharacterType::StrongPush) return;

	if (GetCharacterMovement())
	{
		bIsPushing = true;
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed / 3;
		GetCharacterMovement()->bEnablePhysicsInteraction = true;
	}
}

void APlayerCharacter::DisablePushPhysics()
{
	if (CharacterType != ECharacterType::StrongPush) return;

	if (GetCharacterMovement())
	{
		bIsPushing = false;
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		GetCharacterMovement()->bEnablePhysicsInteraction = false;
	}
}

void APlayerCharacter::PushObject(const FInputActionValue& value)
{
	if (CharacterType != ECharacterType::StrongPush) return;

	FVector Start = CameraComp->GetComponentLocation();
	FVector End = Start + CameraComp->GetForwardVector() * PushDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_PhysicsBody,
		Params
	);

	// 디버그 라인 (필요하면 활성화)
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.02f, 0, 1.0f);

	if (bHit && HitResult.GetComponent()->IsSimulatingPhysics())
	{
		ApplyPushToHit(HitResult);
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed / 2;
		bIsPushing = true;  // 물체 미는 중
	}
	else
	{
		bIsPushing = false; // 밀게 없음
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}

void APlayerCharacter::ApplyPushToHit(const FHitResult& Hit)
{
	UPrimitiveComponent* HitComp = Hit.GetComponent();
	if (HitComp && HitComp->IsSimulatingPhysics())
	{
		FVector ForceDir = CameraComp->GetForwardVector();
		ForceDir.Normalize();

		float FinalPush = PushPower;

		HitComp->AddForce(ForceDir * FinalPush, NAME_None, true);
	}
}

void APlayerCharacter::Server_PushObject_Implementation()
{
}

void APlayerCharacter::Server_DisablePush_Implementation()
{
}

void APlayerCharacter::Server_EnablePush_Implementation()
{
}
