### Super Mario War - Steam Link Port

This port is for running Super Mario War on the Steam Link, because the Steam Link is an ARMv7 device we need to use the SDK provided by valve at `https://github.com/ValveSoftware/steamlink-sdk.git`

I have not changed much, only setting it to default to 1920x1080 resolution and trying to get it to default to  gamepad only.
Windows/XBOX etc.. stuff will be cleared out eventually because they will never apply to this port.

I'm assuming the level-editor will not work without more modifications, and have so far only tested local gameplay.
Disconnecting Gamepads during play will mean the app needs to be restarted, this seriously needs looking at!

## Build Super Mario War

This repository contains some submodules which you can use if the dependencies are not available for your OS, are outdated or you simply don't want to install them on your system. To use the included libraries, do a recursive cloning:
As long as you clone this repo into the examples subfolder of the steamlink-sdk it should compile with no problems.

`git clone https://github.com/ValveSoftware/steamlink-sdk.git`

`cd steamlink-sdk/examples`

`git clone --recursive https://github.com/thecosmicslug/SuperMarioWar-SteamLink.git`

`cd SuperMarioWar-SteamLink`

`./build_smw-steamlink.sh`

binarys will be placed in `build` directory. To Install an application on the Steamlink is best described on the SDK, but basically you need a directory called 'supermariowar' or such, containing the binary, the data directory, an icon for example icon.png, and a text file called toc.txt in the following format:

name=Super Mario War
icon=icon.png 
run=smw

This file is simply for the steam link launcher, once you have all the files together in your 'supermariowar' directory then you need to compress the whole folder inside a tar.gz file to be placed on a FAT32 USB stick in the following location : `/steamlink/apps/`.
SSH access can be enabled on the link at the same time, a textfile should be created  at `/steamlink/config/system/enable_ssh.txt`

Power on the steamlink with the usb stick plugged in for the initial install along with enabling SSH until a factory reset is performed on the device.

the user/pass is default to
`user=root` and 
`password=steamlink123`, although this can be changed later with the passwd command.
 
Once installed it is much easier to update an application via SSH at the following location on the steamlink:
`/home/apps/<directory-from-tar.gz>`

## History

Super Mario War is a fan-made multiplayer Super Mario Bros. style deathmatch game in which players try to beat one another in a variety of gameplay modes. You can play on teams, design your own levels, design your own worlds, and much more!

The original *Mario War* was created by *Samuele Poletto* in 2002, in which four Super Mario characters could fight on various levels by jumping on each other's head. It was written in Pascal/Assembly, and was released for DOS. Later versions also included a map editor.

In 2004, *Florian Hufsky*, founder of the *72dpiarmy* forum started working on an open-source rewrite, which became *Super Mario War*. This version introduced custom characters, additional gameplay modes and map mechanics, items and powerups. Custom user contents were stored on (the now defunct) `smwstuff.com` site, with thousands of maps and skins available. Due to its open-source nature, the game has been ported to pretty much every desktop and console system.

At the end of 2009, Florian died. The development of the game slowed down and eventually stopped, with SMW 1.8 beta 2 being the last official release. While there have been several attempts for continuing or restarting the development, none of them seem to have succeeded in the long term. In addition, due to technical issues the whole `smwstuff.com` site and all of its contents has also been lost, with partial backups from the uploaders available on the forum.

This is a fork I've started working on around 2014, with the initial goal of implementing network multiplayer support that would work cross-platform between different devices. I've also wanted to make a site for collecting the scattered content from the forum in one place once again. This became [`smwstuff.net`](http://smwstuff.net).

As for the game, unfortunately it became clear very soon that the quality and structure of the original source code will make effective work impossible: most source files were in the "around 10000 lines" category, with 1000+ LOC functions being common, most of them modifying global variables, and there was even an `if-else` with a hundred branches.

Eventually I've managed to make the network multiplayer working, but it's far from perfect. With good conditions, on local networks it may work fine, but subtle bugs and lag usually makes the gameplay over the internet far from optimal. A proper implementation would likely require a redesign of several core parts of the game and another several hundred clean up/refactoring patches to make the code maintainable. And, with this kind of project of course, there's also a chance that next morning you get a cease-and-desist letter from whatever company.

For me, this was the point where I've stopped putting more time on this project (2016). In the long term, a complete rewrite might be faster and more effective than patching the original game for years. Either way, I hope I've left the project in a better state than it was, and that it will be of use for future developers. Have fun!

## About

Super Mario War is a Super Mario multiplayer game. The goal is to stomp as many other Mario's as possible to win the game. There is a range of different gameplay modes in the game, like Capture-The-Flag, King of The Hill, Deathmatch, Team Deathmatch, Tournament Mode, Collect The Coins, Race, and many more. The game also includes a level editor which lets you create your own maps from scratch, or re-create sections from your favorite Mario game, your imagination is the limit! Recently included is a world editor, which strings a bunch of levels with specified conditions to create an SMB3-like experience merged with tournament like play. The game is more importantly a tribute to Nintendo and the original fangame game Mario War by Samuele Poletto.

The game uses artwork and sounds from Nintendo games. We hope that this noncommercial fangame qualifies as fair use work. We just wanted to create this game to show how much we adore Nintendo's characters and games.

### Features

- Up to four players deathmatch fun
- a whole bunch of game modes (featuring GetTheChicken, Domination, CTF, ...)
- online and local multiplayer
- Comes with the leveleditor - you can create your own maps...
- ... and a lot of people did so. There are currently over 1000 maps
- More fun than poking a monkey with a stick
- The whole source code of the game is available, for free
- uses SDL and is fully portable to windows, linux, mac, ...
- CPU Players
- will make you happy and gives you a fuzzy feeling

## How to play

Please see documentation in the docs/ directory.
