// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "TopDownShmupCharacter.h"
#include "TopDownShmup.h"

ATopDownShmupCharacter::ATopDownShmupCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Health = 100.0f;
	Dead = false;
}

void ATopDownShmupCharacter::BeginPlay()
{
	// Call base class BeginPlay
	Super::BeginPlay();

	// Spawn the weapon, if one was specified
	if (WeaponClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Need to set rotation like this because otherwise gun points down
			// NOTE: This should probably be a blueprint parameter
			FRotator Rotation(0.0f, 0.0f, -90.0f);

			// Spawn the Weapon
			MyWeapon = World->SpawnActor<AWeapon>(WeaponClass, FVector::ZeroVector, Rotation, SpawnParams);
			if (MyWeapon)
			{
				// This is attached to "WeaponPoint" which is defined in the skeleton
				// NOTE: This should probably be a blueprint parameter
				MyWeapon->WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("WeaponPoint"));

				// set owner of the weapon 
				MyWeapon->MyPawn = this;
			}
		}
	}
}

// call on start fire on weapon
void ATopDownShmupCharacter::OnStartFire()
{
	// check if weapon is valid
	if (MyWeapon)
	{
		MyWeapon->OnStartFire();
	}
}

// call on stop fire on weapon
void ATopDownShmupCharacter::OnStopFire()
{
	// check if weapon is valid
	if (MyWeapon)
	{
		MyWeapon->OnStopFire();
	}
}


float ATopDownShmupCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(4, 1.0f, FColor::Red, FString::Printf(TEXT("Player hit! Current health : %f / 100"), Health));
		}

		// Reduce health points
		Health -= ActualDamage;
		if (Health < 0.0f)
		{
			// We're dead
			Dead = true;
			SetCanBeDamaged(false); // Don't allow further damage
			OnStopFire(); // stop firing when dead
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->SetIgnoreLookInput(true);
				PlayerController->SetIgnoreMoveInput(true);
			}

			GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ATopDownShmupCharacter::StartDeath, PlayAnimMontage(DeathAnim) - 0.25f, false);
		}
	}

	return ActualDamage;
}

bool ATopDownShmupCharacter::IsDead()
{
	return Dead;
}

void ATopDownShmupCharacter::StartDeath()
{
	StopAnimMontage(DeathAnim); // stop animation
	GetMesh()->Deactivate(); // deactivate skeletal mesh
	GetWorldTimerManager().ClearTimer(DeathTimerHandle);
}