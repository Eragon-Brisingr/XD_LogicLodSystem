// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
XD_AI_LODSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(XD_AI_LodSystem_Log, Log, All);

#define XD_AI_LodSystem_Display_LOG(FMT, ...) UE_LOG(XD_AI_LodSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define XD_AI_LodSystem_Warning_LOG(FMT, ...) UE_LOG(XD_AI_LodSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define XD_AI_LodSystem_Error_Log(FMT, ...) UE_LOG(XD_AI_LodSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)

#define XD_AI_LodSystem_Display_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_AI_LodSystem_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define XD_AI_LodSystem_Warning_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_AI_LodSystem_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define XD_AI_LodSystem_Error_VLog(LogOwner, FMT, ...) UE_VLOG(LogOwner, XD_AI_LodSystem_Log, Error, TEXT(FMT), ##__VA_ARGS__)
