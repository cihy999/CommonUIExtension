// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "CommonExtensionButton.h"
// UE
#include "Blueprint/WidgetTree.h"
#include "Components/ButtonSlot.h"
// CommonUIExtension
#include "Slate/SCommonExtensionButton.h"
#include "CommonExtensionLogs.h"

namespace CommonUIEx::Command
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

TSharedRef<SWidget> UCommonExtensionButtonInternal::RebuildWidget()
{
	MyButton = MyCommonButton = MyCommonExtensionButton = SNew(SCommonExtensionButton)
		.ButtonStyle(&GetStyle())
		.ClickMethod(GetClickMethod())
		.TouchMethod(GetTouchMethod())
		.IsFocusable(GetIsFocusable())
		.IsButtonEnabled(bButtonEnabled)
		.IsInteractionEnabled(bInteractionEnabled)
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

	return MyBox.ToSharedRef();
}

void UCommonExtensionButtonInternal::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyCommonExtensionButton.Reset();
	// MyBox由CommonButtonInternalBase處理
}

FReply UCommonExtensionButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (CommonUIEx::Command::bPrintButtonFocusInfo)
	{
		// Print Log
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
	if (CommonUIEx::Command::bPrintButtonFocusInfo)
	{
		// Print Log
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::NativeOnFocusLost"), *GetName());
	}

	Super::NativeOnFocusLost(InFocusEvent);
}

UCommonButtonInternalBase* UCommonExtensionButton::ConstructInternalButton()
{
	MyExtensionRootButton = WidgetTree->ConstructWidget<UCommonExtensionButtonInternal>(UCommonExtensionButtonInternal::StaticClass(), FName(TEXT("InternalRootButtonBase")));
	return MyExtensionRootButton.Get();
}

void UCommonExtensionButton::HandleFocusReceived()
{
	if (CommonUIEx::Command::bPrintButtonFocusInfo)
	{
		// Print Log
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::HandleFocusReceived"), *GetName());
	}

	Super::HandleFocusReceived();
}

void UCommonExtensionButton::HandleFocusLost()
{
	if (CommonUIEx::Command::bPrintButtonFocusInfo)
	{
		// Print Log
		UE_LOG(CommonWidgetEx, Log, TEXT("%s::HandleFocusLost"), *GetName());
	}

	Super::HandleFocusLost();
}
