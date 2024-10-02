// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonUIActionRouterBase.h"
// CommonUIExtension
#include "CommonUIExtensionDefine.h"
// 
#include "CommonExtensionActionRouter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExInputTypeChangedDelegate, ECommonExInputType, NewActiveInputType);

UCLASS()
class COMMONUIEXTENSION_API UCommonExtensionActionRouter : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

public:
	static UCommonExtensionActionRouter* Get(const UWidget& ContextWidget);

// Input ===========================================================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "CommonExtensionActionRouter")
	ECommonExInputType GetActiveInputType() const;

	UFUNCTION(BlueprintCallable, Category = "CommonExtensionActionRouter")
	void SetActiveInputType(ECommonExInputType NewType);

	DECLARE_EVENT_OneParam(UCommonExtensionActionRouter, FExInputTypeChangedEvent, ECommonExInputType);
	FExInputTypeChangedEvent OnExInputTypeChangedNative;

private:
	UPROPERTY(BlueprintAssignable, Category = "CommonExtensionActionRouter", meta = (AllowPrivateAccess))
	FExInputTypeChangedDelegate OnExInputTypeChanged;

protected:
	void BroadcastInputTypeChanged();

protected:
	ECommonExInputType ActiveInputType = ECommonExInputType::None;
	
// Analog ==========================================================================================================
protected:
	// ~Begin UCommonUIActionRouterBase Interface
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const;
	// ~End UCommonUIActionRouterBase Interface
};
