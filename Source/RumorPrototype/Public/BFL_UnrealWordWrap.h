/* ****************************************************************************
 * Written for the Light-Wizzard, UnrealWordWrap is Unreal Engine Word Wrap
 * written in C++ for Blueprints, and is Free Unlicensed Open Source.
 * https://github.com/Light-Wizzard/UnrealWordWrap
*/
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HAL/PlatformFileManager.h"
#include <Misc/Paths.h>
#include <Misc/FileHelper.h>
#include "BFL_UnrealWordWrap.generated.h"
// ****************************************************************************
/// <summary>
/// Unreal Engine Word Wrap API
/// </summary>
UCLASS()
class UNREALWORDWRAP_API UBFL_UnrealWordWrap : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Custom", meta = (Keywords = "LoadTxtFile"))
		static bool LoadTxtFile(FString InputFileName, FString& TextOut);

	UFUNCTION(BlueprintPure, Category = "Custom", meta = (Keywords = "WrapTxt"))
		static bool WrapTxt(FString InputText, int32 wrapAt, int32 TabLen, FString& TextOut);

	UFUNCTION(BlueprintCallable, Category = "Custom", meta = (Keywords = "SaveTxtFile"))
		static bool SaveTxtFile(FString OutputFileName, FString SaveText);

	UFUNCTION(BlueprintCallable, Category = "Custom", meta = (Keywords = "IsFile"))
		static bool IsFile(FString FileName, FString& AbsolutePath);

	UFUNCTION(BlueprintPure, Category = "Custom", meta = (Keywords = "BreakTxt"))
		static bool BreakTxt(FString InputText, FString& TheTextOut);
};
// ***************************** End of File **********************************
