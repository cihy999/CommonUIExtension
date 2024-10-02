// Copyright (C) Cindy Chen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 區分滑鼠、鍵盤的輸入類型
UENUM(BlueprintType)
enum class ECommonExInputType : uint8
{
	None,
	Mouse,
	Keyboard,
	Gamepad,
	Touch,
};
