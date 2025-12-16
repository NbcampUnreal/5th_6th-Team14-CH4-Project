// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/SIC/DragonBridge.h"
#include "Gimmick/SIC/MakerBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ADragonBridge::ADragonBridge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void ADragonBridge::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = SceneRoot->GetRelativeLocation();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	TArray <AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMakerBlock::StaticClass(), Found);

	Targets.SetNum(MaxTargetNum);

	for (AActor* A : Found)
	{
		AMakerBlock* Marker = Cast<AMakerBlock>(A);
		if (!Marker) continue;

		Targets[Marker->TargetIndex] = Marker->GetActorLocation();
	}
}
void ADragonBridge::ToggleState()
{
	if (HasAuthority())
	{
		ToggleState_Internal();
	}
	else
	{
		Server_ToggleState();
	}
}
void ADragonBridge::Server_ToggleState_Implementation()
{
	ToggleState_Internal();
}
void ADragonBridge::ToggleState_Internal()
{
	SceneRoot->SetRelativeLocation(StartLocation);

	TargetNum = 0;
	bIsEnd = false;

	TargetLocation = Targets[0];
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	PrimaryActorTick.SetTickFunctionEnable(true);
	UE_LOG(LogTemp, Warning, TEXT("ToggleState Ativate"));

}
void ADragonBridge::NextTarget()
{
	TargetNum++;

	if (TargetNum >= MaxTargetNum)
	{
		bIsEnd = true;
		return;
	}

	TargetLocation = Targets[TargetNum];

}
// Called every frame
void ADragonBridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) return;

	FVector Current = SceneRoot->GetRelativeLocation();

	UE_LOG(LogTemp, Warning, TEXT("Move!"));

	FVector NewLocation = 
		FMath::VInterpConstantTo(Current, TargetLocation, DeltaTime, MoveSpeed);

	SceneRoot->SetRelativeLocation(NewLocation);

	if (FVector::Dist(Current, TargetLocation) < 1.0f)
	{
		if (bIsEnd)
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			SetActorTickEnabled(false);
			return;
		}
		NextTarget();
	}

}

void ADragonBridge::OnRep_TargetNum()
{

}
void ADragonBridge::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADragonBridge, TargetNum);
	DOREPLIFETIME(ADragonBridge, StartLocation);
	DOREPLIFETIME(ADragonBridge, TargetLocation);
	DOREPLIFETIME(ADragonBridge, bIsEnd);
}