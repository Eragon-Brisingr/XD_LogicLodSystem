// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
XD_LOGICLODSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(XD_LogicLodSystem_Log, Log, All);

#define XD_LogicLodSystem_Display_LOG(FMT, ...) UE_LOG(XD_LogicLodSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define XD_LogicLodSystem_Warning_LOG(FMT, ...) UE_LOG(XD_LogicLodSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define XD_LogicLodSystem_Error_Log(FMT, ...) UE_LOG(XD_LogicLodSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)

#define XD_LogicLodSystem_Display_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_LogicLodSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define XD_LogicLodSystem_Warning_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_LogicLodSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define XD_LogicLodSystem_Error_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_LogicLodSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)

XD_LOGICLODSYSTEM_API DECLARE_STATS_GROUP(TEXT("LogicLod"), STATGROUP_LOGIC_LOD, STATCAT_Advanced);