// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Quests/DialogueWithSubtitles.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DialogueSubtitlesWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

ADialogueWithSubtitles::ADialogueWithSubtitles()
{
	PrimaryActorTick.bCanEverTick = true;

	ActivateDialogueBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Activate Dialogue Box"));
	RootComponent = ActivateDialogueBox;
	ActivateDialogueBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	ActivateDialogueBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ADialogueWithSubtitles::BeginPlay()
{
	Super::BeginPlay();
	
	ActivateDialogueBox->OnComponentBeginOverlap.AddDynamic(this, &ADialogueWithSubtitles::OnActivateDialogueBoxBeginOverlap);
}

void ADialogueWithSubtitles::OnActivateDialogueBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bWasActivated)
		return;

	bWasActivated = true;

	FTimerHandle DelayDialogueHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayDialogueHandle, this, &ADialogueWithSubtitles::StartDialogue, StartDialogueDelay, false);
}

void ADialogueWithSubtitles::StartDialogue()
{
	if (IsValid(DialogueSound))
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DialogueSound, GetActorLocation());
	else
		UE_LOG(LogTemp, Warning, TEXT("DialogueSound is nullptr in DialogueWithSubtitles actor!"));

	if (!GetIsSubtitlesEnabled())
		return;

	bool bSpawned = SpawnDialogueWidget();
	if (!bSpawned)
		return;

	ChangeSubtitles();
}

bool ADialogueWithSubtitles::SpawnDialogueWidget()
{
	if (!IsValid(DialogueWidgetClass))
		return false;

	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController))
		return false;

	SpawnedDialogueWidget = Cast<UDialogueSubtitlesWidget>(CreateWidget(PlayerController, DialogueWidgetClass));
	if (!IsValid(SpawnedDialogueWidget))
		return false;

	SpawnedDialogueWidget->AddToViewport();

	return true;
}

void ADialogueWithSubtitles::ChangeSubtitles()
{
	if (!IsValid(SpawnedDialogueWidget))
		return;

	if (CurrentDialogueSubtitles == AllSubtitlesForDialogue.Num())
	{
		SpawnedDialogueWidget->ShowWidget(false);
		return;
	}

	SpawnedDialogueWidget->ChangeCurrentSubtitles(AllSubtitlesForDialogue[CurrentDialogueSubtitles].CurrentSubtitles);
	GetWorld()->GetTimerManager().SetTimer(ChangeSubtitlesHandle, this, &ADialogueWithSubtitles::ChangeSubtitles, AllSubtitlesForDialogue[CurrentDialogueSubtitles].TimeOnScreen, false);

	CurrentDialogueSubtitles++;
}

bool ADialogueWithSubtitles::GetIsSubtitlesEnabled()
{
	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(GameInstance))
		return false;

	float SubtitlesEnabledValue = GameInstance->FindSavedValueAccordingToName(SubtitlesEnabledSavedName);

	return SubtitlesEnabledValue == 0 ? false : true;
}

