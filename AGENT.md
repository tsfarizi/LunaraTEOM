# AGENT.md

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

## Project Orientation
* Single runtime module **LunaraTEOM** implements the primary game module and houses camera, player controller, and extensive Slate/UMG wrapper code.
* Gameplay starts from Blueprint **BP_GameMode_Lunara** with default map `/Game/Level/Level` configured in `Config/DefaultEngine.ini`; camera pawn `ACameraPawn` provides RTS-style controls and is intended to be possessed by `ALunaraTeomPlayerController`.
* UI layer leans on custom Slate widgets (`SMainWindowWidget`, `SIconButtonWidget`, `STextButtonWidget`) wrapped for UMG exposure; styling centralised in `FLunaraTeomSlateStyle`.
* Enhanced Input is assumed with soft references to mapping context and actions located under `/Game/Input`.

## Build & Toolchain
* Set environment variable **UE_PATH** to the Unreal Engine 5.6 install root before running any commands.
* Regenerate project files when adjusting module targets using `"{UE_PATH}/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool" -ProjectFiles -project="LunaraTEOM.uproject" -game -rocket`.
* Deterministic builds: run UBT from repository root, e.g. `... UnrealBuildTool LunaraTEOM Win64 Development -Project="LunaraTEOM.uproject"` (Shipping also supported).
* Editor target (`LunaraTEOMEditor`) defaults to Development configuration; specify `-TargetType=Editor` if invoking via GenerateProjectFiles.

## Run Profiles
* **Play In Editor (PIE):** `{UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "LunaraTEOM.uproject" -game -log` then press Play; ensures PIE inherits default map/game mode.
* **Standalone window:** `{UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "LunaraTEOM.uproject" /Game/Level/Level -log -game` for runtime parity without full PIE overhead.
* **Headless listen server:** `{UE_PATH}/Engine/Binaries/Win64/UnrealEditor.exe "LunaraTEOM.uproject" /Game/Level/Level?listen -server -log -nullrhi`.
* **Quick multiplayer soak:** launch server command above, then two clients with `... -game -log -WinX=0 -WinY=0` and `... -game -log -WinX=1920 -WinY=0`, connecting via `open 127.0.0.1` console.

## Automation & Tests
* No AutomationSpec, Gauntlet, or unit tests are present. Use the in-editor Session Frontend > Automation tab for ad-hoc map or widget checks if added later.
* For performance captures, append `-benchmark -csvlog=Perf.csv` to standalone/editor commands.

## Modification Policy
* C++ and config changes are safe within `Source/` and `Config/`. Introduce new Slate assets or Enhanced Input mappings via content browser but avoid bulk moves in `Content/` without coordination.
* Do not alter engine plugins included via association; introduce new optional plugins by editing `LunaraTEOM.uproject` only when build-tested on Win64.
* Maintain constructor defaults for camera movement to keep Enhanced Input tuning predictable.

## Data & Asset Hygiene
* `.gitignore` already excludes `Binaries/`, `DerivedDataCache/`, `Intermediate/`, and packaged builds—keep these directories clean before commits.
* Large binary assets (`*.uasset`, `*.umap`) should remain under Git LFS if adopted; coordinate before enabling to avoid history rewrite.
* Cooked outputs belong under `WindowsNoEditor/` (or platform equivalent); archive releases using the provided UAT template to `{outdir}` outside the repo.

