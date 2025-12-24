#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/PlayerCharacter.h"
#include "ProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_14_API AProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	AProjectGameModeBase();
		
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TSubclassOf<APlayerCharacter> StrongPushCharacterBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	TSubclassOf<APlayerCharacter> JumpCharacterBP;

	UFUNCTION(BlueprintCallable)
	void SpawnSelectedCharacter(APlayerController* PC, ECharacterType Type);

	void TryStartGame();
};
