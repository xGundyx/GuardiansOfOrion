// Fill out your copyright notice in the Description page of Project Settings.

#include "Orion.h"
#include "OrionAchievements.h"

AOrionAchievements::AOrionAchievements(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AOrionAchievements::Init()
{
	Achievements.Empty();

	//level based unlocks
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROGRESS"), TEXT("progress"), TEXT("REACH LEVEL 3 WITH ANY CHARACTER"), ACH_REACHLEVELTHREE, -1, 1, NULL, TEXT("HARD MODE UNLOCKED!"), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("GETTING THE HANG OF THINGS"), TEXT("gettingthehangofthings"), TEXT("REACH LEVEL 10 WITH ANY CHARACTER"), ACH_REACHLEVELTEN, -1, 1, NULL, TEXT("INSANE MODE UNLOCKED!"), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("NOOB NO LONGER"), TEXT("noobnolonger"), TEXT("REACH LEVEL 20 WITH ANY CHARACTER"), ACH_REACHLEVELTWENTY, -1, 1, NULL, TEXT("REDIKULOUS MODE UNLOCKED!"), 250));

	//levels
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("FLYING HIGH I"), TEXT("flyinghighone"), TEXT("REACH LEVEL 10 WITH ASSAULT"), ACH_ASSAULTLEVEL10, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("FLYING HIGH II"), TEXT("flyinghightwo"), TEXT("REACH LEVEL 20 WITH ASSAULT"), ACH_ASSAULTLEVEL20, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("FLYING HIGH III"), TEXT("flyinghighthree"), TEXT("REACH LEVEL 30 WITH ASSAULT"), ACH_ASSAULTLEVEL30, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("FLYING HIGH IV"), TEXT("flyinghighfour"), TEXT("REACH LEVEL 40 WITH ASSAULT"), ACH_ASSAULTLEVEL40, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("FLYING HIGH V"), TEXT("flyinghighfive"), TEXT("REACH LEVEL 50 WITH ASSAULT"), ACH_ASSAULTLEVEL50, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROTECTOR I"), TEXT("protectorone"), TEXT("REACH LEVEL 10 WITH SUPPORT"), ACH_SUPPORTLEVEL10, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROTECTOR II"), TEXT("protectotwo"), TEXT("REACH LEVEL 20 WITH SUPPORT"), ACH_SUPPORTLEVEL20, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROTECTOR III"), TEXT("protectothree"), TEXT("REACH LEVEL 30 WITH SUPPORT"), ACH_SUPPORTLEVEL30, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROTECTOR IV"), TEXT("protectofour"), TEXT("REACH LEVEL 40 WITH SUPPORT"), ACH_SUPPORTLEVEL40, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("PROTECTOR V"), TEXT("protectofive"), TEXT("REACH LEVEL 50 WITH SUPPORT"), ACH_SUPPORTLEVEL50, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("STEALTH I"), TEXT("stealthone"), TEXT("REACH LEVEL 10 WITH RECON"), ACH_RECONLEVEL10, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("STEALTH II"), TEXT("stealthtwo"), TEXT("REACH LEVEL 20 WITH RECON"), ACH_RECONLEVEL20, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("STEALTH III"), TEXT("stealththree"), TEXT("REACH LEVEL 30 WITH RECON"), ACH_RECONLEVEL30, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("STEALTH IV"), TEXT("stealthfour"), TEXT("REACH LEVEL 40 WITH RECON"), ACH_RECONLEVEL40, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_CHARACTERS, TEXT("STEALTH V"), TEXT("stealthfive"), TEXT("REACH LEVEL 50 WITH RECON"), ACH_RECONLEVEL50, -1, 1, NULL, TEXT(""), 250));
	
	//combat
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RAPTOR MASTER I"), TEXT("raptormasterone"), TEXT("KILL 100 RAPTORS"), ACH_RAPTORSKILLEDI, STAT_RAPTORKILL, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RAPTOR MASTER II"), TEXT("raptormastertwo"), TEXT("KILL 500 RAPTORS"), ACH_RAPTORSKILLEDII, STAT_RAPTORKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RAPTOR MASTER III"), TEXT("raptormasterthree"), TEXT("KILL 1000 RAPTORS"), ACH_RAPTORSKILLEDIII, STAT_RAPTORKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RAPTOR MASTER IV"), TEXT("raptormasterfour"), TEXT("KILL 5000 RAPTORS"), ACH_RAPTORSKILLEDIV, STAT_RAPTORKILL, 5000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RAPTOR MASTER V"), TEXT("raptormasterfive"), TEXT("KILL 10000 RAPTORS"), ACH_RAPTORSKILLEDV, STAT_RAPTORKILL, 10000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("COMPY MASTER I"), TEXT("compymasterone"), TEXT("KILL 100 COMPYS"), ACH_COMPYSKILLEDI, STAT_COMPYKILL, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("COMPY MASTER II"), TEXT("compymastertwo"), TEXT("KILL 500 COMPYS"), ACH_COMPYSKILLEDII, STAT_COMPYKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("COMPY MASTER III"), TEXT("compymasterthree"), TEXT("KILL 1000 COMPYS"), ACH_COMPYSKILLEDIII, STAT_COMPYKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("COMPY MASTER IV"), TEXT("compymasterfour"), TEXT("KILL 5000 COMPYS"), ACH_COMPYSKILLEDIV, STAT_COMPYKILL, 5000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("COMPY MASTER V"), TEXT("compymasterfive"), TEXT("KILL 10000 COMPYS"), ACH_COMPYSKILLEDV, STAT_COMPYKILL, 10000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("DILO MASTER I"), TEXT("dilomasterone"), TEXT("KILL 50 DILOS"), ACH_DILOSKILLEDI, STAT_DILOKILL, 50, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("DILO MASTER II"), TEXT("dilomastertwo"), TEXT("KILL 250 DILOS"), ACH_DILOSKILLEDII, STAT_DILOKILL, 250, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("DILO MASTER III"), TEXT("dilomasterthree"), TEXT("KILL 500 DILOS"), ACH_DILOSKILLEDIII, STAT_DILOKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("DILO MASTER IV"), TEXT("dilomasterfour"), TEXT("KILL 1000 DILOS"), ACH_DILOSKILLEDIV, STAT_DILOKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("DILO MASTER V"), TEXT("dilomasterfive"), TEXT("KILL 2000 DILOS"), ACH_DILOSKILLEDV, STAT_DILOKILL, 2000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RHAM MASTER I"), TEXT("rhammasterone"), TEXT("KILL 50 RHAMS"), ACH_RHAMSKILLEDI, STAT_RHAMKILL, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RHAM MASTER II"), TEXT("rhammastertwo"), TEXT("KILL 250 RHAMS"), ACH_RHAMSKILLEDII, STAT_RHAMKILL, 250, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RHAM MASTER III"), TEXT("rhammasterthree"), TEXT("KILL 1000 RHAMS"), ACH_RHAMSKILLEDIII, STAT_RHAMKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RHAM MASTER IV"), TEXT("rhammasterfour"), TEXT("KILL 2500 RHAMS"), ACH_RHAMSKILLEDIV, STAT_RHAMKILL, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("RHAM MASTER V"), TEXT("rhammasterfive"), TEXT("KILL 5000 RHAMS"), ACH_RHAMSKILLEDV, STAT_RHAMKILL, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ANKY MASTER I"), TEXT("ankymasterone"), TEXT("KILL 5 ANKYS"), ACH_ANKYSKILLEDI, STAT_ANKYKILL, 5, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ANKY MASTER II"), TEXT("ankymastertwo"), TEXT("KILL 25 ANKYS"), ACH_ANKYSKILLEDII, STAT_ANKYKILL, 25, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ANKY MASTER III"), TEXT("ankymasterthree"), TEXT("KILL 100 ANKYS"), ACH_ANKYSKILLEDIII, STAT_ANKYKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ANKY MASTER IV"), TEXT("ankymasterfour"), TEXT("KILL 250 ANKYS"), ACH_ANKYSKILLEDIV, STAT_ANKYKILL, 250, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ANKY MASTER V"), TEXT("ankymasterfive"), TEXT("KILL 500 ANKYS"), ACH_ANKYSKILLEDV, STAT_ANKYKILL, 500, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TREX MASTER I"), TEXT("trexmasterone"), TEXT("KILL 1 TREX"), ACH_TREXKILLEDI, STAT_TREXKILL, 1, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TREX MASTER II"), TEXT("trexmastertwo"), TEXT("KILL 10 TREX"), ACH_TREXKILLEDII, STAT_TREXKILL, 10, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TREX MASTER III"), TEXT("trexmasterthree"), TEXT("KILL 50 TREX"), ACH_TREXKILLEDIII, STAT_TREXKILL, 50, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TREX MASTER IV"), TEXT("trexmasterfour"), TEXT("KILL 100 TREX"), ACH_TREXKILLEDIV, STAT_TREXKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TREX MASTER V"), TEXT("trexmasterfive"), TEXT("KILL 250 TREX"), ACH_TREXKILLEDV, STAT_TREXKILL, 250, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TRICERATOPS MASTER I"), TEXT("triceratopsmasterone"), TEXT("KILL 5 TRICERATOPS"), ACH_TRIKESKILLEDI, STAT_TRIKEKILL, 5, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TRICERATOPS MASTER II"), TEXT("triceratopsmastertwo"), TEXT("KILL 25 TRICERATOPS"), ACH_TRIKESKILLEDII, STAT_TRIKEKILL, 25, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TRICERATOPS MASTER III"), TEXT("triceratopsmasterthree"), TEXT("KILL 100 TRICERATOPS"), ACH_TRIKESKILLEDIII, STAT_TRIKEKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TRICERATOPS MASTER IV"), TEXT("triceratopsmasterfour"), TEXT("KILL 250 TRICERATOPS"), ACH_TRIKESKILLEDIV, STAT_TRIKEKILL, 250, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("TRICERATOPS MASTER V"), TEXT("triceratopsmasterfive"), TEXT("KILL 500 TRICERATOPS"), ACH_TRIKESKILLEDV, STAT_TRIKEKILL, 500, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("KRUGER MASTER I"), TEXT("krutermasterone"), TEXT("KILL 25 KRUGERS"), ACH_KRUGERSKILLEDI, STAT_KRUGERKILL, 25, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("KRUGER MASTER II"), TEXT("krugermastertwo"), TEXT("KILL 100 KRUGERS"), ACH_KRUGERSKILLEDII, STAT_KRUGERKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("KRUGER MASTER III"), TEXT("krugermasterthree"), TEXT("KILL 400 KRUGERS"), ACH_KRUGERSKILLEDIII, STAT_KRUGERKILL, 400, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("KRUGER MASTER IV"), TEXT("krugermasterfour"), TEXT("KILL 1000 KRUGERS"), ACH_KRUGERSKILLEDIV, STAT_KRUGERKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("KRUGER MASTER V"), TEXT("krugermasterfive"), TEXT("KILL 2000 KRUGERS"), ACH_KRUGERSKILLEDV, STAT_KRUGERKILL, 2000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("JECKYL MASTER I"), TEXT("jeckylmasterone"), TEXT("KILL 25 JECKYLS"), ACH_JECKYLSKILLEDI, STAT_JECKYLKILL, 25, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("JECKYL MASTER II"), TEXT("jeckylmastertwo"), TEXT("KILL 100 JECKYLS"), ACH_JECKYLSKILLEDII, STAT_JECKYLKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("JECKYL MASTER III"), TEXT("jeckylmasterthree"), TEXT("KILL 400 JECKYLS"), ACH_JECKYLSKILLEDIII, STAT_JECKYLKILL, 400, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("JECKYL MASTER IV"), TEXT("jeckylmasterfour"), TEXT("KILL 1000 JECKYLS"), ACH_JECKYLSKILLEDIV, STAT_JECKYLKILL, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("JECKYL MASTER V"), TEXT("jeckylmasterfive"), TEXT("KILL 2000 JECKYLS"), ACH_JECKYLSKILLEDV, STAT_JECKYLKILL, 2000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("NAMOR MASTER I"), TEXT("namormasterone"), TEXT("KILL 1 NAMORS"), ACH_NAMORSKILLEDI, STAT_NAMORKILL, 1, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("NAMOR MASTER II"), TEXT("namormastertwo"), TEXT("KILL 10 NAMORS"), ACH_NAMORSKILLEDII, STAT_NAMORKILL, 10, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("NAMOR MASTER III"), TEXT("namormasterthree"), TEXT("KILL 50 NAMORS"), ACH_NAMORSKILLEDIII, STAT_NAMORKILL, 50, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("NAMOR MASTER IV"), TEXT("namormasterfour"), TEXT("KILL 100 NAMORS"), ACH_NAMORSKILLEDIV, STAT_NAMORKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("NAMOR MASTER V"), TEXT("namormasterfive"), TEXT("KILL 200 NAMORS"), ACH_NAMORSKILLEDV, STAT_NAMORKILL, 200, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ROBOT ORB MASTER I"), TEXT("robotorbmasterone"), TEXT("KILL 10 ROBOT ORBS"), ACH_ROBOTORBSKILLEDI, STAT_ORBKILL, 10, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ROBOT ORB MASTER II"), TEXT("robotorbmastertwo"), TEXT("KILL 50 ROBOT ORBS"), ACH_ROBOTORBSKILLEDII, STAT_ORBKILL, 00, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ROBOT ORB MASTER III"), TEXT("robotorbmasterthree"), TEXT("KILL 100 ROBOT ORBS"), ACH_ROBOTORBSKILLEDIII, STAT_ORBKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ROBOT ORB MASTER IV"), TEXT("robotorbmasterfour"), TEXT("KILL 500 ROBOT ORBS"), ACH_ROBOTORBSKILLEDIV, STAT_ORBKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("ROBOT ORB MASTER V"), TEXT("robotorbmasterfive"), TEXT("KILL 1000 ROBOT ORBS"), ACH_ROBOTORBSKILLEDV, STAT_ORBKILL, 1000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("BONES MASTER I"), TEXT("bonesmasterone"), TEXT("KILL 10 BONES"), ACH_BONESSKILLEDI, STAT_BONESKILL, 10, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("BONES MASTER II"), TEXT("bonesmastertwo"), TEXT("KILL 50 BONES"), ACH_BONESSKILLEDII, STAT_BONESKILL, 50, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("BONES MASTER III"), TEXT("bonesmasterthree"), TEXT("KILL 100 BONES"), ACH_BONESSKILLEDIII, STAT_BONESKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("BONES MASTER IV"), TEXT("bonesmasterfour"), TEXT("KILL 500 BONES"), ACH_BONESSKILLEDIV, STAT_BONESKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("BONES MASTER V"), TEXT("bonesmasterfive"), TEXT("KILL 1000 BONES"), ACH_BONESSKILLEDV, STAT_BONESKILL, 1000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("GRUMPS MASTER I"), TEXT("grumpsmasterone"), TEXT("KILL 10 GRUMPS"), ACH_GRUMPSSKILLEDI, STAT_GRUMPSKILL, 10, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("GRUMPS MASTER II"), TEXT("grumpsmastertwo"), TEXT("KILL 50 GRUMPS"), ACH_GRUMPSSKILLEDII, STAT_GRUMPSKILL, 50, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("GRUMPS MASTER III"), TEXT("grumpsmasterthree"), TEXT("KILL 100 GRUMPS"), ACH_GRUMPSSKILLEDIII, STAT_GRUMPSKILL, 100, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("GRUMPS MASTER IV"), TEXT("grumpsmasterfour"), TEXT("KILL 500 GRUMPS"), ACH_GRUMPSSKILLEDIV, STAT_GRUMPSKILL, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_COMBAT, TEXT("GRUMPS MASTER V"), TEXT("grumpsmasterfive"), TEXT("KILL 1000 GRUMPS"), ACH_GRUMPSSKILLEDV, STAT_GRUMPSKILL, 1000, NULL, TEXT(""), 250));

	//weapon/equipment/gear
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-RIFLE MASTER I"), TEXT("autoriflemasterone"), TEXT("KILL 100 ENEMIES WITH THE AUTO-RIFLE"), ACH_AUTORIFLEKILLSI, STAT_AUTORIFLEKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-RIFLE MASTER II"), TEXT("autoriflemastertwo"), TEXT("KILL 500 ENEMIES WITH THE AUTO-RIFLE"), ACH_AUTORIFLEKILLSI, STAT_AUTORIFLEKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-RIFLE MASTER III"), TEXT("autoriflemasterthree"), TEXT("KILL 1000 ENEMIES WITH THE AUTO-RIFLE"), ACH_AUTORIFLEKILLSI, STAT_AUTORIFLEKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-RIFLE MASTER IV"), TEXT("autoriflemasterfour"), TEXT("KILL 2500 ENEMIES WITH THE AUTO-RIFLE"), ACH_AUTORIFLEKILLSI, STAT_AUTORIFLEKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-RIFLE MASTER V"), TEXT("autoriflemasterfive"), TEXT("KILL 5000 ENEMIES WITH THE AUTO-RIFLE"), ACH_AUTORIFLEKILLSI, STAT_AUTORIFLEKILLS, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("PISTOL MASTER I"), TEXT("pistolmasterone"), TEXT("KILL 100 ENEMIES WITH THE PISTOL"), ACH_PISTOLKILLSI, STAT_PISTOLKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("PISTOL MASTER II"), TEXT("pistolmastertwo"), TEXT("KILL 500 ENEMIES WITH THE PISTOL"), ACH_PISTOLKILLSI, STAT_PISTOLKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("PISTOL MASTER III"), TEXT("pistolmasterthree"), TEXT("KILL 1000 ENEMIES WITH THE PISTOL"), ACH_PISTOLKILLSI, STAT_PISTOLKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("PISTOL MASTER IV"), TEXT("pistolmasterfour"), TEXT("KILL 2500 ENEMIES WITH THE PISTOL"), ACH_PISTOLKILLSI, STAT_PISTOLKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("PISTOL MASTER V"), TEXT("pistolmasterfive"), TEXT("KILL 5000 ENEMIES WITH THE PISTOL"), ACH_PISTOLKILLSI, STAT_PISTOLKILLS, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("COMBAT-SHOTGUN MASTER I"), TEXT("combatshotgunmasterone"), TEXT("KILL 100 ENEMIES WITH THE COMBAT-SHOTGUN"), ACH_COMBATSHOTGUNKILLSI, STAT_COMBATSHOTGUNKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("COMBAT-SHOTGUN MASTER II"), TEXT("combatshotgunmastertwo"), TEXT("KILL 500 ENEMIES WITH THE COMBAT-SHOTGUN"), ACH_COMBATSHOTGUNKILLSI, STAT_COMBATSHOTGUNKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("COMBAT-SHOTGUN MASTER III"), TEXT("combatshotgunmasterthree"), TEXT("KILL 1000 ENEMIES WITH THE COMBAT-SHOTGUN"), ACH_COMBATSHOTGUNKILLSI, STAT_COMBATSHOTGUNKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("COMBAT-SHOTGUN MASTER IV"), TEXT("combatshotgunmasterfour"), TEXT("KILL 2500 ENEMIES WITH THE COMBAT-SHOTGUN"), ACH_COMBATSHOTGUNKILLSI, STAT_COMBATSHOTGUNKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("COMBAT-SHOTGUN MASTER V"), TEXT("combatshotgunmasterfive"), TEXT("KILL 5000 ENEMIES WITH THE COMBAT-SHOTGUN"), ACH_COMBATSHOTGUNKILLSI, STAT_COMBATSHOTGUNKILLS, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("MAGNUM MASTER I"), TEXT("magnummasterone"), TEXT("KILL 100 ENEMIES WITH THE MAGNUM"), ACH_MAGNUMKILLSI, STAT_MAGNUMKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("MAGNUM MASTER II"), TEXT("magnummastertwo"), TEXT("KILL 500 ENEMIES WITH THE MAGNUM"), ACH_MAGNUMKILLSI, STAT_MAGNUMKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("MAGNUM MASTER III"), TEXT("magnummasterthree"), TEXT("KILL 1000 ENEMIES WITH THE MAGNUM"), ACH_MAGNUMKILLSI, STAT_MAGNUMKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("MAGNUM MASTER IV"), TEXT("magnummasterfour"), TEXT("KILL 2500 ENEMIES WITH THE MAGNUM"), ACH_MAGNUMKILLSI, STAT_MAGNUMKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("MAGNUM MASTER V"), TEXT("magnummasterfive"), TEXT("KILL 5000 ENEMIES WITH THE MAGNUM"), ACH_MAGNUMKILLSI, STAT_MAGNUMKILLS, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("SILENCED SMG MASTER I"), TEXT("silencedsmgmasterone"), TEXT("KILL 100 ENEMIES WITH THE SILENCED SMG"), ACH_SILENCEDSMGKILLSI, STAT_SILENCEDSMGKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("SILENCED SMG MASTER II"), TEXT("silencedsmgmastertwo"), TEXT("KILL 500 ENEMIES WITH THE SILENCED SMG"), ACH_SILENCEDSMGKILLSI, STAT_SILENCEDSMGKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("SILENCED SMG MASTER III"), TEXT("silencedsmgmasterthree"), TEXT("KILL 1000 ENEMIES WITH THE SILENCED SMG"), ACH_SILENCEDSMGKILLSI, STAT_SILENCEDSMGKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("SILENCED SMG MASTER IV"), TEXT("silencedsmgmasterfour"), TEXT("KILL 2500 ENEMIES WITH THE SILENCED SMG"), ACH_SILENCEDSMGKILLSI, STAT_SILENCEDSMGKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("SILENCED SMG MASTER V"), TEXT("silencedsmgmasterfive"), TEXT("KILL 5000 ENEMIES WITH THE SILENCED SMG"), ACH_SILENCEDSMGKILLSI, STAT_SILENCEDSMGKILLS, 5000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-PISTOL MASTER I"), TEXT("autopistolmasterone"), TEXT("KILL 100 ENEMIES WITH THE AUTO-PISTOL"), ACH_AUTOPISTOLKILLSI, STAT_AUTOPISTOLKILLS, 100, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-PISTOL MASTER II"), TEXT("autopistolmastertwo"), TEXT("KILL 500 ENEMIES WITH THE AUTO-PISTOL"), ACH_AUTOPISTOLKILLSI, STAT_AUTOPISTOLKILLS, 500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-PISTOL MASTER III"), TEXT("autopistolmasterthree"), TEXT("KILL 1000 ENEMIES WITH THE AUTO-PISTOL"), ACH_AUTOPISTOLKILLSI, STAT_AUTOPISTOLKILLS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-PISTOL MASTER IV"), TEXT("autopistolmasterfour"), TEXT("KILL 2500 ENEMIES WITH THE AUTO-PISTOL"), ACH_AUTOPISTOLKILLSI, STAT_AUTOPISTOLKILLS, 2500, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_GEAR, TEXT("AUTO-PISTOL MASTER V"), TEXT("autopistolmasterfive"), TEXT("KILL 5000 ENEMIES WITH THE AUTO-PISTOL"), ACH_AUTOPISTOLKILLSI, STAT_AUTOPISTOLKILLS, 5000, NULL, TEXT(""), 250));

	//misc
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("ORB MASTER I"), TEXT("orbmasterone"), TEXT("COLLECT 50 POWER ORBS"), ACH_ORBMASTERI, -1, 1, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("ORB MASTER II"), TEXT("orbmastertwo"), TEXT("COLLECT 250 POWER ORBS"), ACH_ORBMASTERII, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("ORB MASTER III"), TEXT("orbmasterthree"), TEXT("COLLECT 1000 POWER ORBS"), ACH_ORBMASTERIII, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("ORB MASTER IV"), TEXT("orbmasterfour"), TEXT("COLLECT 5000 POWER ORBS"), ACH_ORBMASTERIV, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("ORB MASTER V"), TEXT("orbmasterfive"), TEXT("COLLECT 10000 POWER ORBS"), ACH_ORBMASTERV, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("WELCOME TO RELIC"), TEXT("welcometorelic"), TEXT("COMPLETE A SURVIVAL MATCH ON RELIC"), ACH_WELCOMETORELIC, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("WELCOME TO OUTBACK"), TEXT("welcometooutback"), TEXT("COMPLETE A SURVIVAL MATCH ON OUTBACK"), ACH_WELCOMETOOUTBACK, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("WELCOME TO ARID"), TEXT("welcometoarid"), TEXT("COMPLETE A SURVIVAL MATCH ON ARID"), ACH_WELCOMETOARID, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HELPING HAND I"), TEXT("helpinghandone"), TEXT("EARN 50 ASSISTS"), ACH_HELPINGHANDI, STAT_ASSISTS, 50, NULL, TEXT(""), 50));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HELPING HAND II"), TEXT("helpinghandtwo"), TEXT("EARN 200 ASSISTS"), ACH_HELPINGHANDII, STAT_ASSISTS, 200, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HELPING HAND III"), TEXT("helpinghandthree"), TEXT("EARN 1000 ASSISTS"), ACH_HELPINGHANDIII, STAT_ASSISTS, 1000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HELPING HAND IV"), TEXT("helpinghandfour"), TEXT("EARN 5000 ASSISTS"), ACH_HELPINGHANDIV, STAT_ASSISTS, 5000, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HELPING HAND V"), TEXT("helpinghandfive"), TEXT("EARN 10000 ASSISTS"), ACH_HELPINGHANDV, STAT_ASSISTS, 10000, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("HARD AS A ROCK"), TEXT("hardasarock"), TEXT("COMPLETE ANY SURVIVAL MAP ON HARD"), ACH_WINHARD, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("INSANE"), TEXT("insane"), TEXT("COMPLETE ANY SURVIVAL MAP ON INSANE"), ACH_WININSANE, -1, 1, NULL, TEXT(""), 150));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("SNACK TIME"), TEXT("snacktime"), TEXT("GET FATALITIED BY A T-REX"), ACH_TREXFATALITY, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("STICK IT TO EM"), TEXT("stickittoem"), TEXT("GET FATALITIED BY A KRUGER"), ACH_KRUGERFATALITY, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("LICKED"), TEXT("licked"), TEXT("GET FATALITIED BY A JECKYL"), ACH_JECKYLFATALITY, -1, 1, NULL, TEXT(""), 100));
	Achievements.Add(FAchievement(ACHCATEGORY_MISC, TEXT("OVERTIMER"), TEXT("overtimer"), TEXT("SUCCESSFULLY REPAIR A HARVESTOR WHEN TIME HAS RUN OUT"), ACH_OVERTIME, -1, 1, NULL, TEXT(""), 100));

	//challenges
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("FRIENDLIEST FRIEND"), TEXT("friendliestfriend"), TEXT("EARN 200 ASSISTS IN A SINGLE MATCH"), ACH_200ASSIST, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("GOTTA CATCH EM ALL"), TEXT("gottacatchemall"), TEXT("HAVE 6 POWER UP ORBS ACTIVE AT ONCE"), ACH_6ORBS, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("DINO TAMER"), TEXT("dinotamer"), TEXT("KILL A T-REX AND NAMOR WITHIN 5 SECONDS OF EACHOTHER"), ACH_TREXNAMOR, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("IRON BALLS"), TEXT("ironballs"), TEXT("SURVIVE A DIRECT GRENADE FROM A ROBOT ORB ON INSANE OR HIGHER"), ACH_IRONBALLS, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("MONSTER FRAG"), TEXT("monsterfrag"), TEXT("KILL AT LEAST 10 ENEMIES WITH SINGLE FRAG GRENADE"), ACH_MONSTERFRAG, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("INHUMANE"), TEXT("inhumane"), TEXT("BLOW ALL OF A TRICERATOP'S LEGS OFF WITH ONE ATTACK"), ACH_INHUMANE, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("UNKILLABLE"), TEXT("unkillable"), TEXT("TAKE 25,000 DAMAGE WITHOUT DYING"), ACH_UNKILLABLE, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("RELICULOUS"), TEXT("reliculous"), TEXT("WIN A REDIKULOUS SURVIVAL MATCH ON RELIC"), ACH_REDIKULOUSRELIC, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("OUTBACKULOUS"), TEXT("outbackulous"), TEXT("WIN A REDIKULOUS SURVIVAL MATCH ON OUTBACK"), ACH_REDIKULOUSOUTBACK, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("ARIDICULOUS"), TEXT("aridiculous"), TEXT("WIN A REDIKULOUS SURVIVAL MATCH ON ARID"), ACH_REDIKULOUSARID, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("MASOCHIST"), TEXT("masochist"), TEXT("WIN A REDIKULOUS SURVIVAL MATCH WITHOUT DYING AND BEING THERE FROM THE START"), ACH_MASOCHIST, -1, 1, NULL, TEXT(""), 250));
	Achievements.Add(FAchievement(ACHCATEGORY_CHALLENGES, TEXT("SLAYER"), TEXT("slayer"), TEXT("KILL 100,000 ENEMIES"), ACH_DINOSLAYER, -1, 1, NULL, TEXT(""), 250));

	bInitialized = false;
	bReplicates = true;
}

