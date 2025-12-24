#include "Gimmick/SIC/Lever.h"
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/SIC/DragonBridge.h"
#include "Gimmick/SIC/DragonBridgeJump.h"
#include "Gimmick/SIC/MovingBlock.h"

ALever::ALever()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;

	LeverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverMesh"));
	SetRootComponent(LeverMesh);

	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractBox"));
	InteractBox->SetupAttachment(RootComponent);

	InteractBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractBox->SetGenerateOverlapEvents(true);

	InteractBox->OnComponentBeginOverlap.AddDynamic(this, &ALever::OnOverlapBegin);
	InteractBox->OnComponentEndOverlap.AddDynamic(this, &ALever::OnOverlapEnd);
}

void ALever::OnOverlapBegin(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32,
	bool,
	const FHitResult&
)
{
	UE_LOG(LogTemp, Error, TEXT("[Lever] Overlap Begin with %s"), *GetNameSafe(OtherActor));
}

void ALever::OnOverlapEnd(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32
)
{
	UE_LOG(LogTemp, Error, TEXT("[Lever] Overlap End with %s"), *GetNameSafe(OtherActor));
}

void ALever::Server_TryInteract_Implementation(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Error, TEXT("[Lever][Server] TryInteract"));

	if (!IsValid(Player))
		return;

	if (!InteractBox->IsOverlappingActor(Player))
	{
		UE_LOG(LogTemp, Error, TEXT("[Lever][Server] NOT overlapping"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("[Lever][Server] ACTIVATED"));
	if(bIsDragonBridge)
	{
		// DragonBridge
		TArray<AActor*> Bridges;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ADragonBridge::StaticClass(),
			Bridges
		);

		for (AActor* A : Bridges)
		{
			if (ADragonBridge* Bridge = Cast<ADragonBridge>(A))
			{
				Bridge->ToggleState();
			}
		}

		// DragonBridgeJump
		TArray<AActor*> JumpBridges;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ADragonBridgeJump::StaticClass(),
			JumpBridges
		);

		for (AActor* A : JumpBridges)
		{
			if (ADragonBridgeJump* Jump = Cast<ADragonBridgeJump>(A))
			{
				Jump->ToggleState();
			}
		}
	}
	else
	{
		// MovingBlock
		TArray<AActor*> MBlock;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			AMovingBlock::StaticClass(),
			MBlock
		);

		for (AActor* A : MBlock)
		{
			if (AMovingBlock* MB = Cast<AMovingBlock>(A))
			{
				MB->ToggleState();
			}
		}
	}
}
