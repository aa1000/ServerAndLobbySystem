// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */


USTRUCT(BlueprintType)
struct FLobbyPlayerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
	bool bPlayerReadyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Player Info")
		FString PlayerName;
};

UCLASS()
class RND_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()


	TArray<struct FLobbyPlayerInfo> PlayerInfoArray;

	UPROPERTY(EditDefaultsOnly)
	FString GameMapName;

public:

	TArray<class ALobbyPlayerController*> ConnectedPlayers;

	//called when a new player joins the game
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* ExitingPlayer) override;

	//called from player controller when he sends a chat message
	void ProdcastChatMessage(const FText & ChatMessage);

	//called from the host to kick a player
	void KickPlayer(int32 PlayerIndex);

	void PlayerRequestUpdate();
	
	void UpdatePlayerList();

	void StartGameFromLobby();

	bool IsAllPlayerReady() const;
};
