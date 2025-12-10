#include "Gimmick/BK/GateActor.h"

AGateActor::AGateActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	RootComponent = GateMesh;

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

	const FVector Current = GetActorLocation();
	const FRotator CurrentRot = GetActorRotation();

	const FVector Target = bIsOpen ? OpenPos : ClosedPos;
	const FRotator TargetRot = bIsOpen ? OpenRot : ClosedRot;


	const FVector NewPos = FMath::VInterpConstantTo(Current, Target, DeltaTime, SlideSpeed);
	const FRotator NewRot = FMath::RInterpConstantTo(CurrentRot, TargetRot, DeltaTime, RotateSpeed);

	SetActorLocationAndRotation(NewPos, NewRot);

}

void AGateActor::OpenGate()
{
	bIsOpen = true;
}

void AGateActor::CloseGate()
{
	bIsOpen = false;
}

