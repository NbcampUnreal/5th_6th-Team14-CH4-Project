// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project_14 : ModuleRules
{
	public Project_14(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			//Default
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			
			//UI
			"UMG","Slate","SlateCore",
			
			//Chating Plugin module
			"SocketIOClient",
			"SIOJson",

			//Niagara
			"Niagara",          
            "NiagaraCore",
            
            //HTTP protocol
            "HTTP",
            "HttpServer",
            
            //JSon
            "Json",
            "JsonUtilities", "HttpServer",
        });
	}
}
