// Copyright (C) Cindy Chen. All Rights Reserved.

// Self
#include "Input/CommonExtensionActionRouter.h"
// CommonUIExtension
#include "Input/CommonExtensionAnalogCursor.h"

TSharedRef<FCommonAnalogCursor> UCommonExtensionActionRouter::MakeAnalogCursor() const
{
    return FCommonAnalogCursor::CreateAnalogCursor<UCommonExtensionAnalogCursor>(*this);
}
