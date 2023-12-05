// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/DeadlyFan.h"
#include "Components/StaticMeshComponent.h"

#include "MarineRunner/Interfaces/InteractInterface.h"

// Sets default values
ADeadlyFan::ADeadlyFan()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DeadlyFanBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeadlyFanBaseMesh"));
	RootComponent = DeadlyFanBaseMesh;
	DeadlyFanBaseMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);

	RotatingMeshAnchor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RotatingMeshAnchor"));
	RotatingMeshAnchor->SetupAttachment(RootComponent);

	RotatingMeshAnchor->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);

	DeadlyFanMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeadlyFanMesh"));
	DeadlyFanMesh->SetupAttachment(RotatingMeshAnchor);
	DeadlyFanMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DeadlyFanMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ADeadlyFan::BeginPlay()
{
	Super::BeginPlay();
	
	DeadlyFanMesh->OnComponentBeginOverlap.AddDynamic(this, &ADeadlyFan::OnFanMeshBeginOverlap);
}

// Called every frame
void ADeadlyFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateFan(DeltaTime);
}

void ADeadlyFan::OnFanMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) == false)
		return;

	IInteractInterface* InteractInterface = Cast<IInteractInterface>(OtherActor);
	if (InteractInterface == nullptr)
		return;

	InteractInterface->ApplyDamage(1000.f, 0.f, SweepResult, nullptr);
}

void ADeadlyFan::RotateFan(float Delta)
{
	FRotator CurrentFanRotation = DeadlyFanMesh->GetRelativeRotation();
	CurrentFanRotation.Yaw += Delta * RotateSpeed;
	DeadlyFanMesh->SetRelativeRotation(CurrentFanRotation);
}

