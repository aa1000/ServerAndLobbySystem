// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RND_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:


	UFUNCTION(BlueprintImplementableEvent, Category = "NetworkSign")
	void WidgetOnUse(APlayerController* PC);

public:
		UFUNCTION(Client, Reliable)
		void ClientShowWidget();

		UFUNCTION(BlueprintCallable, Category = "Networked Sign")
		void ChangeSignText(FText NewText, ANetworkSign* Sign);

		UFUNCTION(Server, Reliable, WithValidation)
		void ServerChangeText(const FText & NewText, ANetworkSign* Sign);

		/** Contains the actual implementation of the ServerChangeText function */
		void ServerChangeText_Implementation(const FText & NewText, ANetworkSign* Sign);

		/** Validates the client. If the result is false the client will be disconnected */
		FORCEINLINE bool ServerChangeText_Validate(const FText & NewText, ANetworkSign* Sign) { return true; }
	
};
