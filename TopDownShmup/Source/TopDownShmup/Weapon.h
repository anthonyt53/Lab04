// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapon.generated.h"

UCLASS()
class TOPDOWNSHMUP_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	USkeletalMeshComponent* WeaponMesh;

	// gun control
	virtual void OnStartFire();
	virtual void OnStopFire();
	
	// gun firing sound
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireLoopSound;
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireFinishSound;

	// muzzle flash
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFX;

	// my pawn
	APawn* MyPawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// gun firing sound
	UPROPERTY(Transient)
	UAudioComponent* FireAC;
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	// muzzle flash
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzleFlash;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
