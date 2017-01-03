// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/UsableItem.h"
#include "Components/TextRenderComponent.h"
#include "NetworkSign.generated.h"

/**
 * 
 */
UCLASS()
class RND_API ANetworkSign : public AUsableItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* TextRender;

	UPROPERTY(EditDefaultsOnly, Transient, ReplicatedUsing = OnRep_TextChange, Category = Stats)
	FText SignText;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "NetworkSign")
	void WidgetOnUse(APlayerController* PC);
	
public:

	ANetworkSign();

	virtual void Use(APawn* Caller) override;

	UFUNCTION()
	void OnRep_TextChange();

	UFUNCTION(BlueprintCallable, Category = "Networked Sign")
	FORCEINLINE FText GetSignText() const { return SignText; }

	UFUNCTION(BlueprintCallable, Category = "Networked Sign")
	void ChangeSignText(FText NewText);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeText(const FText & NewText);

	/** Contains the actual implementation of the ServerChangeText function */
	void ServerChangeText_Implementation(const FText & NewText);

	/** Validates the client. If the result is false the client will be disconnected */
	FORCEINLINE bool ServerChangeText_Validate(const FText & NewText) { return true; }


	
};