## Dependency Matrix & Feature Flags
* **Module `LunaraTEOM`:** Public deps on `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `UMG`, `Slate`, `SlateCore`; private deps duplicate Slate/UMG for Slate widget implementations.
* Renderer tweaks in `Config/DefaultEngine.ini` enable Nanite-friendly lighting (`r.AllowStaticLighting=False`, `r.GenerateMeshDistanceFields=True`, ray tracing flags) and default DX12 on Windows.
* Game mode redirectors in `DefaultEngine.ini` map legacy `TP_BlankBP` names to `/Script/LunaraTEOM`.

## Exposed Properties (Editor UI)

### Module: LunaraTEOM, Class: ACameraPawn

/// Purpose: Root component anchoring spring arm and camera hierarchy.
/// Type: TObjectPtr<USceneComponent>
/// Default: Instanced in constructor (SceneRoot)
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Camera" | Meta: AllowPrivateAccess="true"
/// Note: Avoid reparenting; spring arm assumes this as attach parent.
SceneRoot (TObjectPtr<USceneComponent>)

/// Purpose: Manages boom rotation and length for orbit/zoom controls.
/// Type: TObjectPtr<USpringArmComponent>
/// Default: Instanced in constructor with collision disabled
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Camera" | Meta: AllowPrivateAccess="true"
/// Note: Keep collision test off to preserve unobstructed top-down camera.
SpringArm (TObjectPtr<USpringArmComponent>)

/// Purpose: Active view camera at spring arm tip.
/// Type: TObjectPtr<UCameraComponent>
/// Default: Instanced in constructor; control rotation disabled
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Camera" | Meta: AllowPrivateAccess="true"
/// Note: Re-enable control rotation only if pawn rotation is overhauled.
Camera (TObjectPtr<UCameraComponent>)

/// Purpose: Minimum boom length controlling closest zoom distance.
/// Type: float | Units: cm | Range: ≥10 | Default: 400
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Zoom" | Meta: ClampMin="10.0"
/// Note: Lowering below 200 risks near-plane clipping; adjust jump threshold accordingly.
MinArmLength (float)

/// Purpose: Maximum boom length for furthest zoom-out.
/// Type: float | Units: cm | Range: ≥100 | Default: 2500
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Zoom" | Meta: ClampMin="100.0"
/// Note: Raising increases parallax; keep ≤4000 for level bounds awareness.
MaxArmLength (float)

/// Purpose: Step size per zoom input.
/// Type: float | Units: cm | Range: ≥1 | Default: 120
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Zoom" | Meta: ClampMin="1.0"
/// Note: Match to Enhanced Input axis scalar to avoid hypersensitive zoom.
ZoomStep (float)

/// Purpose: Toggles zoom axis inversion across all devices.
/// Type: bool | Default: false
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Zoom"
/// Note: Flip only if mouse wheel feels reversed to players; applies globally.
bInvertZoom (bool)

/// Purpose: Horizontal orbit speed.
/// Type: float | Units: deg/s | Range: ≥0 | Default: 120
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Orbit" | Meta: ClampMin="0.0"
/// Note: Tune alongside input sensitivity for consistent feel across DPI scales.
OrbitYawSpeed (float)

/// Purpose: Vertical orbit speed.
/// Type: float | Units: deg/s | Range: ≥0 | Default: 90
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Orbit" | Meta: ClampMin="0.0"
/// Note: Excessive values risk overshooting pitch clamps; pair with Min/MaxPitch.
OrbitPitchSpeed (float)

/// Purpose: Lower pitch clamp (negative values aim downward).
/// Type: float | Units: degrees | Range: ≤0 | Default: -75
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Orbit" | Meta: ClampMax="0.0"
/// Note: Keep below zero to maintain top-down perspective; swap with MaxPitch if inverted.
MinPitch (float)

/// Purpose: Upper pitch clamp.
/// Type: float | Units: degrees | Range: ≤0 | Default: -30
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Orbit" | Meta: ClampMax="0.0"
/// Note: Values near 0 flatten view; ensure MinPitch ≤ MaxPitch (auto-swapped in BeginPlay).
MaxPitch (float)

/// Purpose: Pan speed for ground-plane translation.
/// Type: float | Units: cm/s | Range: ≥0 | Default: 1500
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Pan" | Meta: ClampMin="0.0"
/// Note: Higher speeds demand larger JumpThreshold to avoid overshoot.
PanSpeed (float)

/// Purpose: Trace distance for cursor-based focus during zoom.
/// Type: float | Units: cm | Range: ≥100 | Default: 50000
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Focus" | Meta: ClampMin="100.0"
/// Note: Long rays cost performance; match to tallest level geometry.
RayLength (float)

/// Purpose: Fallback plane height when focus ray misses.
/// Type: float | Units: cm | Default: 0
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Focus"
/// Note: Set to terrain Z to maintain consistent zoom anchor over voids.
GroundZ (float)

/// Purpose: Distance threshold before accepting a new focus hit.
/// Type: float | Units: cm | Range: ≥0 | Default: 100
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Focus" | Meta: ClampMin="0.0"
/// Note: Increase for rapid camera moves; smaller keeps cursor-precise zoom.
JumpThreshold (float)

/// Purpose: Master toggle for camera input.
/// Type: bool | Default: true
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input"
/// Note: Disable during UI capture to prevent stray movement.
bInputEnabled (bool)

/// Purpose: Enables debug draw/logging.
/// Type: bool | Default: false
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Debug"
/// Note: May spam log; leave false in production builds.
bDebug (bool)

/// Purpose: Direct reference to Enhanced Input mapping context.
/// Type: TObjectPtr<UInputMappingContext>
/// Default: nullptr (resolved via soft path)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Assign for editor preview to avoid runtime load delays.
DefaultInputMapping (TObjectPtr<UInputMappingContext>)

/// Purpose: Soft path fallback when mapping context pointer unset.
/// Type: FSoftObjectPath | Default: /Game/Input/IMC_CameraPawn.IMC_CameraPawn
/// Specifiers: EditDefaultsOnly, BlueprintReadOnly | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Ensure asset exists; missing path logs warnings and skips mapping.
DefaultInputMappingPath (FSoftObjectPath)

/// Purpose: Direct Enhanced Input action for zoom.
/// Type: TObjectPtr<UInputAction>
/// Default: nullptr (resolved on demand)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Provide axis-type action outputting float values.
ZoomAction (TObjectPtr<UInputAction>)

/// Purpose: Soft path fallback for zoom action.
/// Type: FSoftObjectPath | Default: TODO
/// Specifiers: EditDefaultsOnly, BlueprintReadOnly | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Fill with `/Game/Input/IA_Zoom.IA_Zoom` or equivalent to auto-load at runtime.
ZoomActionPath (FSoftObjectPath)

/// Purpose: Direct Enhanced Input action for orbit.
/// Type: TObjectPtr<UInputAction>
/// Default: nullptr
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Requires 2D axis; align mapping with OrbitYawSpeed and OrbitPitchSpeed.
OrbitAction (TObjectPtr<UInputAction>)

/// Purpose: Soft path fallback for orbit action.
/// Type: FSoftObjectPath | Default: TODO
/// Specifiers: EditDefaultsOnly, BlueprintReadOnly | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Set to your IA asset for hands-off initialization.
OrbitActionPath (FSoftObjectPath)

/// Purpose: Direct Enhanced Input action for panning.
/// Type: TObjectPtr<UInputAction>
/// Default: nullptr
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Should supply 2D axis scaled to cm/s (WASD/drag).
PanAction (TObjectPtr<UInputAction>)

/// Purpose: Soft path fallback for pan action.
/// Type: FSoftObjectPath | Default: TODO
/// Specifiers: EditDefaultsOnly, BlueprintReadOnly | Category: "Camera|Input" | Meta: AllowPrivateAccess="true"
/// Note: Align with Enhanced Input asset naming to avoid load failures.
PanActionPath (FSoftObjectPath)

/// Purpose: Priority when adding mapping context to Enhanced Input subsystem.
/// Type: int32 | Range: ≥0 | Default: 0
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Camera|Input" | Meta: ClampMin="0"
/// Note: Raise when multiple contexts compete for bindings.
InputMappingPriority (int32)

### Module: LunaraTEOM, Class: ALunaraTeomPlayerController

/// Purpose: Enables click events for actor and Slate interactions.
/// Type: bool | Default: true
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Input|Cursor"
/// Note: Disable only if routing clicks exclusively through Enhanced Input.
bEnableMouseClickEvents (bool)

/// Purpose: Enables hover events for Slate widgets and hit proxies.
/// Type: bool | Default: true
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Input|Cursor"
/// Note: Required for tooltip and hover effects; slight overhead in dense scenes.
bEnableMouseHoverEvents (bool)

/// Purpose: Sets default hardware cursor icon.
/// Type: TEnumAsByte<EMouseCursor::Type>
/// Default: EMouseCursor::Default
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Input|Cursor"
/// Note: Change to Crosshairs or Custom as needed; ensure cursor assets exist.
DefaultCursor (TEnumAsByte<EMouseCursor::Type>)

### Module: LunaraTEOM, Struct: FListBuildingButtonItem

/// Purpose: Text displayed on each list-building button.
/// Type: FText | Default: Empty
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "List Building"
/// Note: Localize via LOCTEXT; avoid runtime formatting to keep widget stable.
Label (FText)

/// Purpose: Slate brush used as the button's icon.
/// Type: FSlateBrush | Default: Empty brush
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "List Building"
/// Note: Assign texture/material resources; respects standard brush sizing.
Icon (FSlateBrush)

### Module: LunaraTEOM, Class: UListBuildingContainerWidget

/// Purpose: Defines the ordered buttons rendered by the Slate container.
/// Type: TArray<FListBuildingButtonItem>
/// Default: Empty
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "List Building" | Meta: ExposeOnSpawn="true"
/// Note: Synchronize via `SetButtonItems` to propagate changes to Slate immediately.
ButtonItems (TArray<FListBuildingButtonItem>)

### Module: LunaraTEOM, Class: UIconButtonWidget

/// Purpose: Asset used as the circular icon for the button.
/// Type: TSoftObjectPtr<UObject>
/// Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: AllowedClasses="Texture2D,MaterialInterface"
/// Note: Soft reference delays load; call `SynchronizeProperties` after swapping assets at runtime.
IconAsset (TSoftObjectPtr<UObject>)

/// Purpose: Diameter of the icon button.
/// Type: float | Units: px | Range: ≥16 | Default: 64
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: ClampMin=16
/// Note: Large diameters may require layout padding updates in parent Slate widgets.
Diameter (float)

/// Purpose: Blueprint event fired on confirmed click.
/// Type: FOnIconButtonClickedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Bind in UMG designer or Blueprint to trigger gameplay logic.
OnClicked (FOnIconButtonClickedBP)

/// Purpose: Event fired immediately on press.
/// Type: FOnIconButtonPressedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Use for audio feedback or pressed states.
OnPressed (FOnIconButtonPressedBP)

/// Purpose: Event fired when press ends over the button.
/// Type: FOnIconButtonReleasedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Pair with OnPressed for full press/release workflows.
OnReleased (FOnIconButtonReleasedBP)

/// Purpose: Event fired when pointer starts hovering.
/// Type: FOnIconButtonHoveredBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Trigger highlight animations; ensure hover events enabled on PlayerController.
OnHovered (FOnIconButtonHoveredBP)

/// Purpose: Event fired when pointer stops hovering.
/// Type: FOnIconButtonUnhoveredBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Reset visual state on hover exit.
OnUnhovered (FOnIconButtonUnhoveredBP)

### Module: LunaraTEOM, Class: UTextButtonWidget

/// Purpose: Label text shown on the button.
/// Type: FText | Default: "Button"
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Content"
/// Note: Use LOCTEXT for localization; dynamic changes via `SynchronizeProperties`.
Label (FText)

/// Purpose: Font asset for button label.
/// Type: TSoftObjectPtr<UObject>
/// Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: AllowedClasses="Font,FontFace"
/// Note: Soft reference allows per-button font customization without hard loads.
FontObject (TSoftObjectPtr<UObject>)

/// Purpose: Point size for label font.
/// Type: int32 | Units: points | Range: ≥1 | Default: 18
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: ClampMin=1
/// Note: Combine with FontObject to match Slate style guidelines.
FontSize (int32)

/// Purpose: Click event for Blueprint logic.
/// Type: FOnButtonClickedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Bind for interaction behavior; triggered after press-release combo.
OnClicked (FOnButtonClickedBP)

/// Purpose: Press event fired on button down.
/// Type: FOnButtonPressedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Ideal for playing pressed animations or sounds.
OnPressed (FOnButtonPressedBP)

/// Purpose: Release event when pointer lifts over the button.
/// Type: FOnButtonReleasedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Reset states or trigger hold-duration logic.
OnReleased (FOnButtonReleasedBP)

/// Purpose: Hover enter event.
/// Type: FOnButtonHoveredBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Ensure PlayerController hover events enabled to receive callbacks.
OnHovered (FOnButtonHoveredBP)

/// Purpose: Hover exit event.
/// Type: FOnButtonUnhoveredBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Cleanup highlight or tooltip UI.
OnUnhovered (FOnButtonUnhoveredBP)

### Module: LunaraTEOM, Class: UMainWindowWidget

/// Purpose: Window title text.
/// Type: FText | Default: Empty
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Content"
/// Note: Bind to reflect context (e.g., building name); supports localization.
Title (FText)

/// Purpose: Font asset for window title.
/// Type: TSoftObjectPtr<UObject>
/// Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: AllowedClasses="Font,FontFace"
/// Note: Align with `FLunaraTeomSlateStyle` fonts for cohesive look.
TitleFontObject (TSoftObjectPtr<UObject>)

/// Purpose: Point size for window title font.
/// Type: int32 | Units: points | Range: ≥1 | Default: 20
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: ClampMin=1
/// Note: Larger sizes require adjusting title bar padding.
TitleFontSize (int32)

/// Purpose: Optional icon displayed beside title.
/// Type: TSoftObjectPtr<UObject>
/// Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Appearance" | Meta: AllowedClasses="Texture2D,MaterialInterface"
/// Note: Provides brand imagery; ensure texture authored for Slate UI scale.
IconObject (TSoftObjectPtr<UObject>)

/// Purpose: Event fired when close button clicked.
/// Type: FOnWindowCloseClickedBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Bind to remove window widgets or trigger cleanup logic.
OnCloseClicked (FOnWindowCloseClickedBP)

/// Purpose: Event fired after window becomes visible.
/// Type: FOnWindowShownBP
/// Default: Unbound
/// Specifiers: BlueprintAssignable | Category: "Events"
/// Note: Useful for lazy loading content or starting animations once shown.
OnShown (FOnWindowShownBP)

### Module: LunaraTEOM, Struct: FLunaraTeomSlateStyle

/// Purpose: Primary surface color for windows/cards.
/// Type: FLinearColor | Default: (0.658, 0.478, 0.176, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Adjust alongside SecondaryColor to maintain contrast ratios.
SurfaceColor (FLinearColor)

/// Purpose: Main accent color for interactive elements.
/// Type: FLinearColor | Default: (0.110, 0.184, 0.360, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Keep readability against BackgroundColor; drives button states.
PrimaryColor (FLinearColor)

/// Purpose: Highlight color for hover/focus.
/// Type: FLinearColor | Default: (0.306, 0.772, 0.945, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Align with accessibility guidelines; ensure high luminance.
HighlightColor (FLinearColor)

/// Purpose: Background color for frames/panels.
/// Type: FLinearColor | Default: (0.086, 0.133, 0.262, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Changing requires retuning text colors for legibility.
BackgroundColor (FLinearColor)

/// Purpose: Secondary accent for emphasis/status.
/// Type: FLinearColor | Default: (0.851, 0.650, 0.302, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Use sparingly to avoid overpowering PrimaryColor.
AccentColor (FLinearColor)

/// Purpose: Support color for subtle UI details.
/// Type: FLinearColor | Default: (0.721, 0.537, 0.227, 1)
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Colors"
/// Note: Harmonize with SurfaceColor for gradient ramps.
SecondaryColor (FLinearColor)

/// Purpose: Soft reference to Cinzel Regular font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Assign valid font assets; runtime loads fallback to CoreStyle if unset.
CinzelRegularFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Soft reference to Cinzel Medium font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Keeps typography consistent; load synchronously in style ctor.
CinzelMediumFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Soft reference to Cinzel SemiBold font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Useful for emphasized headers.
CinzelSemiBoldFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Soft reference to Cinzel Bold font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Pair with HighlightColor for CTA text.
CinzelBoldFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Soft reference to Cinzel ExtraBold font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Reserve for large titles to avoid heavy weight proliferation.
CinzelExtraBoldFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Soft reference to Cinzel Black font face.
/// Type: TSoftObjectPtr<UFontFace> | Default: None
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Provides ultra-bold option; ensure legibility at small sizes.
CinzelBlackFace (TSoftObjectPtr<UFontFace>)

/// Purpose: Default font size applied when constructing Slate fonts.
/// Type: int32 | Units: points | Default: 24
/// Specifiers: EditAnywhere, BlueprintReadWrite | Category: "Lunara|Fonts"
/// Note: Changing triggers reinitialization of runtime font infos.
DefaultFontSize (int32)

/// Purpose: Runtime font info built from CinzelRegularFace.
/// Type: FSlateFontInfo
/// Default: Populated in constructor via CinzelRegularFace or fallback CoreStyle
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Read-only; use for binding Slate style resources.
CinzelRegular (FSlateFontInfo)

/// Purpose: Runtime font info built from CinzelMediumFace.
/// Type: FSlateFontInfo
/// Default: Computed in constructor using DefaultFontSize
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Access for medium-weight text styles.
CinzelMedium (FSlateFontInfo)

/// Purpose: Runtime font info built from CinzelSemiBoldFace.
/// Type: FSlateFontInfo
/// Default: Computed in constructor
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Bind to Slate style sets; do not modify in editor.
CinzelSemiBold (FSlateFontInfo)

/// Purpose: Runtime font info built from CinzelBoldFace.
/// Type: FSlateFontInfo
/// Default: Computed in constructor
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Use for bold emphasis within Slate brushes.
CinzelBold (FSlateFontInfo)

/// Purpose: Runtime font info built from CinzelExtraBoldFace.
/// Type: FSlateFontInfo
/// Default: Computed in constructor
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Designed for hero text; heavy weight.
CinzelExtraBold (FSlateFontInfo)

/// Purpose: Runtime font info built from CinzelBlackFace.
/// Type: FSlateFontInfo
/// Default: Computed in constructor
/// Specifiers: VisibleAnywhere, BlueprintReadOnly | Category: "Lunara|Fonts"
/// Note: Highest weight; ensure kerning adjustments for readability.
CinzelBlack (FSlateFontInfo)

## Troubleshooting
* **UHT reflection errors:** Run `UnrealBuildTool ... -ProjectFiles` to regenerate, verify all header includes have proper minimal APIs, and delete `Intermediate/Build/Win64` before rebuilding.
* **Linker failures in Shipping:** Ensure optional plugins remain disabled or built for Shipping; re-run `UnrealBuildTool LunaraTEOM Win64 Shipping` with `-Clean` if stale objects persist.
* **Cook issues or missing assets:** Delete `Saved/Cooked/` and re-cook via UAT; fix any soft-path references (`DefaultInputMappingPath`, `ZoomActionPath`) reported as missing in logs.
* **Editor redirectors:** After moving assets in the editor, use `Content Browser > Fix Up Redirectors` before committing to keep soft references valid.
