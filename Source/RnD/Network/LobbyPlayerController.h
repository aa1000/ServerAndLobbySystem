// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RND_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

//		UPROPERTY(Replicated)
//		bool bIsReady;

public:

		//ALobbyPlayerController();
		

		virtual void BeginPlay() override;
		
		/**
		*	called from UMG to send a chat message
		*	@Param	ChatMessage		The passed chat message
		*/
		UFUNCTION(BlueprintCallable, Category = "Network|Chat")
		void SendChatMessage(const FText &ChatMessage);


		/**
		*	server side function if the caller is a client
		*	@Param	ChatMessage		The passed chat message
		*/
		UFUNCTION(Server, Unreliable, WithValidation)
		void Server_SendChatMessage(const FText & ChatMessage);
		void Server_SendChatMessage_Implementation(const FText & ChatMessage);
		FORCEINLINE bool Server_SendChatMessage_Validate(const FText & ChatMessage) { return true; }


		
		/**
		*	called from GameMode when it prodcasts all the chat messages to all players
		*	@Param	ChatMessage		The passed chat message
		*/
		UFUNCTION(Client, Unreliable)
		void Client_ReceiveChatMessage(const FText &ChatMessage);
		void Client_ReceiveChatMessage_Implementation(const FText &ChatMessage);

		/**
		*	calls UMG to add the incoming text message to the Chatbox
		*	@Param	ChatMessage		The passed chat message
		*/
		UFUNCTION(BlueprintImplementableEvent, Category = "Network|Chat")
		void ReceiveChatMessage(const FText &ChatMessage);

		/**
		*	called to kick a certain player by their index in the GameMode ConnectedPlayers Array
		*	@Param		PlayerIndex		player index in the GameMode ConnectedPlayers Array
		*/
		UFUNCTION(BlueprintCallable, Category = "Network|Lobby")
		void KickPlayer(int32 PlayerIndex);

		/**
		*	called from the game mode when a player is kicked to make him destroy his session and leave
		*/
		UFUNCTION(Client, Reliable)
		void Client_GotKicked();
		void Client_GotKicked_Implementation();

		/**
		*	called from the GameMode when the players need to update the PlayerList in UMG
		*	@Param	PlayerInfoArray		the player info array passed in from the server
		*/
		UFUNCTION(Client, Reliable)
		void Client_UpdatePlayerList(const TArray<FLobbyPlayerInfo>& PlayerInfoArray);
		void Client_UpdatePlayerList_Implementation(const TArray<FLobbyPlayerInfo>& PlayerInfoArray);

		/**
		*	Calls UMG to update the PlayerList
		*	@Param	PlayerInfoArray		the player info array passed in from the server
		*/
		UFUNCTION(BlueprintImplementableEvent, Category = "Network|PlayerList")
		void UpdateUMGPlayerList(const TArray<struct FLobbyPlayerInfo>& PlayerInfoArray);


		/**
		*	called from the player on begain play to request the player info array from the server
		*/
		void RequestServerPlayerListUpdate();

		/**
		*	server side RequestServerPlayerListUpdate
		*/
		UFUNCTION(Server, Reliable, WithValidation)
		void Server_RequestServerPlayerListUpdate();
		void Server_RequestServerPlayerListUpdate_Implementation();
		FORCEINLINE bool Server_RequestServerPlayerListUpdate_Validate() { return true; }


		UFUNCTION(BlueprintCallable, Category = "Network|Lobby")
		void SetIsReadyState(bool NewReadyState);

		UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetIsReadyState(bool NewReadyState);
		void Server_SetIsReadyState_Implementation(bool NewReadyState);
		FORCEINLINE bool Server_SetIsReadyState_Validate(bool NewReadyState) { return true; }

		UFUNCTION(BlueprintPure, Category = "Network|Lobby")
		bool CanGameStart() const;

		UFUNCTION(BlueprintCallable, Category = "Network|Lobby")
		void StartGame();
	
};
