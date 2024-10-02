// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "CommonExtensionButton.h"
// UE
#include "Blueprint/WidgetTree.h"
#include "Components/ButtonSlot.h"
// CommonUIExtension
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

UCommonExtensionButtonInternal::UCommonExtensionButtonInternal(const FObjectInitializer& Initializer)
	: Super(Initializer)
	, bEnableHoverOnFocus(true)
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

bool UCommonExtensionButton::IsHoverEnabledOnFocus() const
{
	return MyExtensionRootButton.IsValid() && MyExtensionRootButton->GetEnableHoverOnFocus();
}

FReply UCommonExtensionButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::NativeOnFocusReceived"), *GetName());
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
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::NativeOnFocusLost"), *GetName());
	}

	if (MyExtensionRootButton.IsValid())
	{
		// 收到Focus後，轉為SButton才能收到Accept

		// RootButton存在就讓它處理，本身不管
		return;
	}

	Super::NativeOnFocusLost(InFocusEvent);
}

void UCommonExtensionButton::HandleFocusReceived()
{
	if (ExCommand::bPrintButtonFocusInfo)
	{
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::HandleFocusReceived"), *GetName());
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
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::HandleFocusLost"), *GetName());
	}

	if (IsHoverEnabledOnFocus() && !IsHovered())
	{
		// 模擬滑鼠離開達成Unhover狀態
		NativeOnMouseLeave(FPointerEvent());
	}

	Super::HandleFocusLost();
}

UCommonButtonInternalBase* UCommonExtensionButton::ConstructInternalButton()
{
	MyExtensionRootButton = WidgetTree->ConstructWidget<UCommonExtensionButtonInternal>(UCommonExtensionButtonInternal::StaticClass(), FName(TEXT("InternalRootButtonBase")));
	return MyExtensionRootButton.Get();
}
