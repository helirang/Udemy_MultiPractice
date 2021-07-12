// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "CoopGameGameModeBase.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCoopGameGameModeBase() {}
// Cross Module References
	COOPGAME_API UClass* Z_Construct_UClass_ACoopGameGameModeBase_NoRegister();
	COOPGAME_API UClass* Z_Construct_UClass_ACoopGameGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_CoopGame();
// End Cross Module References
	void ACoopGameGameModeBase::StaticRegisterNativesACoopGameGameModeBase()
	{
	}
	UClass* Z_Construct_UClass_ACoopGameGameModeBase_NoRegister()
	{
		return ACoopGameGameModeBase::StaticClass();
	}
	UClass* Z_Construct_UClass_ACoopGameGameModeBase()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_AGameModeBase();
			Z_Construct_UPackage__Script_CoopGame();
			OuterClass = ACoopGameGameModeBase::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= (EClassFlags)0x20900288u;


				static TCppClassTypeInfo<TCppClassTypeTraits<ACoopGameGameModeBase> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("HideCategories"), TEXT("Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation"));
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("CoopGameGameModeBase.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("CoopGameGameModeBase.h"));
				MetaData->SetValue(OuterClass, TEXT("ShowCategories"), TEXT("Input|MouseInput Input|TouchInput"));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(ACoopGameGameModeBase, 2966365542);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ACoopGameGameModeBase(Z_Construct_UClass_ACoopGameGameModeBase, &ACoopGameGameModeBase::StaticClass, TEXT("/Script/CoopGame"), TEXT("ACoopGameGameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ACoopGameGameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
