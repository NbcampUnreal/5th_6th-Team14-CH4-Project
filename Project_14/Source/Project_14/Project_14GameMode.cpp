// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project_14GameMode.h"
#include "Project_14Character.h"
#include "UObject/ConstructorHelpers.h"

AProject_14GameMode::AProject_14GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
