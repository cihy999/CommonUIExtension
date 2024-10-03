// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonUIActionRouterBase.h"
// CommonUIExtension
#include "UIExtensionDefine.h"
// 
#include "ExtendedActionRouter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExtendedInputTypeChangedDelegate, EExtendedInputType, NewInputType);

UCLASS()
class UIEXTENSION_API UExtendedActionRouter : public UCommonUIActionRouterBase
{
	GENERATED_BODY()

public:
	static UExtendedActionRouter* Get(const UWidget& ContextWidget);

	// ~Begin USubsystem Interface
	virtual void Deinitialize() override;
	// ~End USubsystem Interface

// Input ===========================================================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "CommonExtensionActionRouter")
	EExtendedInputType GetActiveInputType() const;

	UFUNCTION(BlueprintCallable, Category = "CommonExtensionActionRouter")
	void SetActiveInputType(EExtendedInputType NewType);

	DECLARE_EVENT_OneParam(UExtendedActionRouter, FExtendedInputTypeChangedEvent, EExtendedInputType);
	FExtendedInputTypeChangedEvent OnExtendedInputTypeChangedNative;

private:
	UPROPERTY(BlueprintAssignable, Category = "CommonExtensionActionRouter", meta = (AllowPrivateAccess))
	FExtendedInputTypeChangedDelegate OnExtendedInputTypeChanged;

protected:
	void BroadcastInputTypeChanged();

protected:
	EExtendedInputType ActiveInputType = EExtendedInputType::None;
	
// Analog ==========================================================================================================
protected:
	// ~Begin UCommonUIActionRouterBase Interface
	virtual TSharedRef<FCommonAnalogCursor> MakeAnalogCursor() const;
	// ~End UCommonUIActionRouterBase Interface
};
