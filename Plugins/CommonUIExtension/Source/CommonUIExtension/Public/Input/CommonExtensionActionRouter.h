// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonUIActionRouterBase.h"
// 
#include "CommonExtensionActionRouter.generated.h"

UCLASS()
class COMMONUIEXTENSION_API UCommonExtensionActionRouter : public UCommonUIActionRouterBase
{
	GENERATED_BODY()
	
// Analog ==========================================================================================================
protected:
	// ~Begin UCommonUIActionRouterBase Interface
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const;
	// ~End UCommonUIActionRouterBase Interface
};
