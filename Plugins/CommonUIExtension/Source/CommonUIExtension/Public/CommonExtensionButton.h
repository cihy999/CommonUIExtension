// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "CommonButtonBase.h"
// 
#include "CommonExtensionButton.generated.h"

class SCommonExtensionButton;
enum class ECommonExInputType : uint8;

UCLASS(Experimental)
class COMMONUIEXTENSION_API UCommonExtensionButtonInternal : public UCommonButtonInternalBase
{
	GENERATED_BODY()

// Focus ============================================================================================================
public:
	TSharedPtr<SCommonExtensionButton> GetCachedButton() const;
	bool GetEnableHoverOnFocus() const;

	void SetEnableHoverOnFocus(bool bInEnableHoverOnFocus);

protected:
	UPROPERTY(Transient)
	bool bEnableHoverOnFocus;

// Overriden ==============================================================================================
public:
	UCommonExtensionButtonInternal(const FObjectInitializer& Initializer);

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

// Input ============================================================================================================
protected:
	// ~Begin UCommonButtonBase Interface
	virtual void BindInputMethodChangedDelegate();
	virtual void UnbindInputMethodChangedDelegate();
	// ~End UCommonButtonBase Interface

	virtual void OnExInputTypeChanged(ECommonExInputType NewActiveInputType);

	// 根據InputType決定是否啟用HoverOnFocus
	void UpdateHoverOnFocus(ECommonExInputType NewActiveInputType, bool bTriggerHoverEvent = true);

// Focus ============================================================================================================
protected:
	UFUNCTION(BlueprintCallable)
	bool IsHoverEnabledOnFocus() const;

protected:
	// ~Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	// ~End UUserWidget Interface
	
	// ~Begin UCommonButtonBase Interface
	virtual void HandleFocusReceived() override;
	virtual void HandleFocusLost() override;
	// ~End UCommonButtonBase Interface

// Overriden ==============================================================================================
protected:
	// ~Begin UCommonButtonBase Interface
	virtual bool Initialize() override;
	virtual UCommonButtonInternalBase* ConstructInternalButton() override;
	// ~End UCommonButtonBase Interface

private:
	TWeakObjectPtr<class UCommonExtensionButtonInternal> MyExtensionRootButton;
};
