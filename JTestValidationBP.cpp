// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "KismetCompiler.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJTest_ValidationBP, "JTest.Validation.BP", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FJTest_ValidationBP::RunTest(const FString& Parameters)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataList;

    // collect all assets
	AssetRegistryModule.Get().GetAssetsByPath(FName("/Game"), AssetDataList, true);
    const FName TargetClassName = UBlueprint::StaticClass()->GetFName();

	for (const FAssetData& AssetData : AssetDataList)
	{
        if (AssetData.AssetClassPath.GetAssetName() == TargetClassName)
        {
            FString AssetFullPath = AssetData.PackagePath.ToString();

            // load asset
            UObject* Asset = AssetData.GetAsset();
            TestNotNull(FString::Printf(TEXT("Load Asset = %s"), *AssetFullPath), Asset);
            if (Asset)
            {
                if (auto* Blueprint = Cast<UBlueprint>(Asset))
                {
                    FString Desc;

                    FCompilerResultsLog ResultsLog;
                    ResultsLog.bLogDetailedResults = true;
                    ResultsLog.EventDisplayThresholdMs = 0;

                    // testing
                    FKismetEditorUtilities::CompileBlueprint(Blueprint, EBlueprintCompileOptions::None, &ResultsLog);
                    TestFalse(FString::Printf(TEXT("CompileBP = %s"), *AssetFullPath), ResultsLog.NumErrors > 0);
                }
            }
        }        
	}

	return !HasAnyErrors();
}


#endif // #if WITH_DEV_AUTOMATION_TESTS