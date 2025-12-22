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
#include "Gimmick/SIC/Lever.h"

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

	bIsPushing = false;
	PushSpeed = 200.f;
	PushWeightMultiplier = 1.0f;
	PushingActor = nullptr;
	PushingComponent = nullptr;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	if (bIsPushing && HasAuthority() && PushingActor)
	{
		FVector ToActor = (PushingActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector Forward = GetActorForwardVector();

		float Dot = FVector::DotProduct(Forward, ToActor);
		if (Dot < FMath::Cos(FMath::DegreesToRadians(30.0f)))
		{
			StopPush();
			return;
		}

		FVector Start = GetActorLocation();
		FVector End = Start + Forward * 100.f;
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (!GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
			ECC_WorldDynamic, FCollisionShape::MakeCapsule(28.f, 88.f), Params))
		{
			StopPush();
		}
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CharacterType);
	DOREPLIFETIME(APlayerCharacter, bIsPushing);
	DOREPLIFETIME(APlayerCharacter, PushingActor);
}

void APlayerCharacter::OnRep_CharacterType()
{

	if (CharacterType == ECharacterType::StrongPush)
	{
		GetCharacterMovement()->JumpZVelocity = 0.f;
	}
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

			if (PlayerController->InteractAction)
			{
				EnhancedInput->BindAction(
					PlayerController->InteractAction, 
					ETriggerEvent::Triggered, 
					this, 
					&APlayerCharacter::StartPush
				);

				EnhancedInput->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Completed, 
					this, 
					&APlayerCharacter::StopPush
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
		FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector MoveDir = Forward * MoveInput.X + Right * MoveInput.Y;

		AddMovementInput(Forward, MoveInput.X);
		AddMovementInput(Right, MoveInput.Y);

		if (bIsPushing && PushingActor)
		{
			Server_MovePushingActor(MoveInput);
		}
	}
}

void APlayerCharacter::Server_MovePushingActor_Implementation(FVector2D MoveInput)
{
	MovePushingActor(MoveInput);
}

void APlayerCharacter::MovePushingActor(const FVector2D& MoveInput)
{
	if (!HasAuthority() || !PushingActor) return;

	FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
	FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	FVector Dir = (Forward * MoveInput.X + Right * MoveInput.Y).GetSafeNormal();

	FVector DeltaMove = Dir * PushSpeed * PushWeightMultiplier * GetWorld()->GetDeltaSeconds();
	PushingActor->AddActorWorldOffset(DeltaMove, true);
	PushingActor->ForceNetUpdate();

	DrawDebugSphere(GetWorld(), PushingActor->GetActorLocation(), 30.f, 8, FColor::Red, false, 0.1f);
}

void APlayerCharacter::StartPush()
{
	if (CharacterType != ECharacterType::StrongPush) return;

	if (!HasAuthority())
	{
		ServerStartPush();
		return;
	}

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 100.f;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeCapsule(34.f, 88.f), Params))
	{
		PushingActor = Hit.GetActor();
		PushingComponent = Hit.GetComponent();

		if (PushingActor)
		{
			bIsPushing = true;
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed / 3.f;
		}
	}
}

void APlayerCharacter::StopPush()
{
	if (!HasAuthority())
	{
		ServerStopPush();
		return;
	}

	bIsPushing = false;
	PushingActor = nullptr;
	PushingComponent = nullptr;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void APlayerCharacter::ServerStartPush_Implementation() 
{ 
	StartPush(); 
}
void APlayerCharacter::ServerStopPush_Implementation() 
{ 
	StopPush(); 
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

void APlayerCharacter::MouseInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Right Click!"));
	if (!Value.Get<bool>())
		return;

	if (!CurrentLever)
		return;

	//  핵심: 소유 액터(플레이어)에서 서버 RPC 호출
	Server_InteractLever(CurrentLever);
}
void APlayerCharacter::Server_InteractLever_Implementation(ALever* Lever)
{
	if (!IsValid(Lever))
		return;

	// 서버에서 레버 실행 (서버 권한이므로 HasAuthority true)
	Lever->TryInteract(this);
}

