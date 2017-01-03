// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "NetworkSign.h"
#include "Network/NetworkPlayerController.h"



ANetworkSign::ANetworkSign()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sign Mesh"));
	Mesh->SetupAttachment(RootComponent);

	TextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text Render"));
	TextRender->SetupAttachment(RootComponent);
	
	bReplicates = true;
	
}

void ANetworkSign::Use(APawn * Caller)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, TEXT("Use called"));
	ANetworkPlayerController* PC = Cast<ANetworkPlayerController>(Caller->GetController());

	if (PC)
		PC->ClientShowWidget();
}

void ANetworkSign::OnRep_TextChange()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, TEXT("OnRep_TextCHange"));
	TextRender->SetText(SignText);
}

void ANetworkSign::ChangeSignText(FText NewText)
{
	if (Role == ROLE_Authority)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, NewText.ToString());
		SignText = NewText;
		TextRender->SetText(SignText);
	}
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Should Call Server"));
	else
		ServerChangeText(NewText);
}

void ANetworkSign::ServerChangeText_Implementation(const FText & NewText)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, TEXT("Should Call Server"));
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Text Should be") + NewText.ToString() );
	ChangeSignText(NewText);
	
}

void ANetworkSign::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Tell the engine to call the OnRep_Health and OnRep_BombCount each time
	//a variable changes
	DOREPLIFETIME(ANetworkSign, SignText);
}
