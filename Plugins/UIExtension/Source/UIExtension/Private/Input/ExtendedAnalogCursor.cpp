// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Input/ExtendedAnalogCursor.h"
// UE
#include "Components/Widget.h"
// CommonUI
#include "CommonInputSubsystem.h"
#include "ICommonInputModule.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputSettings.h"
// UIExtension
#include "Input/ExtendedActionRouter.h"
#include "UIExtensionDefine.h"
#include "UIExtensionInputSettings.h"

FExtendedAnalogCursor::FExtendedAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
	: FCommonAnalogCursor(InActionRouter)
{
	// HACK: 因為要通知ActionRouter，必須記錄可以修改的物件，只能強制轉型
	const UExtendedActionRouter* ConstRouter = Cast<UExtendedActionRouter>(&InActionRouter);
	if (ConstRouter)
	{
		UExtendedActionRouter* Router = const_cast<UExtendedActionRouter*>(ConstRouter);
		if (Router)
		{
			ExtendedActionRouter = Router;
		}
	}
}

void FExtendedAnalogCursor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	// 不使用合成游標幫搖桿製造Hover，因此這裡完全重寫FCommonAnalogCursor::Tick

	// Refreshing visibility per tick to address multiplayer p2 cursor visibility getting stuck
	RefreshCursorVisibility();

	// Don't bother trying to do anything while the game viewport has capture
	if (IsUsingGamepad() && IsGameViewportInFocusPathWithoutCapture())
	{
		// The game viewport can't have been focused without a user, so we're quite safe to assume/enforce validity of the user here
		const TSharedRef<FSlateUser> SlateUser = SlateApp.GetUser(GetOwnerUserIndex()).ToSharedRef();

#if WITH_EDITOR
		// Instantly acknowledge any changes to our settings when we're in the editor
		RefreshCursorSettings();
		if (!IsViewportWindowInFocusPath(ActionRouter))
		{
			return;
		}
#endif
		if (bIsAnalogMovementEnabled)
		{
			const FVector2D NewPosition = CalculateTickedCursorPosition(DeltaTime, SlateApp, SlateUser);

			UCommonInputSubsystem& InputSubsystem = ActionRouter.GetInputSubsystem();
			InputSubsystem.SetCursorPosition(NewPosition, false);
		}
		else
		{
			// 不做合成游標移動到目標Widget上
		}

		if (bShouldHandleRightAnalog)
		{
			TimeUntilScrollUpdate -= DeltaTime;
			if (TimeUntilScrollUpdate <= 0.0f && GetAnalogValues(EAnalogStick::Right).SizeSquared() > FMath::Square(ScrollDeadZone))
			{
				// Generate mouse wheel events over all widgets currently registered as scroll recipients
				const TArray<const UWidget*>& AnalogScrollRecipients = ActionRouter.GatherActiveAnalogScrollRecipients();
				if (AnalogScrollRecipients.Num() > 0)
				{
					const FCommonAnalogCursorSettings& CursorSettings = UCommonUIInputSettings::Get().GetAnalogCursorSettings();
					const auto GetScrollAmountFunc = [&CursorSettings](float AnalogValue)
						{
							const float AmountBeyondDeadZone = FMath::Abs(AnalogValue) - CursorSettings.ScrollDeadZone;
							if (AmountBeyondDeadZone <= 0.f)
							{
								return 0.f;
							}
							return (AmountBeyondDeadZone / (1.f - CursorSettings.ScrollDeadZone)) * -FMath::Sign(AnalogValue) * CursorSettings.ScrollMultiplier;
						};

					const FVector2D& RightStickValues = GetAnalogValues(EAnalogStick::Right);
					const FVector2D ScrollAmounts(GetScrollAmountFunc(RightStickValues.X), GetScrollAmountFunc(RightStickValues.Y));

					for (const UWidget* ScrollRecipient : AnalogScrollRecipients)
					{
						check(ScrollRecipient);
						if (ScrollRecipient->GetCachedWidget())
						{
							const EOrientation Orientation = DetermineScrollOrientation(*ScrollRecipient);
							const float ScrollAmount = Orientation == Orient_Vertical ? ScrollAmounts.Y : ScrollAmounts.X;
							if (FMath::Abs(ScrollAmount) > SMALL_NUMBER)
							{
								const FVector2D WidgetCenter = ScrollRecipient->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D(.5f, .5f));
								if (IsInViewport(WidgetCenter))
								{
									FPointerEvent MouseEvent(
										SlateUser->GetUserIndex(),
										FSlateApplication::CursorPointerIndex,
										WidgetCenter,
										WidgetCenter,
										TSet<FKey>(),
										EKeys::MouseWheelAxis,
										ScrollAmount,
										FModifierKeysState());

									SlateApp.ProcessMouseWheelOrGestureEvent(MouseEvent, nullptr);
								}
							}
						}
					}
				}
			}
		}
	}
}

