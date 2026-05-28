// Copyright. All Rights Reserved.

using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class AjsfxMetaSoundAuditionerTests : ModuleRules
	{
		public AjsfxMetaSoundAuditionerTests(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			});

			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"AjsfxMetaSoundAuditioner",
			});

			PrivateIncludePaths.Add(System.IO.Path.Combine(ModuleDirectory, "..", "AjsfxMetaSoundAuditioner", "Private"));
		}
	}
}
