// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "MainGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, BlueprintType)
class SURFACEGENERATORV3_API UMainGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, BlueprintReadWrite)
	int ScreenPercentage = 100;
	UPROPERTY(Config, BlueprintReadWrite)
	bool ShowFPS = false;
	UPROPERTY(Config, BlueprintReadWrite)
	int RenderDistance = 16;
	UPROPERTY(Config, BlueprintReadWrite)
	int ZRenderDistance = 9;
};
