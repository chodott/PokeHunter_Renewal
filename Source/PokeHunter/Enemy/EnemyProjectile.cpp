// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Collision
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	SetRootComponent(StaticMesh);
	StaticMesh->SetCollisionProfileName("WorldDynamic");

	//Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	StaticMesh->OnComponentHit.AddDynamic(this, &AEnemyProjectile::OnHit);
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ServerApplyDamage(OtherActor, Damage, GetActorForwardVector(), Hit, NULL, this, UDamageType::StaticClass());
}

void AEnemyProjectile::FireInDirection(const FVector& DirectionVec, const FVector& InitialPos, const FVector& EndPos)
{
	FVector Velocity = FVector::ZeroVector;
	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, Velocity, InitialPos, EndPos, GetWorld()->GetGravityZ(), 1.f))
	{

		ProjectileMovement->Velocity = Velocity * ProjectileMovement->InitialSpeed;

	}
}

void AEnemyProjectile::ServerApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	MultiApplyDamage(DamagedActor, DamageAmount, Direction, HitInfo, EventInstigator, DamageCauser, DamageTypeClass);
}

void AEnemyProjectile::MultiApplyDamage_Implementation(AActor* DamagedActor, int DamageAmount, FVector Direction, const FHitResult& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
{
	UGameplayStatics::ApplyPointDamage(DamagedActor, DamageAmount, Direction, HitInfo, NULL, DamageCauser, DamageTypeClass);
}


