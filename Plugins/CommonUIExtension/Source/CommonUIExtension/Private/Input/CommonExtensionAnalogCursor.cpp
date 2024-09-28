// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Input/CommonExtensionAnalogCursor.h"
// UE
#include "Components/Widget.h"
// CommonUI
#include "CommonInputSubsystem.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Input/CommonUIInputSettings.h"

FCommonExtensionAnalogCursor::FCommonExtensionAnalogCursor(const UCommonUIActionRouterBase& InActionRouter)
	: FCommonAnalogCursor(InActionRouter)
{

}

void FCommonExtensionAnalogCursor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
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

bool FCommonExtensionAnalogCursor::ShouldVirtualAcceptSimulateMouseButton(const FKeyEvent& InKeyEvent, EInputEvent InputEvent) const
{
	// 關閉合成游標點擊，搖桿仰賴Focus處理
	return false;
}

bool FCommonExtensionAnalogCursor::IsViewportWindowInFocusPath(const UCommonUIActionRouterBase& InActionRouter)
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
