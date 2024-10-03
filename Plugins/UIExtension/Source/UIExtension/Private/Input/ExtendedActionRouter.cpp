// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Input/ExtendedActionRouter.h"
// UE
#include "Components/Widget.h"
// UIExtension
#include "Input/ExtendedAnalogCursor.h"
#include "UIExtensionLogs.h"

UExtendedActionRouter* UExtendedActionRouter::Get(const UWidget& ContextWidget)
{
    return Cast<UExtendedActionRouter>(Super::Get(ContextWidget));
}

void UExtendedActionRouter::Deinitialize()
{
    Super::Deinitialize();
}

EExtendedInputType UExtendedActionRouter::GetActiveInputType() const
{
    return ActiveInputType;
}

void UExtendedActionRouter::SetActiveInputType(EExtendedInputType NewType)
{
    if (NewType == EExtendedInputType::None)
    {
        UE_LOG(ExtendedInput, Error, TEXT("CommonExtensionActionRouter Set Input Type None!"));
        return;
    }

    if (ActiveInputType != NewType)
    {
        ActiveInputType = NewType;

        bool bEnableModify = true;

#if WITH_EDITOR
        // PIE模式下都維持原始狀態，避免影響到滑鼠使用
        if (UWorld* World = GetWorld())
        {
            if (World->WorldType != EWorldType::Game)
            {
                bEnableModify = false;
            }
        }
#endif

        if (bEnableModify)
        {
            // 偵測到鍵盤，重新調整一次UsePlatformCursorForCursorUser
            FSlateApplication& SlateApplication = FSlateApplication::Get();
            ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();
            bool bCursorUser = LocalPlayer && LocalPlayer->GetSlateUser() == SlateApplication.GetCursorUser();
            if (bCursorUser && ActiveInputType == EExtendedInputType::Keyboard)
            {
                SlateApplication.UsePlatformCursorForCursorUser(false);
            }
        }

        BroadcastInputTypeChanged();
    }
}

void UExtendedActionRouter::BroadcastInputTypeChanged()
{
    // 檢查世界再廣播
    if (UWorld* World = GetWorld())
    {
        if (!World->bIsTearingDown)
        {
            OnExtendedInputTypeChangedNative.Broadcast(ActiveInputType);
            OnExtendedInputTypeChanged.Broadcast(ActiveInputType);
        }
    }
}

TSharedRef<FCommonAnalogCursor> UExtendedActionRouter::MakeAnalogCursor() const
{
    return FCommonAnalogCursor::CreateAnalogCursor<FExtendedAnalogCursor>(*this);
}
