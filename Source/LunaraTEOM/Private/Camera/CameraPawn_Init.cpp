#include "Camera/CameraPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"

namespace
{
	constexpr float KINDA_SMALL_NUMBER_CM = 1.0e-3f;

	inline bool IsVectorFinite(const FVector& V)
	{
		return FMath::IsFinite(V.X) && FMath::IsFinite(V.Y) && FMath::IsFinite(V.Z);
	}
}

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SceneRoot);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = false;
	SpringArm->TargetArmLength = 1200.0f;
	SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw  = false;
	bUseControllerRotationRoll = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	DefaultInputMappingPath = TEXT("/Game/Input/IMC_CameraPawn.IMC_CameraPawn");
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(SceneRoot, TEXT("CameraPawn %s missing SceneRoot"), *GetName());
	ensureMsgf(SpringArm, TEXT("CameraPawn %s missing SpringArm"), *GetName());
	ensureMsgf(Camera,    TEXT("CameraPawn %s missing Camera"),    *GetName());

	if (MinPitch > MaxPitch)
	{
		UE_LOG(LogCameraPawn, Warning, TEXT("BeginPlay: MinPitch %.2f > MaxPitch %.2f. Swapping values to preserve clamp."),
			MinPitch, MaxPitch);
		Swap(MinPitch, MaxPitch);
	}

	if (SpringArm)
	{
		SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength, MinArmLength, MaxArmLength);
	}

	ResolveInputAssets();
	InitializeInputMapping();
}

void ACameraPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ResolveInputAssets();
	InitializeInputMapping();
}

void ACameraPawn::PawnClientRestart()
{
	Super::PawnClientRestart();
	ResolveInputAssets();
	InitializeInputMapping();
}

bool ACameraPawn::ResolveInputAssets()
{
	bool bMappingResolved = true;

	if (!DefaultInputMapping)
	{
		if (DefaultInputMappingPath.IsValid())
		{
			if (UInputMappingContext* LoadedContext = Cast<UInputMappingContext>(DefaultInputMappingPath.TryLoad()))
			{
				DefaultInputMapping = LoadedContext;
				UE_LOG(LogCameraPawn, Verbose, TEXT("ResolveInputAssets: Loaded mapping context from %s"),
					*DefaultInputMappingPath.ToString());
			}
			else
			{
				UE_LOG(LogCameraPawn, Warning, TEXT("ResolveInputAssets: Failed to load mapping context from %s"),
					*DefaultInputMappingPath.ToString());
				bMappingResolved = false;
			}
		}
		else
		{
			UE_LOG(LogCameraPawn, Verbose, TEXT("ResolveInputAssets: DefaultInputMappingPath not set; expecting mapping via pointer."));
			bMappingResolved = false;
		}
	}

	auto LoadAction = [](TObjectPtr<UInputAction>& ActionPtr, const FSoftObjectPath& Path, const TCHAR* Label)
	{
		if (!ActionPtr && Path.IsValid())
		{
			if (UInputAction* LoadedAction = Cast<UInputAction>(Path.TryLoad()))
			{
				ActionPtr = LoadedAction;
				UE_LOG(LogCameraPawn, Verbose, TEXT("ResolveInputAssets: Loaded %s from %s"), Label, *Path.ToString());
			}
			else
			{
				UE_LOG(LogCameraPawn, Warning, TEXT("ResolveInputAssets: Failed to load %s from %s"), Label, *Path.ToString());
			}
		}
	};

	LoadAction(ZoomAction,  ZoomActionPath,  TEXT("ZoomAction"));
	LoadAction(OrbitAction, OrbitActionPath, TEXT("OrbitAction"));
	LoadAction(PanAction,   PanActionPath,   TEXT("PanAction"));

	return bMappingResolved && DefaultInputMapping != nullptr;
}

void ACameraPawn::InitializeInputMapping()
{
	if (!ResolveInputAssets())
	{
		UE_LOG(LogCameraPawn, Warning, TEXT("InitializeInputMapping skipped: mapping context unresolved."));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogCameraPawn, Warning, TEXT("InitializeInputMapping failed: no controller."));
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogCameraPawn, Warning, TEXT("InitializeInputMapping failed: controller has no local player."));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogCameraPawn, Warning, TEXT("InitializeInputMapping failed: EnhancedInput subsystem unavailable."));
		return;
	}

	if (Subsystem->HasMappingContext(DefaultInputMapping))
	{
		Subsystem->RemoveMappingContext(DefaultInputMapping);
		UE_LOG(LogCameraPawn, VeryVerbose, TEXT("InitializeInputMapping: Removed existing mapping %s before re-adding."),
			*GetNameSafe(DefaultInputMapping));
	}

	Subsystem->AddMappingContext(DefaultInputMapping, InputMappingPriority);
	UE_LOG(LogCameraPawn, Verbose, TEXT("InitializeInputMapping: Added %s with priority %d for %s."),
		*GetNameSafe(DefaultInputMapping), InputMappingPriority, *GetName());
}
