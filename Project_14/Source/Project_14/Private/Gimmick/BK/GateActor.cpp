#include "Gimmick/BK/GateActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AGateActor::AGateActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;            
	SetReplicateMovement(true);     

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	SetRootComponent(GateMesh);
}

void AGateActor::BeginPlay()
{
	Super::BeginPlay();

	ClosedPos = GetActorLocation();
	ClosedRot = GetActorRotation();

	OpenPos = ClosedPos + (OpenDirection.GetSafeNormal() * MoveDistance);
	OpenRot = ClosedRot + OpenRotationOffset;
}

void AGateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector TargetPos = bIsOpen ? OpenPos : ClosedPos;
	const FRotator TargetRot = bIsOpen ? OpenRot : ClosedRot;

	const FVector NewPos =
		FMath::VInterpConstantTo(GetActorLocation(), TargetPos, DeltaTime, SlideSpeed);

	const FRotator NewRot =
		FMath::RInterpConstantTo(GetActorRotation(), TargetRot, DeltaTime, RotateSpeed);

	SetActorLocationAndRotation(NewPos, NewRot);
}

void AGateActor::OpenGate()
{
	if (HasAuthority())
	{
		bIsOpen = true;
		OnRep_GateState();
	}
	else
	{
		ServerOpenGate();
	}
}

void AGateActor::CloseGate()
{
	if (HasAuthority())
	{
		bIsOpen = false;
		OnRep_GateState();
	}
	else
	{
		ServerCloseGate();
	}
}

void AGateActor::ServerOpenGate_Implementation()
{
	bIsOpen = true;
	OnRep_GateState();
}

void AGateActor::ServerCloseGate_Implementation()
{
	bIsOpen = false;
	OnRep_GateState();
}

void AGateActor::OnRep_GateState()
{
	
}


void AGateActor::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGateActor, bIsOpen);
}
