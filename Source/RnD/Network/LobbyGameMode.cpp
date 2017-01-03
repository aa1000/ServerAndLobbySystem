// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "NetworkPlayerState.h"




void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ALobbyPlayerController* JoiningPlayer = Cast<ALobbyPlayerController>(NewPlayer);

	//if the joining player is a lobby player controller, add him to a list of connected Players
	if (JoiningPlayer)
		ConnectedPlayers.Add(JoiningPlayer);
}

void ALobbyGameMode::Logout(AController* ExitingPlayer)
{
	Super::Logout(ExitingPlayer);

	//update the ConnectedPlayers Array and the PlayerList in the lobby whenever a player leaves
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(ExitingPlayer);
	if (LobbyPlayerController)
	{
		ConnectedPlayers.Remove(LobbyPlayerController);
		UpdatePlayerList();
	}
	
}

void ALobbyGameMode::ProdcastChatMessage(const FText & ChatMessage)
{
	//call all the connected players and pass in the chat message
	for (ALobbyPlayerController* Player : ConnectedPlayers)
		Player->Client_ReceiveChatMessage(ChatMessage);
}

void ALobbyGameMode::KickPlayer(int32 PlayerIndex)
{
	//call the player to make him destroy his session and leave game
	ConnectedPlayers[PlayerIndex]->Client_GotKicked();
}

void ALobbyGameMode::UpdatePlayerList()
{
	//Epmty the PlayerInfo Array to re-populate it
	PlayerInfoArray.Empty();

	//get all the player info from all the connected players
	for (ALobbyPlayerController* Player : ConnectedPlayers)
	{
		//temporary LobbyPlayerInfo var to hold the player info
		FLobbyPlayerInfo TempLobbyPlayerInfo;

		ANetworkPlayerState* NetworkedPlayerState = Cast<ANetworkPlayerState>(Player->PlayerState);
		if (NetworkedPlayerState)
			TempLobbyPlayerInfo.bPlayerReadyState = NetworkedPlayerState->bIsReady;
		else
			TempLobbyPlayerInfo.bPlayerReadyState = false;

		TempLobbyPlayerInfo.PlayerName = Player->PlayerState->PlayerName;
		PlayerInfoArray.Add(TempLobbyPlayerInfo);
	}

	//call all the players to make them update and pass in the player info array
	for (ALobbyPlayerController* Player : ConnectedPlayers)
		Player->Client_UpdatePlayerList(PlayerInfoArray);
}

void ALobbyGameMode::StartGameFromLobby()
{
	GetWorld()->ServerTravel(GameMapName);
}

bool ALobbyGameMode::IsAllPlayerReady() const
{
	for (ALobbyPlayerController* Player : ConnectedPlayers)
	{
		ANetworkPlayerState* NetworkedPlayerState = Cast<ANetworkPlayerState>(Player->PlayerState);
		if (NetworkedPlayerState)
			if (!NetworkedPlayerState->bIsReady)
				return false;
	}
	return true;
}


void ALobbyGameMode::PlayerRequestUpdate()
{
	UpdatePlayerList();
}


