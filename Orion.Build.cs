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
       // Definitions.Add("ORIONONLINESUBSYSTEMPLAYFAB_PACKAGE=1");
        if (Target.Type == TargetRules.TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "OnlineSubsystem", "ShaderCore", "Slate", "SlateCore", 
                "UMG", "Sockets", "Networking", "Json", "JsonUtilities", /*"OrionVoxel", */"UnrealEd", "ProceduralMeshComponent", "PlayFab", "PhotonClient", "Steamworks","OnlineSubsystemSteam", "Landscape",
                "OnlineSubsystemUtils", "MoviePlayer", "EngineSettings"
            });

           // DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        }
        else
        {
            PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "InputCore", /*"RHI", "RenderCore",*/ "OnlineSubsystem", "ShaderCore", /*"Slate", "SlateCore", */
             "UMG", /*"Sockets", "Networking",*/ "Json", "JsonUtilities", /*"OrionVoxel",*//* "UnrealEd",*/ "ProceduralMeshComponent", "PlayFab", "PhotonClient","Steamworks", "OnlineSubsystemSteam", "Landscape",
             "PlayFab", "OnlineSubsystemUtils", "MoviePlayer", "EngineSettings"
            });

          //  DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        }
        LoadPlayFab(Target);

        if (Target.Platform == UnrealTargetPlatform.Win32)
            Definitions.Add("IS_WIN32=1");
        else
            Definitions.Add("IS_WIN32=0");
	}

    public bool LoadPlayFab(TargetInfo Target)
    {
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {

            if (Target.Type != TargetRules.TargetType.Server)
            {
                //PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "lib", "x64", "PlayFabClientAPI.lib"));

               //PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "include"));
               //PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabClientSDK", "dependencies", "include"));

                Definitions.Add("IS_SERVER=0");

                //photon stuff
                //LoadPhoton(Target);
            }
            else
            {
                //only the server needs access to these
                //PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "lib", "x64", "PlayFabServerAPI.lib"));

                //PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "include"));
                //PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "PlayFabSDK", "PlayFabServerSDK", "dependencies", "include"));

                Definitions.Add("IS_SERVER=1");
            }

            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Shared"));
        }

        Definitions.Add(string.Format("WITH_PLAYFAB_BINDING={0}", 1));

        //boost asio
      //  PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "Boost", "stage", "lib", "PlayFabClientAPI.lib"));

       // PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "Boost"));
        return true;
    }

    private string PhotonPath
    {
        get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "Photon")); }
    }

    private void AddPhotonLibPathWin(TargetInfo Target, string name)
    {
        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "Win32";
        PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Windows", name + "-cpp_vc12_release_windows_md_" + PlatformString + ".lib"));
    }

    private void AddPhotonLibPathAndroid(TargetInfo Target, string name)
    {
        PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Android", "lib" + name + "-cpp-static_debug_android_armeabi_no-rtti.a"));
    }

    private void AddPhotonLibPathIOS(TargetInfo Target, string name)
    {
        string archStr = (Target.Architecture == "-simulator") ? "iphonesimulator" : "iphoneos";
        PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "iOS", "lib" + name + "-cpp_debug_" + archStr + ".a"));
    }

    private void AddPhotonLibPathMac(TargetInfo Target, string name)
    {
        PublicAdditionalLibraries.Add(Path.Combine(PhotonPath, "lib", "Mac", "lib" + name + "-cpp_debug_macosx.a"));
    }

    public bool LoadPhoton(TargetInfo Target)
    {
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            Definitions.Add("_EG_WINDOWS_PLATFORM");
            AddPhotonLibPathWin(Target, "Common");
            AddPhotonLibPathWin(Target, "Photon");
            AddPhotonLibPathWin(Target, "LoadBalancing");
            AddPhotonLibPathWin(Target, "Chat");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            Definitions.Add("_EG_ANDROID_PLATFORM");
            AddPhotonLibPathAndroid(Target, "common");
            AddPhotonLibPathAndroid(Target, "photon");
            AddPhotonLibPathAndroid(Target, "loadbalancing");
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            Definitions.Add("_EG_IPHONE_PLATFORM");
            AddPhotonLibPathIOS(Target, "Common");
            AddPhotonLibPathIOS(Target, "Photon");
            AddPhotonLibPathIOS(Target, "LoadBalancing");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            Definitions.Add("_EG_IMAC_PLATFORM");
            AddPhotonLibPathMac(Target, "Common");
            AddPhotonLibPathMac(Target, "Photon");
            AddPhotonLibPathMac(Target, "LoadBalancing");
        }
        else
        {
            //throw new Exception("\nTarget platform not supported: " + Target.Platform);
        }

        // Include path
        PublicIncludePaths.Add(PhotonPath);

        return true;
    }
}
