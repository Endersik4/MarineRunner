// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/Obstacles/DeadlyFan.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/DamageInterface.h"

ADeadlyFan::ADeadlyFan()
{
	PrimaryActorTick.bCanEverTick = true;

	DeadlyFanBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeadlyFanBaseMesh"));
	RootComponent = DeadlyFanBaseMesh;
	DeadlyFanBaseMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);

	SocketMeshToRotate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SocketMeshToRotate"));
	SocketMeshToRotate->SetupAttachment(RootComponent);

	SocketMeshToRotate->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);

	DeadlyFanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeadlyFanMesh"));
	DeadlyFanMesh->SetupAttachment(SocketMeshToRotate);
	DeadlyFanMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DeadlyFanMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

void ADeadlyFan::BeginPlay()
{
	Super::BeginPlay();
	
	DeadlyFanMesh->OnComponentBeginOverlap.AddDynamic(this, &ADeadlyFan::OnFanMeshBeginOverlap);

	if (IsValid(FanSound))
		SpawnedFanSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FanSound, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("Fan Sound is nullptr in DeadlyFan!"));
}

// Called every frame
void ADeadlyFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateFan(DeltaTime);
}

void ADeadlyFan::OnFanMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
		return;

	IDamageInterface* InteractInterface = Cast<IDamageInterface>(OtherActor);
	if (!InteractInterface)
		return;

	InteractInterface->ApplyDamage(DamageToApply, ImpulseForceToApply, SweepResult, nullptr, EWT_None);
}

void ADeadlyFan::RotateFan(float Delta)
{
	FRotator CurrentFanRotation = DeadlyFanMesh->GetRelativeRotation();
	CurrentFanRotation.Yaw += Delta * RotateSpeed;
	DeadlyFanMesh->SetRelativeRotation(CurrentFanRotation);
}

