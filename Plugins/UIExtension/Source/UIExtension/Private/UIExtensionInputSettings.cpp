// Copyright (C) 2024 Cindy Chen. All Rights Reserved.

// Self
#include "UIExtensionInputSettings.h"

FName UUIExtensionInputSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText UUIExtensionInputSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("Input Settings for UI Extension."));
}
#endif
