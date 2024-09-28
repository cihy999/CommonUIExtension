// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Slate/SCommonExtensionButton.h"

void SCommonExtensionButton::Construct(const FArguments& InArgs)
{
	SCommonButton::Construct(SCommonButton::FArguments()
		.ButtonStyle(InArgs._ButtonStyle)
		.HAlign(InArgs._HAlign)
		.VAlign(InArgs._VAlign)
		.ClickMethod(InArgs._ClickMethod)
		.TouchMethod(InArgs._TouchMethod)
		.PressMethod(InArgs._PressMethod)
		.IsFocusable(InArgs._IsFocusable)
		.IsButtonEnabled(InArgs._IsButtonEnabled)
		.IsInteractionEnabled(InArgs._IsInteractionEnabled)
		.OnClicked(InArgs._OnClicked)
		.OnDoubleClicked(InArgs._OnDoubleClicked)
		.OnPressed(InArgs._OnPressed)
		.OnReleased(InArgs._OnReleased)
		.OnReceivedFocus(InArgs._OnReceivedFocus)
		.OnLostFocus(InArgs._OnLostFocus)
		.Content()
		[
			InArgs._Content.Widget
		]
	);
}