void AOrionAchievements::UnlockAchievement(int32 AchID, AOrionPlayerController *PC)
{
#if IS_SERVER
	if (!bInitialized)
		return;

	//don't unlock things we already have unlocked!
	if(Achievements[AchID].bUnlocked)
		return;

	Achievements[AchID].bUnlocked = true;

	PC->UnlockAchievement(TEXT("ACHIEVEMENT UNLOCKED!"), Achievements[AchID].Name);

	EventSavePlayerAchievements(PC);
#endif
}

void AOrionAchievements::ReadPlayerAchievements(AOrionPlayerController *PC)
{
	EventReadPlayerAchievements(PC);
}

void AOrionAchievements::SetAchievementValues(TArray<FAchievement> AchievementsRead)
{
	for (int32 i = 0; i < AchievementsRead.Num(); i++)
	{
		int32 index = Achievements.Find(AchievementsRead[i]);

		if (index != INDEX_NONE)
		{
			Achievements[index].bUnlocked = AchievementsRead[i].bUnlocked;
		}
	}

	bInitialized = true;
}

void AOrionAchievements::CheckForLevelUnlocks(int32 NewLevel, AOrionPlayerController *PC)
{
	if (!PC)
		return;

	if (!bInitialized)
		return;

	if (NewLevel >= 3 && !Achievements[ACH_REACHLEVELTHREE].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTHREE, PC);
	else if (NewLevel >= 10 && !Achievements[ACH_REACHLEVELTEN].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTEN, PC);
	else if (NewLevel >= 20 && !Achievements[ACH_REACHLEVELTWENTY].bUnlocked)
		UnlockAchievement(ACH_REACHLEVELTWENTY, PC);

	int cIndex = PC->ClassIndex;

	switch (cIndex)
	{
	case 0:
		if (NewLevel >= 10)
			UnlockAchievement(ACH_ASSAULTLEVEL10, PC);
		else if (NewLevel >= 20)
			UnlockAchievement(ACH_ASSAULTLEVEL20, PC);
		else if (NewLevel >= 30)
			UnlockAchievement(ACH_ASSAULTLEVEL30, PC);
		else if (NewLevel >= 40)
			UnlockAchievement(ACH_ASSAULTLEVEL40, PC);
		else if (NewLevel >= 50)
			UnlockAchievement(ACH_ASSAULTLEVEL50, PC);
		break;
	case 1:
		if (NewLevel >= 10)
			UnlockAchievement(ACH_SUPPORTLEVEL10, PC);
		else if (NewLevel >= 20)
			UnlockAchievement(ACH_SUPPORTLEVEL20, PC);
		else if (NewLevel >= 30)
			UnlockAchievement(ACH_SUPPORTLEVEL30, PC);
		else if (NewLevel >= 40)
			UnlockAchievement(ACH_SUPPORTLEVEL40, PC);
		else if (NewLevel >= 50)
			UnlockAchievement(ACH_SUPPORTLEVEL50, PC);
		break;
	case 2:
		if (NewLevel >= 10)
			UnlockAchievement(ACH_RECONLEVEL10, PC);
		else if (NewLevel >= 20)
			UnlockAchievement(ACH_RECONLEVEL20, PC);
		else if (NewLevel >= 30)
			UnlockAchievement(ACH_RECONLEVEL30, PC);
		else if (NewLevel >= 40)
			UnlockAchievement(ACH_RECONLEVEL40, PC);
		else if (NewLevel >= 50)
			UnlockAchievement(ACH_RECONLEVEL50, PC);
		break;
	}
}

