// Copyright (C) 2024 Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
// 
#include "UIExtensionInputSettings.generated.h"

/**
 * 
 */
UCLASS(config = Input, defaultconfig)
class UIEXTENSION_API UUIExtensionInputSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Input")
	bool bHideCursorWhenUsingKeyboard = false;

// Overriden ===========================================================================================
public:
	virtual FName GetCategoryName() const override;

#if WITH_EDITOR
	virtual FText GetSectionDescription() const override;
#endif
};
