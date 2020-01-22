// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_LogicLodWorldCollection.h"
#include <Engine/Level.h>

#define LOCTEXT_NAMESPACE "XD_LogicLodSystem"

FName FLogicLodSystemUtils::GetLevelName(const ULevel* Level)
{
	return Level->GetOuter()->GetFName();
}

#undef LOCTEXT_NAMESPACE
