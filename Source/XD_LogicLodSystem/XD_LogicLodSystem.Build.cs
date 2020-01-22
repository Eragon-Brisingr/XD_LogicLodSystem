// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class XD_LogicLodSystem : ModuleRules
{
	public XD_LogicLodSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
        

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Navmesh",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",

                "AIModule",
                "GameplayTasks",
                "GameplayTags",
                "NavigationSystem",

                "XD_SaveGameSystem",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}