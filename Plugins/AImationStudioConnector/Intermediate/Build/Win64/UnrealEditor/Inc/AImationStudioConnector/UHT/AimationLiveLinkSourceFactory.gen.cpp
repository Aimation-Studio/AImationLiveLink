// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AImationStudioConnector/Public/AimationLiveLinkSourceFactory.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAimationLiveLinkSourceFactory() {}
// Cross Module References
	AIMATIONSTUDIOCONNECTOR_API UClass* Z_Construct_UClass_UCustomLiveLinkSourceFactory();
	AIMATIONSTUDIOCONNECTOR_API UClass* Z_Construct_UClass_UCustomLiveLinkSourceFactory_NoRegister();
	LIVELINKINTERFACE_API UClass* Z_Construct_UClass_ULiveLinkSourceFactory();
	UPackage* Z_Construct_UPackage__Script_AImationStudioConnector();
// End Cross Module References
	void UCustomLiveLinkSourceFactory::StaticRegisterNativesUCustomLiveLinkSourceFactory()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UCustomLiveLinkSourceFactory);
	UClass* Z_Construct_UClass_UCustomLiveLinkSourceFactory_NoRegister()
	{
		return UCustomLiveLinkSourceFactory::StaticClass();
	}
	struct Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_ULiveLinkSourceFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_AImationStudioConnector,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "AimationLiveLinkSourceFactory.h" },
		{ "ModuleRelativePath", "Public/AimationLiveLinkSourceFactory.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UCustomLiveLinkSourceFactory>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::ClassParams = {
		&UCustomLiveLinkSourceFactory::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x000000A0u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::Class_MetaDataParams), Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_UCustomLiveLinkSourceFactory()
	{
		if (!Z_Registration_Info_UClass_UCustomLiveLinkSourceFactory.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UCustomLiveLinkSourceFactory.OuterSingleton, Z_Construct_UClass_UCustomLiveLinkSourceFactory_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UCustomLiveLinkSourceFactory.OuterSingleton;
	}
	template<> AIMATIONSTUDIOCONNECTOR_API UClass* StaticClass<UCustomLiveLinkSourceFactory>()
	{
		return UCustomLiveLinkSourceFactory::StaticClass();
	}
	UCustomLiveLinkSourceFactory::UCustomLiveLinkSourceFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UCustomLiveLinkSourceFactory);
	UCustomLiveLinkSourceFactory::~UCustomLiveLinkSourceFactory() {}
	struct Z_CompiledInDeferFile_FID_AImationUEPluginProj_Plugins_AImationStudioConnector_Source_AImationStudioConnector_Public_AimationLiveLinkSourceFactory_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AImationUEPluginProj_Plugins_AImationStudioConnector_Source_AImationStudioConnector_Public_AimationLiveLinkSourceFactory_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UCustomLiveLinkSourceFactory, UCustomLiveLinkSourceFactory::StaticClass, TEXT("UCustomLiveLinkSourceFactory"), &Z_Registration_Info_UClass_UCustomLiveLinkSourceFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UCustomLiveLinkSourceFactory), 4028728882U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AImationUEPluginProj_Plugins_AImationStudioConnector_Source_AImationStudioConnector_Public_AimationLiveLinkSourceFactory_h_1147381290(TEXT("/Script/AImationStudioConnector"),
		Z_CompiledInDeferFile_FID_AImationUEPluginProj_Plugins_AImationStudioConnector_Source_AImationStudioConnector_Public_AimationLiveLinkSourceFactory_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AImationUEPluginProj_Plugins_AImationStudioConnector_Source_AImationStudioConnector_Public_AimationLiveLinkSourceFactory_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
