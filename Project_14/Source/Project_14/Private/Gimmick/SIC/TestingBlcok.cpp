#include "Gimmick/SIC/TestingBlcok.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Gimmick/SIC/DragonBridge.h"
#include "Gimmick/SIC/DragonBridgeJump.h"

ATestingBlcok::ATestingBlcok()
{
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	BlockMesh->SetupAttachment(Root);

	// 오버랩 설정
	BlockMesh->SetGenerateOverlapEvents(true);
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlockMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ATestingBlcok::BeginPlay()
{
	Super::BeginPlay();

	// 서버에서만 처리
	if (HasAuthority())
	{
		BlockMesh->OnComponentBeginOverlap.AddDynamic(
			this,
			&ATestingBlcok::OnBlockBeginOverlap
		);
	}
}

void ATestingBlcok::OnBlockBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
		return;

	if (!OtherActor)
		return;

	UE_LOG(LogTemp, Warning, TEXT("[TestingBlcok] Overlap Triggered"));

	//  DragonBridge 전부 실행
	{
		TArray<AActor*> Bridges;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ADragonBridge::StaticClass(),
			Bridges
		);

		for (AActor* A : Bridges)
		{
			ADragonBridge* Bridge = Cast<ADragonBridge>(A);
			if (Bridge)
			{
				UE_LOG(LogTemp, Warning, TEXT("[TestingBlcok] DragonBridge Activate"));
				Bridge->ToggleState();
			}
		}
	}

	//  DragonBridgeJump 전부 실행
	{
		TArray<AActor*> JumpBridges;
		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ADragonBridgeJump::StaticClass(),
			JumpBridges
		);

		for (AActor* A : JumpBridges)
		{
			ADragonBridgeJump* Jump = Cast<ADragonBridgeJump>(A);
			if (Jump)
			{
				UE_LOG(LogTemp, Warning, TEXT("[TestingBlcok] DragonBridgeJump Activate"));
				Jump->ToggleState();
			}
		}
	}
}
