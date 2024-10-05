// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "CommonButtonBase.h"
// 
#include "ExtendedButton.generated.h"

class SExtendedButton;
enum class EExtendedInputType : uint8;

UCLASS(Experimental)
class UIEXTENSION_API UExtendedButtonInternal : public UCommonButtonInternalBase
{
	GENERATED_BODY()

// Focus ============================================================================================================
public:
	TSharedPtr<SExtendedButton> GetCachedButton() const;
	bool GetEnableHoverOnFocus() const;

	void SetEnableHoverOnFocus(bool bInEnableHoverOnFocus);

protected:
	UPROPERTY(Transient)
	bool bEnableHoverOnFocus;

// Overriden ==============================================================================================
public:
	UExtendedButtonInternal(const FObjectInitializer& Initializer);

protected:
	// ~Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// ~End UWidget Interface

protected:
	/** Cached pointer to the underlying slate button owned by this UWidget */
	TSharedPtr<SExtendedButton> MyExtendedButton;
};

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class UIEXTENSION_API UExtendedButton : public UCommonButtonBase
{
	GENERATED_BODY()

// Input ============================================================================================================
protected:
	// ~Begin UCommonButtonBase Interface
	virtual void BindInputMethodChangedDelegate();
	virtual void UnbindInputMethodChangedDelegate();
	// ~End UCommonButtonBase Interface

	virtual void HandleExtendedInputChanged(EExtendedInputType NewInputType);

	// 根據InputType決定是否啟用HoverOnFocus
	void UpdateHoverOnFocus(EExtendedInputType NewInputType);

	UFUNCTION(BlueprintImplementableEvent, Category = "ExtendedButton", meta = (DisplayName = "On Extended Input Changed"))
	void BP_OnExtendedInputChanged(EExtendedInputType NewInputType);

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
	TWeakObjectPtr<class UExtendedButtonInternal> MyExtendedRootButton;
};