//add a timer delay so we don't have to spam this during heavy action
void AOrionAchievements::CheckForStatUnlocks()
{
	GetWorldTimerManager().SetTimer(UnlockTimer, this, &AOrionAchievements::UpdateUnlocks, 1.0f, false);
}

void AOrionAchievements::UpdateUnlocks()
{
#if IS_SERVER
	if (!PCOwner)
		return;

	if (!PCOwner->GetStats())
		return;

	bool bUnlocked = false;

	for (int32 i = 0; i < ACH_NUM; i++)
	{
		if (Achievements[i].bUnlocked)
			continue;

		if (Achievements[i].StatID < 0)
			continue;

		if (PCOwner->GetStats()->aStats[Achievements[i].StatID] >= Achievements[i].Goal)
		{
			Achievements[i].bUnlocked = true;

			PCOwner->UnlockAchievement(TEXT("ACHIEVEMENT UNLOCKED!"), Achievements[i].Name);

			bUnlocked = true;
		}
	}

	if (bUnlocked)
		EventSavePlayerAchievements(PCOwner);
#endif
}

void AOrionAchievements::CheckOrbs()
{
	if (PCOwner && PCOwner->GetStats())
	{
		int32 TotalOrbs = PCOwner->GetStats()->aStats[STAT_HEALTHORBS].StatValue +
			PCOwner->GetStats()->aStats[STAT_STOPPINGORBS].StatValue +
			PCOwner->GetStats()->aStats[STAT_ROFORBS].StatValue +
			PCOwner->GetStats()->aStats[STAT_SPEEDORBS].StatValue +
			PCOwner->GetStats()->aStats[STAT_REDUCTIONORBS].StatValue +
			PCOwner->GetStats()->aStats[STAT_EXPORBS].StatValue;

		if (TotalOrbs >= 10000)
			UnlockAchievement(ACH_ORBMASTERV, PCOwner);
		else if (TotalOrbs >= 5000)
			UnlockAchievement(ACH_ORBMASTERIV, PCOwner);
		else if (TotalOrbs >= 1000)
			UnlockAchievement(ACH_ORBMASTERIII, PCOwner);
		else if (TotalOrbs >= 250)
			UnlockAchievement(ACH_ORBMASTERII, PCOwner);
		else if (TotalOrbs >= 50)
			UnlockAchievement(ACH_ORBMASTERI, PCOwner);
	}
}

void AOrionAchievements::CheckForSlayer()
{
	if (PCOwner && PCOwner->GetStats())
	{
		int32 TotalKills = PCOwner->GetStats()->aStats[STAT_RAPTORKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_COMPYKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_DILOKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_RHAMKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_TREXKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_TRIKEKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_ANKYKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_KRUGERKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_JECKYLKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_NAMORKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_ORBKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_BONESKILL].StatValue +
			PCOwner->GetStats()->aStats[STAT_GRUMPSKILL].StatValue;

		if (TotalKills >= 100000)
			UnlockAchievement(ACH_DINOSLAYER, PCOwner);
	}
}


