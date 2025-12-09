#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleBall.generated.h"

UCLASS()
class PROJECT_14_API AObstacleBall : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacleBall();

protected:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;
};
