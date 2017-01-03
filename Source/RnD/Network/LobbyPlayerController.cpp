// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "LobbyPlayerController.h"
#include "LobbyGameMode.h"
#include "NWGameInstance.h"
#include "NetworkPlayerState.h"
#include "LobbyGameState.h"

/*
ALobbyPlayerController::ALobbyPlayerController()
{
	bIsReady = true;
}
*/
void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//Timer handle to call the RequestServerPlayerListUpdate() later
	// the reason I use a timer is to give the PlayerState enough time to set up the player name
	FTimerHandle RequestServerPlayerListUpdateHanndle;
	//Call the server to request PlayerList Update after 0.01s
	GetWorld()->GetTimerManager().SetTimer(RequestServerPlayerListUpdateHanndle, this, &ALobbyPlayerController::RequestServerPlayerListUpdate, 0.05f, false);
}



void ALobbyPlayerController::SendChatMessage(const FText & ChatMessage)
{
	// if this is the server call the game mode to prodcast the Chat Message
	if (Role == ROLE_Authority)
	{
		ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			//Add the player's name to the Chat Message then send it to the server
			const FText OutChatMessage = FText::FromString(PlayerState->PlayerName + ": " + ChatMessage.ToString());
			GM->ProdcastChatMessage(OutChatMessage);
		}
	}
	else //else call the serverside function on this
		Server_SendChatMessage(ChatMessage);
}


void ALobbyPlayerController::Server_SendChatMessage_Implementation(const FText & ChatMessage)
{
	//if not the server call the server side function to 
	SendChatMessage(ChatMessage);
}

//called from the GameMode when it prodcast the chat message to all connected players
void ALobbyPlayerController::Client_ReceiveChatMessage_Implementation(const FText & ChatMessage)
{
	//call the Recive message function to show it on UMG
	ReceiveChatMessage(ChatMessage);
}

void ALobbyPlayerController::KickPlayer(int32 PlayerIndex)
{
	//if the player is the host, get the game mode and send it to kick the player from the game
	if (Role == ROLE_Authority)
	{
		ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->KickPlayer(PlayerIndex);
	}
}

//called from the game mode when the player is kicked by the host to make the player destroy his session and leave game
void ALobbyPlayerController::Client_GotKicked_Implementation()
{
	//get the game Instance to make the player destroy his session and leave game
	UNWGameInstance* NetworkedGameInstance = Cast<UNWGameInstance>(GetWorld()->GetGameInstance());
	if (NetworkedGameInstance)
	{
		//show the player that he got kicked in message in UMG
		NetworkedGameInstance->ShowErrorMessage(FText::FromString("You got kicked from the server"));
		//make the player call the game Instance to destroy his session
		NetworkedGameInstance->DestroySessionAndLeaveGame();
	}
}

//called from server and passes in an array of player infos
void ALobbyPlayerController::Client_UpdatePlayerList_Implementation(const TArray<FLobbyPlayerInfo>& PlayerInfoArray)
{
	UpdateUMGPlayerList(PlayerInfoArray);
}

//called fro mthe palyer on begin play to request the player info array fro mthe server
void ALobbyPlayerController::RequestServerPlayerListUpdate()
{
	// if this is the server call the game mode to request info
	if (Role == ROLE_Authority)
	{
		ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
			GM->PlayerRequestUpdate();
	}
	else //else call the serverside function on this
		Server_RequestServerPlayerListUpdate();
}


void ALobbyPlayerController::Server_RequestServerPlayerListUpdate_Implementation()
{
	//if not the server call the server side function
	RequestServerPlayerListUpdate();
}

void ALobbyPlayerController::SetIsReadyState(bool NewReadyState)
{
	if (Role == ROLE_Authority)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("RoleAuthority/ server called"));
		ANetworkPlayerState* NetworkedPlayerState = Cast<ANetworkPlayerState>(PlayerState);
		if (NetworkedPlayerState)
			NetworkedPlayerState->bIsReady = NewReadyState;
		RequestServerPlayerListUpdate();
	}
	else
		Server_SetIsReadyState(NewReadyState);

}

void ALobbyPlayerController::Server_SetIsReadyState_Implementation(bool NewReadyState)
{
	//bool PassedNewState = NewReadyState;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Role not Authority call server side"));
	SetIsReadyState(NewReadyState);
}

bool ALobbyPlayerController::CanGameStart() const
{
	if (Role == ROLE_Authority)
	{
		ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			return GM->IsAllPlayerReady();
	}
	return false;
}

void ALobbyPlayerController::StartGame()
{
	//if the player is the host, get the game mode and send it to start the game
	if (Role == ROLE_Authority)
	{
		ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->StartGameFromLobby();
	}
}