// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "CommonButtonBase.h"
// 
#include "CommonExtensionButton.generated.h"

class SCommonExtensionButton;

UCLASS(Experimental)
class COMMONUIEXTENSION_API UCommonExtensionButtonInternal : public UCommonButtonInternalBase
{
	GENERATED_BODY()

public:
	TSharedPtr<SCommonExtensionButton> GetCachedButton() const;

// Overriden ==============================================================================================
protected:
	// ~Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// ~End UWidget Interface

protected:
	/** Cached pointer to the underlying slate button owned by this UWidget */
	TSharedPtr<SCommonExtensionButton> MyCommonExtensionButton;
};

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class COMMONUIEXTENSION_API UCommonExtensionButton : public UCommonButtonBase
{
	GENERATED_BODY()

// Overriden ==============================================================================================
protected:
	// ~Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	// ~End UUserWidget Interface

	// ~Begin UCommonButtonBase Interface
	virtual UCommonButtonInternalBase* ConstructInternalButton() override;

	virtual void HandleFocusReceived() override;
	virtual void HandleFocusLost() override;
	// ~End UCommonButtonBase Interface

private:
	TWeakObjectPtr<class UCommonExtensionButtonInternal> MyExtensionRootButton;
};
