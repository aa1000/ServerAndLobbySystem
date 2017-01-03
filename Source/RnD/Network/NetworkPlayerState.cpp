// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "NetworkPlayerState.h"
#include "NWGameInstance.h"


ANetworkPlayerState::ANetworkPlayerState()
{
	bIsReady = false;
}

void ANetworkPlayerState::BeginPlay()
{

	Super::BeginPlay();

	//change the player name to the name in the game insance if it is on lan
	ChangePlayerName();

}

void ANetworkPlayerState::ChangePlayerName()
{
	//check if this has authority
	if (Role == ROLE_Authority)
	{
		//try to get the NetworkedGameInstance
		UNWGameInstance* NWGameInstance = Cast<UNWGameInstance>(GetWorld()->GetGameInstance());

		//if the game instance is not null, get the player name from it
		if (NWGameInstance)
		{
			FString ActualPlayerName = NWGameInstance->GetPlayerName();
			//if the string is empty that means we are on steam so no need to change the name
			if (ActualPlayerName != "")
				//if we are on lan set the player name to the name we got from game instance
				SetPlayerName(ActualPlayerName);
		}

	}
	else //if the player doesn't have authority call the serverside to call this function again
		Server_ChangePlayerName();
}

void ANetworkPlayerState::Server_ChangePlayerName_Implementation()
{
	//call the change player name fucntion from server side
	ChangePlayerName();
}


void ANetworkPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetworkPlayerState, bIsReady);
}