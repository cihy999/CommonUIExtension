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
