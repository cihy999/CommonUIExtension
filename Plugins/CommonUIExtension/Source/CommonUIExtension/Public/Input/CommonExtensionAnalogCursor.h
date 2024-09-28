// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonAnalogCursor.h"

class UCommonUIActionRouterBase;

class COMMONUIEXTENSION_API UCommonExtensionAnalogCursor : public FCommonAnalogCursor
{
public:
	UCommonExtensionAnalogCursor(const UCommonUIActionRouterBase& InActionRouter);
};