bool FExtendedAnalogCursor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	EExtendedInputType InputType = GetInputType(InKeyEvent);
	if (IsRelevantInputForType(SlateApp, InKeyEvent, InputType))
	{
		RefreshActiveInputType(InputType);
	}

	return FCommonAnalogCursor::HandleKeyDownEvent(SlateApp, InKeyEvent);
}

bool FExtendedAnalogCursor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	EExtendedInputType InputType = GetInputType(InPointerEvent);
	if (IsRelevantInputForType(SlateApp, InPointerEvent, InputType))
	{
		RefreshActiveInputType(InputType);
	}

	return FCommonAnalogCursor::HandleMouseMoveEvent(SlateApp, InPointerEvent);
}

bool FExtendedAnalogCursor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	EExtendedInputType InputType = GetInputType(InPointerEvent);
	if (IsRelevantInputForType(SlateApp, InPointerEvent, InputType))
	{
		RefreshActiveInputType(InputType);
	}

	return FCommonAnalogCursor::HandleMouseButtonDownEvent(SlateApp, InPointerEvent);
}

bool FExtendedAnalogCursor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	EExtendedInputType InputType = GetInputType(InPointerEvent);
	if (IsRelevantInputForType(SlateApp, InPointerEvent, InputType))
	{
		RefreshActiveInputType(InputType);
	}

	return FCommonAnalogCursor::HandleMouseButtonDoubleClickEvent(SlateApp, InPointerEvent);
}

void FExtendedAnalogCursor::Initialize()
{
	FCommonAnalogCursor::Initialize();

	// 初始化輸入類型
	RefreshActiveInputType(GetInputType(ActiveInputMethod));
}

bool FExtendedAnalogCursor::ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const
{
	// 關閉合成游標點擊，搖桿仰賴Focus處理
	return false;
}

void FExtendedAnalogCursor::RefreshCursorVisibility()
{
	bool bUseDefaultInputType = false;
	bool bShowCursorByInputType = false;

	const UUIExtensionInputSettings* InputSettings = GetDefault<UUIExtensionInputSettings>();

	if (InputSettings->bHideCursorWhenUsingKeyboard && ExtendedActionRouter.IsValid())
	{
#if WITH_EDITOR

		// 避免影響到Editor滑鼠使用，只有Standlone才關閉游標
		bool bGameWorldDetected = false;
		if (UWorld* World = ExtendedActionRouter->GetWorld())
		{
			if (World->WorldType == EWorldType::Game)
			{
				bGameWorldDetected = true;
			}
		}

		if (bGameWorldDetected)
		{
			bShowCursorByInputType = ExtendedActionRouter->GetActiveInputType() == EExtendedInputType::Mouse;
		}
		else
		{
			bUseDefaultInputType = true;
		}

#else

		if (ExtendedActionRouter->GetActiveInputType() == EExtendedInputType::Mouse)
		{
			bShowCursor = true;
		}

#endif
	}
	else
	{
		bUseDefaultInputType = true;
	}

	if (bUseDefaultInputType)
	{
		bShowCursorByInputType = ActiveInputMethod == ECommonInputType::MouseAndKeyboard;
	}

	bool bShowCursor = bIsAnalogMovementEnabled || ActionRouter.ShouldAlwaysShowCursor() || bShowCursorByInputType;

	FSlateApplication& SlateApp = FSlateApplication::Get();
	TSharedPtr<FSlateUser> SlateUser = SlateApp.GetUser(GetOwnerUserIndex());
	if (SlateUser.IsValid())
	{
		if (SlateUser == SlateApp.GetCursorUser() && !bShowCursor)
		{
			SlateApp.SetPlatformCursorVisibility(false);
		}

		SlateUser->SetCursorVisibility(bShowCursor);
	}
}

