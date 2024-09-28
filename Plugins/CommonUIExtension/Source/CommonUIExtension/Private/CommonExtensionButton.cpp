// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "CommonExtensionButton.h"
// UE
#include "Blueprint/WidgetTree.h"
#include "Components/ButtonSlot.h"
// CommonUIExtension
#include "Slate/SCommonExtensionButton.h"

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

UCommonButtonInternalBase* UCommonExtensionButton::ConstructInternalButton()
{
	MyExtensionRootButton = WidgetTree->ConstructWidget<UCommonExtensionButtonInternal>(UCommonExtensionButtonInternal::StaticClass(), FName(TEXT("InternalRootButtonBase")));
	return MyExtensionRootButton.Get();
}
