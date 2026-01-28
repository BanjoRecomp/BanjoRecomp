# Banjo: Recompiled
Banjo: Recompiled is a project that uses [N64: Recompiled](https://github.com/N64Recomp/N64Recomp) to **statically recompile** Banjo-Kazooie into a native port with many new features, enhancements, and extensive mod support. This project uses [RT64](https://github.com/rt64/rt64) as the rendering engine to provide graphical enhancements.

### [Check out the latest release here](https://github.com/BanjoRecomp/BanjoRecomp/releases/latest).

Join the [N64: Recompiled Community Discord](https://discord.gg/AWZThJ4dPf) to discuss this and other N64: Recompiled projects!

[![Discord Invitation](https://discordapp.com/api/guilds/1374083583739826328/widget.png?style=banner2 'N64 Recomp')](https://discord.gg/AWZThJ4dPf)

### **This repository and its releases do not contain game assets. The original game is required to build or run this project.**

## Table of Contents
* [System Requirements](#system-requirements)
* [Features](#features)
  * [Plug and Play](#plug-and-play)
  * [Fully Intact N64 Effects](#fully-intact-n64-effects)
  * [Faithful Audio](#faithful-audio)
  * [Easy-to-Use Menus](#easy-to-use-menus)
  * [High Framerate Support](#high-framerate-support)
  * [Widescreen and Ultrawide Support](#widescreen-and-ultrawide-support)
  * [Mod Support](#mod-support)
  * [Dual Analog Camera](#dual-analog-camera)
  * [Note Saving](#note-saving)
  * [Low Input Lag](#low-input-lag)
  * [Instant Load Times](#instant-load-times)
  * [Linux and Steam Deck Support](#linux-and-steam-deck-support)
* [Planned Features](#planned-features)
* [FAQ](#faq)
* [Known Issues](#known-issues)
* [Building](#building)
* [Libraries Used and Projects Referenced](#libraries-used-and-projects-referenced)

## System Requirements
A GPU supporting Direct3D 12.0 (Shader Model 6), Vulkan 1.2, or Metal Argument Buffers Tier 2 support is required to run this project. The oldest GPUs that should be supported for each vendor are:
* GeForce GT 630
* Radeon HD 7750 (the one from 2012, not to be confused with the RX 7000 series) and newer
* Intel HD 510 (Skylake)
* A Mac with Apple Silicon or an Intel 7th Gen CPU with MacOS 13.0+

On x86-64 PCs, a CPU supporting the SSE4.1 instruction set is also required (Intel Core 2 Penryn series or AMD Bulldozer and newer). ARM64 builds will work on any ARM64 CPU.

If you have issues with crashes on startup, make sure your graphics drivers are fully up to date. 

## Features

#### Plug and Play
Simply provide your copy of the North American 1.0 version of the game in the main menu and start playing! This project will automatically load assets from the provided copy, so there is no need to go through a separate extraction step or build the game yourself.

#### Fully Intact N64 Effects
All advanced graphical effects, such as the jigsaw puzzle screen transition and Bottles' Moving Picture Game are fully intact and have been adapted for widescreen and high framerates. Fine details such as the game's original mipmapping preserved with a high degree of accuracy.

#### Faithful Audio
Music and sounds are perfectly faithful to the original N64 version of game, with all audio processing intact and no popping or stuttering. Scenes that synchronize audio to visuals have their timings corrected to ensure they remain synchronized as they did on original hardware.

#### Easy-to-Use Menus
Gameplay settings, graphics settings, input mappings, and audio settings can all be configured with the in-game config menu. The menus can all be used with mouse, controller, or keyboard for maximum convenience.

#### High Framerate Support
Play at any framerate you want thanks to functionality provided by RT64! Game objects and terrain, texture scrolling, screen effects, and all HUD elements are all rendered at high framerates. By default, this project is configured to run at your monitor's refresh rate. You can also play at the original framerate of the game if you prefer. **Changing framerate has no effect on gameplay.**

**Note**: External framerate limiters (such as the NVIDIA Control Panel) are known to potentially cause problems, so if you notice any stuttering then turn them off and use the manual framerate slider in the in-game graphics menu instead.

#### Widescreen and Ultrawide Support
Any aspect ratio is supported, with all effects modded to work correctly in widescreen. The HUD can also be positioned at 16:9 when using ultrawide aspect ratios if preferred. Game models have been extended where necessary to ensure cutscenes work correctly in 16:9 aspect ratio. Cutscenes can also be optionally pillarboxed to 16:9 or the game's original aspect ratio to hide any details that weren't meant to be visible.

#### Mod Support
Install community made mods and texture packs! Mods can change any part of the game, including adding completely new features and content. You can install mods by simply dragging the mod files onto the game window before starting the game or by clicking the **Install Mods** button in the mod menu. Mods can be toggled in the mod menu, and some mods can be configured there as well.

A Thunderstore mod hosting page is currently being set up for this project. In the meantime, you can find the mods showcased in the release trailer here:

* [BK Jiggies of Time](https://github.com/Mr-Wiseguy/JiggiesOfTimeRecomp/releases/latest)
* [BK Nostalgia 64](https://github.com/Mr-Wiseguy/Nostalgia64Recomp/releases/latest)
* [BK Reloaded Texture Pack](https://evilgames.eu/texture-packs/bk-reloaded.htm)

If you're interested in making mods for this project, check out [the mod template](https://github.com/BanjoRecomp/BKRecompModTemplate) and [the modding documentation](https://hackmd.io/fMDiGEJ9TBSjomuZZOgzNg). If you're interested in making texture packs, check out [the RT64 documentation](https://github.com/rt64/rt64/blob/main/TEXTURE-PACKS.md). You can also join the [N64: Recompiled Modding Discord server](https://discord.gg/Z8Nbm455D4).

#### Dual Analog Camera
Play with a dual analog control for finer camera control! When this option is enabled, the right analog stick will control the camera. Buttons mapped to the right analog stick will still work while crouching, so you can keep the C-buttons mapped to it even when this setting is enabled.

#### Note Saving
Keep your notes when leaving a level or dying like in later releases of the game!

This setting can be turned off to restore the N64 version's note score mechanic. Changing this setting will only take effect when returning to Grunty's Lair or restarting the game.

#### Low Input Lag
This project has been optimized to have as little input lag as possible, making the game feel more responsive than ever!

#### Instant Load Times
Saving and loading files, going from place to place, and pausing all happen in the blink of an eye thanks to the game running natively on modern hardware.

#### Linux and Steam Deck Support
A Linux binary as well as a Flatpak is available for playing on most up-to-date distros, including on the Steam Deck.

To play on Steam Deck, extract the Linux build onto your deck. Then, in desktop mode, right click the BanjoRecompiled executable file and select "Add to Steam". From there, you can return to Gaming mode and configure the controls as needed.

## Planned Features
* Ray Tracing via RT64

## FAQ

#### What is static recompilation?
Static recompilation is the process of automatically translating an application from one platform to another. For more details, check out the full description of how this project's recompilation works here: [N64: Recompiled](https://github.com/Mr-Wiseguy/N64Recomp).

#### How is this related to the decompilation project?
Unlike N64 ports in the past, this project is not based on the source code provided by a decompilation of the game. This is because static recompilation bypasses the need for decompiled source code when making a port, allowing ports to be made **without source code**. However, the reverse engineering work done by the decompilation team was invaluable for providing some of the enhancements featured in this project. For this reason, the project uses headers and some functions from the decompilation project in order to make modifications to the game. Many thanks to the decompilation team for all of the hard work they've done.

#### Where is the savefile stored?
- Windows: `%LOCALAPPDATA%\BanjoRecompiled\saves`
- Linux: `~/.config/BanjoRecompiled/saves`
- macOS: `~/Library/Application Support/BanjoRecompiled/saves`

#### How do I choose a different ROM?
**You don't.** This project is **only** a port of Banjo-Kazooie, and it will only accept one specific ROM: the US 1.0 version of the N64 release of Banjo-Kazooie. ROMs in formats other than .z64 will be automatically converted, as long as it is the correct ROM. **This is not an emulator, and it cannot run any arbitrary ROM.**

Instead, you can change the game by installing mods. See the [mod support](#mod-support) section for details.

#### Can you run this project as a portable application?
Yes, if you place a file named `portable.txt` in the same folder as the executable then this project will run in portable mode. In portable mode, the save files, config files, and mods are placed in the same folder as the executable.

#### Did you fix the duplicate Mumbo token IDs?
Yes. The incorrect Mumbo tokens now use previously unused token IDs. This won't affect mods that replace levels.

#### Does note saving have the same issue as the XBLA version where it collects notes during Bottles' puzzles and prevents you from getting 100% completion?
No. Take has been care to ensure that notes are not collected during demos. 

## Known Issues
* Overlays such as MSI Afterburner and other software such as Wallpaper Engine can cause performance issues with this project that prevent the game from rendering correctly. Disabling such software is recommended.

## Building
Building is not required to play this project, as prebuilt binaries (which do not contain game assets) can be found in the [Releases](https://github.com/BanjoRecomp/BanjoRecomp/releases) section. Instructions on how to build this project can be found in the [BUILDING.md](BUILDING.md) file.

## Libraries Used and Projects Referenced
* [N64Recomp](https://github.com/N64Recomp/N64Recomp) for statically recompiling the game's code to run natively
* [RT64](https://github.com/rt64/rt64) for the project's rendering engine
* [N64ModernRuntime](https://github.com/N64Recomp/N64ModernRuntime) for replacing the original N64 runtime libraries and providing mod support
* [RecompFrontend](https://github.com/N64Recomp/RecompFrontend) for menus and input handling
* [Banjo-Kazooie Decompilation](https://gitlab.com/banjo.decomp/banjo-kazooie) for headers and some function definitions, used for making patches or some enhancements
