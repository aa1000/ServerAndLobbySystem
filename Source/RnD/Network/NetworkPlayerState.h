// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "NetworkPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RND_API ANetworkPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	
public:

	ANetworkPlayerState();

	virtual void BeginPlay() override;
	
	UPROPERTY(Replicated)
	bool bIsReady;

	/**
	*	Calls the game instace to get the lan player name if we are not on steam then sets it
	*/
	void ChangePlayerName();

	/**
	*	ChangePlayerName server side function if the caller is a client
	*/
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ChangePlayerName();
	void Server_ChangePlayerName_Implementation();
	FORCEINLINE bool Server_ChangePlayerName_Validate() { return true; }
};
