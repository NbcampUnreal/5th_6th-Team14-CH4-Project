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
	OpenPos = ClosedPos + (OpenDirection.GetSafeNormal() * MoveDistance);
	
}

void AGateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FVector Current = GetActorLocation();
	const FVector Target = bIsOpen ? OpenPos : ClosedPos;

	const FVector NewPos = FMath::VInterpConstantTo(Current, Target, DeltaTime, SlideSpeed);
	SetActorLocation(NewPos);

}

void AGateActor::OpenGate()
{
	bIsOpen = true;
}

void AGateActor::CloseGate()
{
	bIsOpen = false;
}

