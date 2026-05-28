// Copyright. All Rights Reserved.

using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class AjsfxMetaSoundAuditioner : ModuleRules
	{
		public AjsfxMetaSoundAuditioner(ReadOnlyTargetRules Target) : base(Target)
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
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"ToolMenus",
				"WorkspaceMenuStructure",
				"InputCore",
				"PropertyEditor",
				"EditorWidgets",
				"ContentBrowser",
				"AssetRegistry",
				"AudioMixer",
				"AudioExtensions",
				"MetasoundEngine",
				"MetasoundFrontend",
				"MetasoundGraphCore",
			});
		}
	}
}
