// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/WallrunOnButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Curves/CurveVector.h"

// Sets default values
AWallrunOnButton::AWallrunOnButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMeshComponent"));
	RootComponent = BaseMeshComponent;

	SocketRotateMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SocketRotateMeshComponent"));
	SocketRotateMeshComponent->SetupAttachment(RootComponent);

	MeshToRotateComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshToRotateComponent"));
	MeshToRotateComponent->SetupAttachment(SocketRotateMeshComponent);
	
	ActivateRotateBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivateRotateBoxComponent"));
	ActivateRotateBoxComponent->SetupAttachment(RootComponent);
	ActivateRotateBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivateRotateBoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);

	ActivateMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Activate Mesh Component"));
	ActivateMeshComponent->SetupAttachment(ActivateRotateBoxComponent);

	ResetCurrentTimeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ResetCurrentTimeText"));
	ResetCurrentTimeText->SetupAttachment(ActivateRotateBoxComponent);
}

// Called when the game starts or when spawned
void AWallrunOnButton::BeginPlay()
{
	Super::BeginPlay();

	CurrentResetSecond = FMath::RoundToInt(ResetToInitialRotationTime);
	ResetCurrentTimeText->SetText(FText::AsNumber(CurrentResetSecond));

	ActivateRotateBoxComponent->OnComponentHit.AddDynamic(this, &AWallrunOnButton::OnActivateRotateBoxHit);

	ActiveDynamicMaterial = UMaterialInstanceDynamic::Create(ActivateMeshComponent->GetMaterial(IndexForMaterialToChange), this);
	ActivateMeshComponent->SetMaterial(IndexForMaterialToChange, ActiveDynamicMaterial);

	InitialSocketRotation = SocketRotateMeshComponent->GetRelativeRotation();
}

// Called every frame
void AWallrunOnButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWasRotated == true)
	{
		RotateMeshTimeline.TickTimeline(DeltaTime);
	}
}

void AWallrunOnButton::OnActivateRotateBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& OutHit)
{
	if (bWasRotated == true)
		return;

	bWasRotated = true;
	StartRotateMeshTimeline();
	ActiveDynamicMaterial->SetVectorParameterValue(FName("Color"), ActiveMaterialColor);
}

void AWallrunOnButton::StartRotateMeshTimeline()
{
	FOnTimelineVector TimelineProgress;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	TimelineProgress.BindUFunction(this, FName("OnTimelineCallback"));
	RotateMeshTimeline.AddInterpVector(RelativeRotationCurve, TimelineProgress);

	onTimelineFinishedCallback.BindUFunction(this, FName("OnTimelineFinished"));
	RotateMeshTimeline.SetTimelineFinishedFunc(onTimelineFinishedCallback);
	
	RotateMeshTimeline.PlayFromStart();
}

void AWallrunOnButton::OnTimelineCallback(FVector NewRotation)
{
	FRotator NewRelativeRotation(NewRotation.Y, NewRotation.Z, NewRotation.X);
	SocketRotateMeshComponent->SetRelativeRotation(NewRelativeRotation);
}

void AWallrunOnButton::OnTimelineFinished()
{
	CurrentResetSecond = FMath::RoundToInt(ResetToInitialRotationTime);
	ResetCurrentTimeText->SetText(FText::AsNumber(CurrentResetSecond));

	if (bResetingRotation == true)
	{
		bWasRotated = false;
		bResetingRotation = false;

		return;
	}

	GetWorldTimerManager().SetTimer(ResetToInitialRotationHandle, this, &AWallrunOnButton::ResetRotateMeshTimeline, ResetToInitialRotationTime, false);
	GetWorldTimerManager().SetTimer(DisplayResetTimeHandle, this, &AWallrunOnButton::ResetTimeSeconds, 1.f, true);
}

void AWallrunOnButton::ResetRotateMeshTimeline()
{
	bResetingRotation = true;
	RotateMeshTimeline.ReverseFromEnd();
	ActiveDynamicMaterial->SetVectorParameterValue(FName("Color"), NotActiveMaterialColor);

}

void AWallrunOnButton::ResetTimeSeconds()
{
	CurrentResetSecond--;
	ResetCurrentTimeText->SetText(FText::AsNumber(CurrentResetSecond));

	if (CurrentResetSecond <= 0)
		GetWorldTimerManager().ClearTimer(DisplayResetTimeHandle);
}

void AWallrunOnButton::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AWallrunOnButton::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

void AWallrunOnButton::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	if (bWasRotated == false)
		return;

	ActiveDynamicMaterial->SetVectorParameterValue(FName("Color"), NotActiveMaterialColor);
	bResetingRotation = false;
	bWasRotated = false;
	GetWorldTimerManager().ClearTimer(DisplayResetTimeHandle);
	GetWorldTimerManager().ClearTimer(ResetToInitialRotationHandle);

	CurrentResetSecond = FMath::RoundToInt(ResetToInitialRotationTime);
	ResetCurrentTimeText->SetText(FText::AsNumber(CurrentResetSecond));
	RotateMeshTimeline.Stop();

	SocketRotateMeshComponent->SetRelativeRotation(InitialSocketRotation);
}