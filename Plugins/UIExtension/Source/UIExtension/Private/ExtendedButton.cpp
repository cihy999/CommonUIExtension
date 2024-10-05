// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "ExtendedButton.h"
// UE
#include "Blueprint/WidgetTree.h"
#include "Components/ButtonSlot.h"
// CommonUI
#include "CommonInputSubsystem.h"
// UIExtension
#include "Input/ExtendedActionRouter.h"
#include "Slate/SExtendedButton.h"
#include "UIExtensionLogs.h"

TSharedPtr<SExtendedButton> UExtendedButtonInternal::GetCachedButton() const
{
	return MyExtendedButton;
}

bool UExtendedButtonInternal::GetEnableHoverOnFocus() const
{
	return bEnableHoverOnFocus;
}

void UExtendedButtonInternal::SetEnableHoverOnFocus(bool bInEnableHoverOnFocus)
{
	if (bEnableHoverOnFocus == bInEnableHoverOnFocus)
	{
		return;
	}

	bEnableHoverOnFocus = bInEnableHoverOnFocus;

	if (MyExtendedButton.IsValid())
	{
		MyExtendedButton->SetIsHoverEnabledOnFocus(bEnableHoverOnFocus);
	}
}

UExtendedButtonInternal::UExtendedButtonInternal(const FObjectInitializer& Initializer)
	: Super(Initializer)
	, bEnableHoverOnFocus(false)
{

}

TSharedRef<SWidget> UExtendedButtonInternal::RebuildWidget()
{
	MyButton = MyCommonButton = MyExtendedButton = SNew(SExtendedButton)
		.ButtonStyle(&GetStyle())
		.ClickMethod(GetClickMethod())
		.TouchMethod(GetTouchMethod())
		.IsFocusable(GetIsFocusable())
		.IsButtonEnabled(bButtonEnabled)
		.IsInteractionEnabled(bInteractionEnabled)
		.IsHoverEnabledOnFocus(bEnableHoverOnFocus)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClickedOverride))
		.OnDoubleClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleDoubleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressedOverride))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleasedOverride))
		.OnReceivedFocus(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleOnReceivedFocus))
		.OnLostFocus(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleOnLostFocus));

	MyBox = SNew(SBox)
		.MinDesiredWidth(MinWidth)
		.MinDesiredHeight(MinHeight)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			MyExtendedButton.ToSharedRef()
		];

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyExtendedButton.ToSharedRef());
	}

	// 必須回傳MyBox，否則MinWidth & MinHeight會失效
	return MyBox.ToSharedRef();
}

void UExtendedButtonInternal::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyExtendedButton.Reset();
	// MyBox由CommonButtonInternalBase處理
}

void UExtendedButton::BindInputMethodChangedDelegate()
{
	Super::BindInputMethodChangedDelegate();

	if (UExtendedActionRouter* ActionRouter = UExtendedActionRouter::Get(*this))
	{
		ActionRouter->OnExtendedInputTypeChangedNative.AddUObject(this, &ThisClass::HandleExtendedInputChanged);
		UpdateHoverOnFocus(ActionRouter->GetActiveInputType());
	}
}

void UExtendedButton::UnbindInputMethodChangedDelegate()
{
	Super::UnbindInputMethodChangedDelegate();

	if (UExtendedActionRouter* ActionRouter = UExtendedActionRouter::Get(*this))
	{
		ActionRouter->OnExtendedInputTypeChangedNative.RemoveAll(this);
	}
}

void UExtendedButton::HandleExtendedInputChanged(EExtendedInputType NewInputType)
{
	UpdateHoverOnFocus(NewInputType);
	BP_OnExtendedInputChanged(NewInputType);
}

void UExtendedButton::UpdateHoverOnFocus(EExtendedInputType NewInputType)
{
	if (!MyExtendedRootButton.IsValid())
	{
		return;
	}

	const bool bWasHovered = IsHovered();

	bool bEnable = NewInputType == EExtendedInputType::Keyboard || NewInputType == EExtendedInputType::Gamepad;
	MyExtendedRootButton->SetEnableHoverOnFocus(bEnable);

	const bool bIsHoveredNow = IsHovered();

	if (IsInteractionEnabled() && bWasHovered != bIsHoveredNow)
	{
		if (bIsHoveredNow)
		{
			NativeOnHovered();
		}
		else
		{
			NativeOnUnhovered();
		}
	}
}

bool UExtendedButton::IsHoverEnabledOnFocus() const
{
	return MyExtendedRootButton.IsValid() && MyExtendedRootButton->GetEnableHoverOnFocus();
}

FReply UExtendedButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (MyExtendedRootButton.IsValid())
	{
		// 收到Focus後，轉為SButton才能收到Accept
		return FReply::Handled().SetUserFocus(MyExtendedRootButton->GetCachedButton().ToSharedRef());
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UExtendedButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (MyExtendedRootButton.IsValid())
	{
		// RootButton存在就讓它處理，本身不管
		return;
	}

	Super::NativeOnFocusLost(InFocusEvent);
}

void UExtendedButton::HandleFocusReceived()
{
	// 重寫 Super::HandleFocusReceived()

	if (bShouldSelectUponReceivingFocus && !GetSelected())
	{
		// Select也會處理Hover Event
		SetIsSelected(true, false);
	}
	else if (IsHoverEnabledOnFocus() && IsHovered())
	{
		// 模擬滑鼠進入達成Hover狀態
		NativeOnMouseEnter(GetCachedGeometry(), FPointerEvent());
	}

	OnFocusReceived().Broadcast();
	BP_OnFocusReceived();
}

void UExtendedButton::HandleFocusLost()
{
	if (IsHoverEnabledOnFocus() && !IsHovered())
	{
		// 模擬滑鼠離開達成Unhover狀態
		NativeOnMouseLeave(FPointerEvent());
	}

	Super::HandleFocusLost();
}

bool UExtendedButton::Initialize()
{
	const bool bInitializedThisCall = Super::Initialize();
	return bInitializedThisCall;
}

UCommonButtonInternalBase* UExtendedButton::ConstructInternalButton()
{
	MyExtendedRootButton = WidgetTree->ConstructWidget<UExtendedButtonInternal>(UExtendedButtonInternal::StaticClass(), FName(TEXT("InternalRootButtonBase")));
	return MyExtendedRootButton.Get();
}
