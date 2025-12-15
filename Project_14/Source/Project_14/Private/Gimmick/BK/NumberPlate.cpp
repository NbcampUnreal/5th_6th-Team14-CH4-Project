#include "Gimmick/BK/NumberPlate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Gimmick/BK/NumberPlatePuzzleManager.h"

ANumberPlate::ANumberPlate()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	SetRootComponent(PlateMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void ANumberPlate::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(
			this, &ANumberPlate::OnOverlapBegin);

		TriggerBox->OnComponentEndOverlap.AddDynamic(
			this, &ANumberPlate::OnOverlapEnd);
	}
}

void ANumberPlate::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (!OtherActor->IsA(ACharacter::StaticClass()))
	{
		return;
	}

	if (PuzzleManager)
	{
		PuzzleManager->NotifyPlateStepped(PlateNumber);
	}
}

void ANumberPlate::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (!OtherActor->IsA(ACharacter::StaticClass()))
	{
		return;
	}

	if (PuzzleManager)
	{
		PuzzleManager->NotifyPlateReleased(PlateNumber);
	}
}
