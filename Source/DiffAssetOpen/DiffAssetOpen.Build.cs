namespace UnrealBuildTool.Rules
{
	public class DiffAssetOpen : ModuleRules
	{
		public DiffAssetOpen(ReadOnlyTargetRules Target) : base(Target)
		{
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
            PublicIncludePaths.AddRange(
				new string[] {
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[] {
				}
				);
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"UnrealEd",
					"LevelEditor",
					"Slate",
					"SlateCore",
					"MainFrame",
					"InputCore",
					"CoreUObject",
					"AssetTools",
					"EditorStyle",
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
				}
				);
		}
	}
}
