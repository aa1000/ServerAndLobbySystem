// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "NetworkPlayerController.h"
#include "Items/NetworkSign.h"

void ANetworkPlayerController::ClientShowWidget_Implementation()
{
	WidgetOnUse(this);
}

void ANetworkPlayerController::ChangeSignText(FText NewText, ANetworkSign* Sign)
{
	if (Role == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, NewText.ToString());
		Sign->ChangeSignText(NewText);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Should Call Server"));
		ServerChangeText(NewText, Sign);
	}
}

void ANetworkPlayerController::ServerChangeText_Implementation(const FText & NewText, ANetworkSign* Sign)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, TEXT("Should Call Server"));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Text Should be") + NewText.ToString());
	ChangeSignText(NewText, Sign);
}
