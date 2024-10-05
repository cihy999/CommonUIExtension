// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

// UE
#include "CoreMinimal.h"
// CommonUI
#include "Input/CommonAnalogCursor.h"

class UCommonUIActionRouterBase;
class UExtendedActionRouter;
enum class ECommonInputType : uint8;
enum class EExtendedInputType : uint8;

/// <summary>
/// 調整CommonAnalogCursor功能
/// <para> 1.關閉合成游標(Synthetic Cursors) </para>
/// <para> 2.區分鍵盤、滑鼠 </para>
/// </summary>
class UIEXTENSION_API FExtendedAnalogCursor : public FCommonAnalogCursor
{
public:
	FExtendedAnalogCursor(const UCommonUIActionRouterBase& InActionRouter);

	// ~Begin IInputProcessor Interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent) override;
	// ~End IInputProcessor Interface

	// ~Begin FCommonAnalogCursor Interface
	virtual void Initialize() override;
	virtual bool ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const override;
	// ~End FCommonAnalogCursor Interface

protected:
	// ~Begin FCommonAnalogCursor Interface
	virtual void RefreshCursorVisibility() override;
	// ~End FCommonAnalogCursor Interface

protected:
	bool IsViewportWindowInFocusPath(const UCommonUIActionRouterBase& ActionRouter);

	bool IsRelevantForInputType(FSlateApplication& SlateApp, const FInputEvent& InputEvent, const EExtendedInputType& DesiredInputType);

	EExtendedInputType GetInputType(const ECommonInputType& InInputType) const;
	EExtendedInputType GetInputType(const FKeyEvent& InKeyEvent) const;
	EExtendedInputType GetInputType(const FPointerEvent& InPointerEvent) const;

	void RefreshActiveInputType(EExtendedInputType InputType);

protected:
	TWeakObjectPtr<UExtendedActionRouter> ExtendedActionRouter;

	float TimeUntilScrollUpdate = 0.f;
	const float ScrollDeadZone = 0.2f;
};