bool FExtendedAnalogCursor::IsViewportWindowInFocusPath(const UCommonUIActionRouterBase& InActionRouter)
{
	// HACK:Common UI未開放這個函式，只能自己複製內容來用

	ULocalPlayer& LocalPlayer = *InActionRouter.GetLocalPlayerChecked();
	if (const TSharedPtr<FSlateUser> SlateUser = FSlateApplication::Get().GetUser(InActionRouter.GetLocalPlayerIndex()))
	{
		if (TSharedPtr<SWindow> Window = LocalPlayer.ViewportClient ? LocalPlayer.ViewportClient->GetWindow() : nullptr)
		{
			return SlateUser->HasFocus(Window) || SlateUser->HasFocusedDescendants(Window.ToSharedRef());
		}
	}
	return true;
}

bool FExtendedAnalogCursor::IsRelevantInputForType(FSlateApplication& SlateApp, const FInputEvent& InputEvent, const EExtendedInputType& DesiredInputType)
{
	// 參考FCommonInputPreprocessor::IsRelevantInput，排除Editor一些狀況

#if WITH_EDITOR
	if (GIntraFrameDebuggingGameThread)
	{
		return false;
	}
#endif

	if (SlateApp.IsActive() || 
		SlateApp.GetHandleDeviceInputWhenApplicationNotActive() || 
		(ICommonInputModule::GetSettings().GetAllowOutOfFocusDeviceInput() && DesiredInputType == EExtendedInputType::Gamepad))
	{
		return FAnalogCursor::IsRelevantInput(InputEvent);
	}

	return false;
}

EExtendedInputType FExtendedAnalogCursor::GetInputType(const ECommonInputType& InInputType) const
{
	switch (ActiveInputMethod)
	{
	// 先以滑鼠為主
	case ECommonInputType::MouseAndKeyboard:
		return EExtendedInputType::Mouse;
		break;
	// 以下照舊
	case ECommonInputType::Gamepad:
		return EExtendedInputType::Gamepad;
		break;
	case ECommonInputType::Touch:
		return EExtendedInputType::Touch;
		break;
	}

	return EExtendedInputType::None;
}

EExtendedInputType FExtendedAnalogCursor::GetInputType(const FKeyEvent& InKeyEvent) const
{
	switch (ActiveInputMethod)
	{
	// 區分鍵盤、滑鼠
	case ECommonInputType::MouseAndKeyboard:
	{
		const FKey& Key = InKeyEvent.GetKey();
		bool bIsKeyboard = !(
			Key.IsGamepadKey()	||
			Key.IsTouch()		||
			Key.IsMouseButton() ||
			Key.IsAnalog()		||
			Key.IsGesture()
			);

		return bIsKeyboard ? EExtendedInputType::Keyboard : EExtendedInputType::None;
	}
	break;
	// 以下照舊
	case ECommonInputType::Gamepad:
		return EExtendedInputType::Gamepad;
		break;
	case ECommonInputType::Touch:
		return EExtendedInputType::Touch;
		break;
	}

	return EExtendedInputType::None;
}

EExtendedInputType FExtendedAnalogCursor::GetInputType(const FPointerEvent& InPointerEvent) const
{
	switch (ActiveInputMethod)
	{
	// 區分鍵盤、滑鼠
	case ECommonInputType::MouseAndKeyboard:
		return InPointerEvent.IsTouchEvent() ? EExtendedInputType::Touch : EExtendedInputType::Mouse;
		break;
	// 以下照舊
	case ECommonInputType::Gamepad:
		return EExtendedInputType::Gamepad;
		break;
	case ECommonInputType::Touch:
		return EExtendedInputType::Touch;
		break;
	}

	return EExtendedInputType::None;
}

void FExtendedAnalogCursor::RefreshActiveInputType(EExtendedInputType InputType)
{
	if (ExtendedActionRouter.IsValid())
	{
		ExtendedActionRouter->SetActiveInputType(InputType);
	}
}
