// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/DeadlyBox.h"
#include "Components/BoxComponent.h"

#include "MarineRunner/Interfaces/InteractInterface.h"


// Sets default values
ADeadlyBox::ADeadlyBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DeadlyBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DeadlyBox"));
	RootComponent = DeadlyBox;
	DeadlyBox->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	DeadlyBox->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ADeadlyBox::BeginPlay()
{
	Super::BeginPlay();
	
	DeadlyBox->OnComponentBeginOverlap.AddDynamic(this, &ADeadlyBox::KillEverythingBeginOverlap);

}

// Called every frame
void ADeadlyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeadlyBox::KillEverythingBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
		return;

	IInteractInterface* InteractInterface = Cast<IInteractInterface>(OtherActor);
	if (!InteractInterface)
		return;

	InteractInterface->ApplyDamage(DamageToApply, ImpulseForceToApply, SweepResult, nullptr);
}

