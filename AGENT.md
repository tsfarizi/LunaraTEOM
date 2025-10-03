# LunaraTEOM Agent Guide

## How to Use This Guide
- You are assisting on LunaraTEOM, an RTS-inspired UE project. Keep answers concise and reference sections below when generating responses.
- Always target **Unreal Engine 5.6** APIs and tooling. When writing or reviewing code, validate naming and behaviour against UE 5.6 documentation and headers from the installed engine.
- Prefer existing project patterns and styles (Slate, Enhanced Input, modular UI colours) before introducing new paradigms.

## Project Snapshot
```yaml
agent_spec: 1.0
project:
  name: LunaraTEOM
  uproject: LunaraTEOM.uproject
  engine_version: "5.6"
  engine_root_env: UE_PATH
targets:
  - name: LunaraTEOM
    type: Game
    configs: [Development, Shipping]
    platforms: [Win64]
  - name: LunaraTEOMEditor
    type: Editor
    configs: [Development]
    platforms: [Win64]
maps:
  default_game_map: /Game/Level/Level
  test_maps: []
plugins:
  required:
    - ModelingToolsEditorMode
    - GameplayBehaviors
    - AvalancheSequenceNavigator
    - Avalanche
  optional:
    - CameraShakePreviewer
    - GameplayCameras
build:
  ubt: {UE_PATH}/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool {Target} {Platform} {Config} -Project="{uproject}" -Progress -NoHotReload
  uat: {UE_PATH}/Engine/Build/BatchFiles/RunUAT BuildCookRun -project="{uproject}" -noP4 -platform={Platform} -clientconfig={Config} -build -cook -stage -pak -archive -archivedirectory="{outdir}"
run:
  editor_game: {UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "{uproject}" {Map}?game -game -log
  editor_pie: {UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "{uproject}" -game -log
  editor_standalone: {UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "{uproject}" {Map} -log
  editor_linux: {UE_PATH}/Engine/Binaries/Linux/UnrealEditor "{uproject}" {Map} -log
  editor_mac: {UE_PATH}/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor "{uproject}" {Map} -log
automation:
  tests: []
content_rules:
  lfs_patterns: []
  do_not_move:
    - Content/*.uasset
    - Content/*.umap
  naming_style_url: https://docs.unrealengine.com/5.0/en-US/recommended-asset-naming-conventions-in-unreal-engine/
mod_policy:
  allowed_paths:
    - Source/**
    - Config/**
    - Plugins/**
  forbidden_paths:
    - Content/**
    - DerivedDataCache/**
    - Saved/**
  safe_operations:
    - GenerateProjectFiles
    - UpdateConfig
    - AddCxxModule
ci:
  steps:
    - build_editor
    - run_tests
    - package
```

## Orientation
- Entry point: Blueprint `BP_GameMode_Lunara` on `/Game/Level/Level`. Camera pawn (`ACameraPawn`) is possessed by `ALunaraTeomPlayerController` to drive RTS controls.
- UI stack: custom Slate widgets (`SMainWindowWidget`, `SListBuildingContainerWidget`, `SBeveledBorder`, etc.) wrapped for UMG exposure. Global palette lives in `FLunaraTeomSlateStyle`.
- Input: Enhanced Input assets under `/Game/Input`. Keep new bindings consistent with existing mapping contexts.

## Build & Automation
- Set `UE_PATH` to the UE 5.6 installation directory before invoking tools.
- Regenerate project files when modules change: `{UE_PATH}/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool -ProjectFiles -project="LunaraTEOM.uproject" -game -rocket`.
- Example builds:
  - Game: `... UnrealBuildTool LunaraTEOM Win64 Development -Project="LunaraTEOM.uproject" -Progress -NoHotReload`
  - Editor: `... UnrealBuildTool LunaraTEOMEditor Win64 Development -Project="LunaraTEOM.uproject"`
- Packaging (UAT): run from repo root using command string in the YAML snapshot.

## Run Profiles
- **PIE:** `{UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "LunaraTEOM.uproject" -game -log`
- **Standalone:** `{UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "LunaraTEOM.uproject" /Game/Level/Level -game -log`
- **Headless listen server:** add `?listen -server -log -nullrhi`
- For quick client joins, launch additional editor instances with window offsets (`-WinX/-WinY`) and connect via `open 127.0.0.1`.

## Coding Guidelines (UE 5.6 Focus)
- Treat Unreal Engine **5.6** as the single source of truth. When adding APIs or macros, verify signatures in the installed source or official UE 5.6 docs before committing.
- Mirror existing module layout (`Source/LunaraTEOM`) and use project style headers for includes (`UI/Style/LunaraTeomSlateWidgetStyle.h`, etc.).
- Slate/UMG: pull palette colours from `FLunaraTeomSlateStyle` rather than in-line literals. Extend the style struct if a new colour is needed.
- C++ gameplay: respect Enhanced Input conventions (mapping contexts, trigger thresholds) and keep constructor defaults safe for Loader builds.
- Testing hooks are minimal. If adding tests, wire them into Automation (Session Frontend) and update this guide.

## Content & Modification Policy
- Editing allowed: `Source/`, `Config/`, and project plugins. Avoid moving or renaming assets under `Content/` without explicit request.
- Keep required plugins enabled; optional ones can be toggled per-need but test Editor startup afterwards.
- When adding assets, follow Unreal naming conventions (link in YAML). No large-scale LFS changes unless coordinated.

## UI Palette Reference (from `FLunaraTeomSlateStyle`)
| Property | Default RGBA | Usage Notes |
| --- | --- | --- |
| `PrimaryColor` | (0.110, 0.184, 0.360, 1.0) | Primary interactive surfaces and window chrome. |
| `HighlightColor` | (0.306, 0.772, 0.945, 1.0) | Hover accents, sheen effects. |
| `BackgroundColor` | (0.086, 0.133, 0.262, 1.0) | Panel backgrounds, glass blur tint. |
| `AccentColor` | (0.851, 0.650, 0.302, 1.0) | Call-to-action edges, hero elements. |
| `SecondaryColor` | (0.721, 0.537, 0.227, 1.0) | Subtle dividers or gradient blends. |

Fonts are provided via Cinzel soft references (Regular through Black). Reuse these `FSlateFontInfo` members for consistency.

## Troubleshooting Tips
- **UHT errors:** regenerate project files and clean `Intermediate/Build/Win64`. Confirm headers expose `LUNARA_API` where needed.
- **Link failures:** rebuild target with `-Clean` and ensure optional plugins are compiled for the requested configuration.
- **Cook issues:** clear `Saved/Cooked/` and re-run UAT; review log for missing asset references.
- **Asset redirectors:** after editor refactors, run *Fix Up Redirectors* before submitting changes.

## Keep This Guide Fresh
If tooling, engine version, or workflows change, update both the YAML snapshot and relevant sections so future agents stay aligned with UE 5.6 expectations.

