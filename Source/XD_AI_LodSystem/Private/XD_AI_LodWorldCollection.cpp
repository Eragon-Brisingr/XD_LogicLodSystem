// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "XD_AI_LodWorldCollection.h"
#include <Engine/Level.h>

#define LOCTEXT_NAMESPACE "XD_AI_LodSystem"

FName FAI_LodSystemUtils::GetLevelName(const ULevel* Level)
{
	return Level->GetOuter()->GetFName();
}

#undef LOCTEXT_NAMESPACE
