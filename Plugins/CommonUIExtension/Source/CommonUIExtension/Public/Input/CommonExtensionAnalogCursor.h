// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonAnalogCursor.h"

class UCommonUIActionRouterBase;

/// <summary>
/// 調整CommonAnalogCursor功能
/// <para> 1.關閉合成游標(Synthetic Cursors) </para>
/// </summary>
class COMMONUIEXTENSION_API FCommonExtensionAnalogCursor : public FCommonAnalogCursor
{
public:
	FCommonExtensionAnalogCursor(const UCommonUIActionRouterBase& InActionRouter);

	// ~Begin IInputProcessor Interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	// ~End IInputProcessor Interface

	// ~Begin FCommonAnalogCursor Interface
	virtual bool ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const;
	// ~End FCommonAnalogCursor Interface

protected:
	bool IsViewportWindowInFocusPath(const UCommonUIActionRouterBase& ActionRouter);

protected:
	float TimeUntilScrollUpdate = 0.f;
	const float ScrollDeadZone = 0.2f;
};
