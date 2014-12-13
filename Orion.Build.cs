// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Orion : ModuleRules
{
    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../External/")); }
    }

	public Orion(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "ShaderCore", "Slate", "SlateCore", "UMG", "Sockets", "Networking" });

        LoadPlayFab(Target);
	}

    public bool LoadPlayFab(TargetInfo Target)
    {
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
           // string LibrariesPath = Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK");
           // LibrariesPath = Path.Combine(LibrariesPath, "lib", "x64");

            if (Target.Type != TargetRules.TargetType.Server)
            {
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "lib", "x64", "PlayFabClientAPI.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "lib", "x64", "libcurl.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "lib", "x64", "libeay32.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "lib", "x64", "ssleay32.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "lib", "x64", "zlib.lib"));
                PublicAdditionalLibraries.Add("Wldap32.Lib");

                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "include"));
                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "include"));

                Definitions.Add("IS_SERVER=0");
            }
            else
            {
                //only the server needs access to these
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "lib", "x64", "PlayFabServerAPI.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "lib", "x64", "libcurl.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "lib", "x64", "libeay32.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "lib", "x64", "ssleay32.lib"));
                PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "lib", "x64", "zlib.lib"));

                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "include"));
                PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "include"));

                Definitions.Add("IS_SERVER=1");
            }

            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Shared"));
        }

        Definitions.Add(string.Format("WITH_PLAYFAB_BINDING={0}", 1));

        //boost asio
      //  PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Boost", "stage", "lib", "PlayFabClientAPI.lib"));

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Boost"));
        return true;
    }
}
