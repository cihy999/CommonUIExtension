// Copyright Epic Games, Inc. All Rights Reserved.

#include "CommonEPGameMode.h"
#include "CommonEPCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACommonEPGameMode::ACommonEPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
