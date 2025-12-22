#include "Gimmick/SIC/Lever.h"
#include "Player/PlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gimmick/SIC/DragonBridge.h"
#include "Gimmick/SIC/DragonBridgeJump.h"

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
	if (OtherActor->IsA<APlayerCharacter>())
	{
		bPlayerOverlapping = true;
	}
}

void ALever::OnOverlapEnd(
	UPrimitiveComponent*,
	AActor* OtherActor,
	UPrimitiveComponent*,
	int32
)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		bPlayerOverlapping = false;
	}
}

void ALever::TryInteract(APlayerCharacter* Player)
{
	if (!bPlayerOverlapping)
		return;

	//  클라든 서버든 무조건 서버 함수 호출
	Server_TryInteract(Player);
}

void ALever::Server_TryInteract_Implementation(APlayerCharacter* Player)
{
	UE_LOG(LogTemp, Warning, TEXT("[Lever] Activated (Server)"));

	// DragonBridge 전부 실행
	{
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
	}

	// DragonBridgeJump 전부 실행
	{
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
}
