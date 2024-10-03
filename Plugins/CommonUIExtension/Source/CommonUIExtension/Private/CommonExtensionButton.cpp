// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "CommonExtensionButton.h"
// UE
#include "Blueprint/WidgetTree.h"
#include "Components/ButtonSlot.h"
// CommonUI
#include "CommonInputSubsystem.h"
// CommonUIExtension
#include "Input/CommonExtensionActionRouter.h"
#include "Slate/SCommonExtensionButton.h"
#include "CommonExtensionLogs.h"

namespace ExCommand
{
	bool bPrintButtonFocusInfo = false;
	FAutoConsoleVariableRef CVarPrintButtonFocusInfo(
		TEXT("CommonUIEx.EnableButtonFocusInfo"),
		bPrintButtonFocusInfo,
		TEXT("Print button focus info."),
		ECVF_Default);
}

TSharedPtr<SCommonExtensionButton> UCommonExtensionButtonInternal::GetCachedButton() const
{
	return MyCommonExtensionButton;
}

bool UCommonExtensionButtonInternal::GetEnableHoverOnFocus() const
{
	return bEnableHoverOnFocus;
}

void UCommonExtensionButtonInternal::SetEnableHoverOnFocus(bool bInEnableHoverOnFocus)
{
	if (bEnableHoverOnFocus == bInEnableHoverOnFocus)
	{
		return;
	}

	bEnableHoverOnFocus = bInEnableHoverOnFocus;

	if (MyCommonExtensionButton.IsValid())
	{
		MyCommonExtensionButton->SetIsHoverEnabledOnFocus(bEnableHoverOnFocus);
	}
}

UCommonExtensionButtonInternal::UCommonExtensionButtonInternal(const FObjectInitializer& Initializer)
	: Super(Initializer)
	, bEnableHoverOnFocus(false)
{

}

TSharedRef<SWidget> UCommonExtensionButtonInternal::RebuildWidget()
{
	MyButton = MyCommonButton = MyCommonExtensionButton = SNew(SCommonExtensionButton)
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
			MyCommonExtensionButton.ToSharedRef()
		];

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyCommonExtensionButton.ToSharedRef());
	}

	// 必須回傳MyBox，否則MinWidth & MinHeight會失效
	return MyBox.ToSharedRef();
}

void UCommonExtensionButtonInternal::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyCommonExtensionButton.Reset();
	// MyBox由CommonButtonInternalBase處理
}

void UCommonExtensionButton::BindInputMethodChangedDelegate()
{
	Super::BindInputMethodChangedDelegate();

	if (UCommonExtensionActionRouter* ActionRouter = UCommonExtensionActionRouter::Get(*this))
	{
		ActionRouter->OnExInputTypeChangedNative.AddUObject(this, &ThisClass::OnExInputTypeChanged);
		UpdateHoverOnFocus(ActionRouter->GetActiveInputType());
	}
}

void UCommonExtensionButton::UnbindInputMethodChangedDelegate()
{
	Super::UnbindInputMethodChangedDelegate();

	if (UCommonExtensionActionRouter* ActionRouter = UCommonExtensionActionRouter::Get(*this))
	{
		ActionRouter->OnExInputTypeChangedNative.RemoveAll(this);
	}
}

void UCommonExtensionButton::OnExInputTypeChanged(ECommonExInputType NewActiveInputType)
{
	UpdateHoverOnFocus(NewActiveInputType);
}

void UCommonExtensionButton::UpdateHoverOnFocus(ECommonExInputType NewActiveInputType, bool bTriggerHoverEvent)
{
	if (!MyExtensionRootButton.IsValid())
	{
		return;
	}

	const bool bWasHovered = IsHovered();
	bool bEnable = NewActiveInputType == ECommonExInputType::Keyboard || NewActiveInputType == ECommonExInputType::Gamepad;

	MyExtensionRootButton->SetEnableHoverOnFocus(bEnable);

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

bool UCommonExtensionButton::IsHoverEnabledOnFocus() const
{
	return MyExtensionRootButton.IsValid() && MyExtensionRootButton->GetEnableHoverOnFocus();
}

FReply UCommonExtensionButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		UE_LOG(CommonExWidget, Log, TEXT("%s::NativeOnFocusReceived"), *GetName());
	}

	if (MyExtensionRootButton.IsValid())
	{
		// 收到Focus後，轉為SButton才能收到Accept
		return FReply::Handled().SetUserFocus(MyExtensionRootButton->GetCachedButton().ToSharedRef());
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UCommonExtensionButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		// Print Log
		UE_LOG(CommonExWidget, Log, TEXT("%s::NativeOnFocusLost"), *GetName());
	}

	if (MyExtensionRootButton.IsValid())
	{
		// RootButton存在就讓它處理，本身不管
		return;
	}

	Super::NativeOnFocusLost(InFocusEvent);
}

void UCommonExtensionButton::HandleFocusReceived()
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		UE_LOG(CommonExWidget, Log, TEXT("%s::HandleFocusReceived"), *GetName());
	}

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

void UCommonExtensionButton::HandleFocusLost()
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		// Print Log
		UE_LOG(CommonExWidget, Log, TEXT("%s::HandleFocusLost"), *GetName());
	}

	if (IsHoverEnabledOnFocus() && !IsHovered())
	{
		// 模擬滑鼠離開達成Unhover狀態
		NativeOnMouseLeave(FPointerEvent());
	}

	Super::HandleFocusLost();
}

bool UCommonExtensionButton::Initialize()
{
	const bool bInitializedThisCall = Super::Initialize();
	return bInitializedThisCall;
}

UCommonButtonInternalBase* UCommonExtensionButton::ConstructInternalButton()
{
	MyExtensionRootButton = WidgetTree->ConstructWidget<UCommonExtensionButtonInternal>(UCommonExtensionButtonInternal::StaticClass(), FName(TEXT("InternalRootButtonBase")));
	return MyExtensionRootButton.Get();
}
