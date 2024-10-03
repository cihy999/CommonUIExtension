// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Input/CommonExtensionActionRouter.h"
// UE
#include "Components/Widget.h"
// CommonUIExtension
#include "Input/CommonExtensionAnalogCursor.h"
#include "CommonExtensionLogs.h"

UCommonExtensionActionRouter* UCommonExtensionActionRouter::Get(const UWidget& ContextWidget)
{
    return Cast<UCommonExtensionActionRouter>(Super::Get(ContextWidget));
}

void UCommonExtensionActionRouter::Deinitialize()
{
    Super::Deinitialize();

    if (FSlateApplication::IsInitialized())
    {
        // 如果是CursorUser的話，把UsePlatformCursorForCursorUser調回來
        // 避免關閉時，游標不見...
        FSlateApplication& SlateApplication = FSlateApplication::Get();
        ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();
        bool bCursorUser = LocalPlayer && LocalPlayer->GetSlateUser() == SlateApplication.GetCursorUser();
        if (bCursorUser && ActiveInputType == ECommonExInputType::Keyboard)
        {
            SlateApplication.UsePlatformCursorForCursorUser(true);
        }
    }
}

ECommonExInputType UCommonExtensionActionRouter::GetActiveInputType() const
{
    return ActiveInputType;
}

void UCommonExtensionActionRouter::SetActiveInputType(ECommonExInputType NewType)
{
    if (NewType == ECommonExInputType::None)
    {
        UE_LOG(CommonExInput, Error, TEXT("CommonExtensionActionRouter Set Input Type None!"));
        return;
    }

    if (ActiveInputType != NewType)
    {
        ActiveInputType = NewType;

        // 偵測到鍵盤，重新調整一次UsePlatformCursorForCursorUser
        FSlateApplication& SlateApplication = FSlateApplication::Get();
        ULocalPlayer* LocalPlayer = GetLocalPlayerChecked();
        bool bCursorUser = LocalPlayer && LocalPlayer->GetSlateUser() == SlateApplication.GetCursorUser();
        if (bCursorUser && ActiveInputType == ECommonExInputType::Keyboard)
        {
            SlateApplication.UsePlatformCursorForCursorUser(false);
        }

        BroadcastInputTypeChanged();
    }
}

void UCommonExtensionActionRouter::BroadcastInputTypeChanged()
{
    // 檢查世界再廣播
    if (UWorld* World = GetWorld())
    {
        if (!World->bIsTearingDown)
        {
            OnExInputTypeChangedNative.Broadcast(ActiveInputType);
            OnExInputTypeChanged.Broadcast(ActiveInputType);
        }
    }
}

TSharedRef<FCommonAnalogCursor> UCommonExtensionActionRouter::MakeAnalogCursor() const
{
    return FCommonAnalogCursor::CreateAnalogCursor<FCommonExtensionAnalogCursor>(*this);
}
