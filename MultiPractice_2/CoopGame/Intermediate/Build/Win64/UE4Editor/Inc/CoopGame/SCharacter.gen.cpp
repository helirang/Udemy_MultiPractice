// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/SCharacter.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSCharacter() {}
// Cross Module References
	COOPGAME_API UClass* Z_Construct_UClass_ASCharacter_NoRegister();
	COOPGAME_API UClass* Z_Construct_UClass_ASCharacter();
	ENGINE_API UClass* Z_Construct_UClass_ACharacter();
	UPackage* Z_Construct_UPackage__Script_CoopGame();
	ENGINE_API UClass* Z_Construct_UClass_USpringArmComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UCameraComponent_NoRegister();
// End Cross Module References
	void ASCharacter::StaticRegisterNativesASCharacter()
	{
	}
	UClass* Z_Construct_UClass_ASCharacter_NoRegister()
	{
		return ASCharacter::StaticClass();
	}
	UClass* Z_Construct_UClass_ASCharacter()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_ACharacter();
			Z_Construct_UPackage__Script_CoopGame();
			OuterClass = ASCharacter::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= (EClassFlags)0x20900080u;


				UProperty* NewProp_SpringArmComp = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("SpringArmComp"), RF_Public|RF_Transient|RF_MarkAsNative) UObjectProperty(CPP_PROPERTY_BASE(SpringArmComp, ASCharacter), 0x00200800000a001d, Z_Construct_UClass_USpringArmComponent_NoRegister());
				UProperty* NewProp_CameraComp = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("CameraComp"), RF_Public|RF_Transient|RF_MarkAsNative) UObjectProperty(CPP_PROPERTY_BASE(CameraComp, ASCharacter), 0x00200800000a001d, Z_Construct_UClass_UCameraComponent_NoRegister());
				static TCppClassTypeInfo<TCppClassTypeTraits<ASCharacter> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("HideCategories"), TEXT("Navigation"));
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("SCharacter.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("Public/SCharacter.h"));
				MetaData->SetValue(NewProp_SpringArmComp, TEXT("Category"), TEXT("Components"));
				MetaData->SetValue(NewProp_SpringArmComp, TEXT("EditInline"), TEXT("true"));
				MetaData->SetValue(NewProp_SpringArmComp, TEXT("ModuleRelativePath"), TEXT("Public/SCharacter.h"));
				MetaData->SetValue(NewProp_CameraComp, TEXT("Category"), TEXT("Components"));
				MetaData->SetValue(NewProp_CameraComp, TEXT("EditInline"), TEXT("true"));
				MetaData->SetValue(NewProp_CameraComp, TEXT("ModuleRelativePath"), TEXT("Public/SCharacter.h"));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASCharacter, 1238913600);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASCharacter(Z_Construct_UClass_ASCharacter, &ASCharacter::StaticClass, TEXT("/Script/CoopGame"), TEXT("ASCharacter"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASCharacter);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
