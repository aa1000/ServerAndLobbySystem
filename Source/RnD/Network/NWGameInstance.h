// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "NWGameInstance.generated.h"


#define SETTING_SERVER_NAME FName(TEXT("SERVERNAMEKEY"))
#define SETTING_SERVER_IS_PROTECTED FName(TEXT("SERVERSERVERISPASSWORDPROTECTEDKEY"))
#define SETTING_SERVER_PROTECT_PASSWORD FName(TEXT("SERVERPROTECTPASSWORDKEY"))

/**
 * 
 */


//A custom struct to be able to access the Session results in blueprint
USTRUCT(BlueprintType)
struct FCustomBlueprintSessionResult
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		FString SessionName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		bool bIsLan;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 CurrentNumberOfPlayers;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 MaxNumberOfPlayers;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		bool bIsPasswordProtected;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		FString SessionPassword;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Blueprint Session Result")
		int32 Ping;
};


//stolen from Advanced Session Plugin cuz I can't find any other way to put FUniqueNetId in a struct
USTRUCT(BlueprintType)
struct FBPUniqueNetId
{
	GENERATED_USTRUCT_BODY()

private:
	bool bUseDirectPointer;


public:
	TSharedPtr<const FUniqueNetId> UniqueNetId;
	const FUniqueNetId * UniqueNetIdPtr;

	void SetUniqueNetId(const TSharedPtr<const FUniqueNetId> &ID)
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
		UniqueNetId = ID;
	}

	void SetUniqueNetId(const FUniqueNetId *ID)
	{
		bUseDirectPointer = true;
		UniqueNetIdPtr = ID;
	}

	bool IsValid() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr)
		{
			return true;
		}
		else if (UniqueNetId.IsValid())
		{
			return true;
		}
		else
			return false;

	}

	const FUniqueNetId* GetUniqueNetId() const
	{
		if (bUseDirectPointer && UniqueNetIdPtr != nullptr)
		{
			// No longer converting to non const as all functions now pass const UniqueNetIds
			return /*const_cast<FUniqueNetId*>*/(UniqueNetIdPtr);
		}
		else if (UniqueNetId.IsValid())
		{
			return UniqueNetId.Get();
		}
		else
			return nullptr;
	}

	FBPUniqueNetId()
	{
		bUseDirectPointer = false;
		UniqueNetIdPtr = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FSteamFriendInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
	UTexture2D* PlayerAvatar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steam Friend Info")
	FBPUniqueNetId PlayerUniqueNetID;
};


UCLASS()
class RND_API UNWGameInstance : public UGameInstance
{
	GENERATED_BODY()


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Online")
	int32 MinServerNameLength;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Online")
	int32 MaxServerNameLength;
	

private:
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;


	/** Delegate for receiving invite */
	//FOnSessionInviteReceivedDelegate OnSessionInviteReceivedDelegate;
	/** Handle to registered delegate for receiving an invite */
	//FDelegateHandle OnSessionInviteReceivedDelegateHandle;


	/** Delegate for accepting invite */
	FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;
	/** Handle to registered delegate for accepting an invite */
	FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;

	/** Delegate for reading the friendlist*/
	FOnReadFriendsListComplete FriendListReadCompleteDelegate;
	
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//store the max number of players in a session whenever we create of join a session
	int32 MaxPlayersinSession;

	//store the chosen search result for later use
	FOnlineSessionSearchResult ChosenSearchResult;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FName LobbyMapName;

	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FName MainMenuMap;



	/**
	*  Function to host a game!
	*
	*  @Param    UserID      User that started the request
	*  @Param    SessionName    Name of the Session
	*  @Param	 ServerName		Name of the Server
	*  @Param    bIsLAN      Is this is LAN Game?
	*  @Param    bIsPresence    "Is the Session to create a presence Session"
	*  @Param    MaxNumPlayers          Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString ServerName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPasswordProtected, FString SessionPassword);


	/**
	*  Function fired when a session create request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);


	/**
	*  Function fired when a session start request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/**
	*  Find an online session
	*
	*  @param UserId user that initiated the request
	*  @param SessionName name of session this search will generate
	*  @param bIsLAN are we searching LAN matches
	*  @param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence);


	/**
	*  Delegate fired when a session search query has completed
	*
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/**
	*  Joins a session via a search result
	*
	*  @param SessionName name of session
	*  @param SearchResult Session to join
	*
	*  @return bool true if successful, false otherwise
	*/

	bool JoinASession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	//overloaded to accept invites
	/**
	*  Joins a session via a search result
	*
	*  @param SessionName name of session
	*  @param SearchResult Session to join
	*
	*  @return bool true if successful, false otherwise
	*/

	bool JoinASession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/**
	*  Delegate fired when a session join request has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/**
	*  Delegate fired when a destroying an online session has completed
	*
	*  @param SessionName the name of the session this callback is for
	*  @param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// Steam never calls this since the invite is sent through the steam chat and you can only respond to it there by accepting or neglicting
	/**
	* Delegate fired when a session invite is received
	* @param	InvitedPlayer	The UniqueNetId of the invited player
	* @param	InvitingPlayer	The inviting player
	* @param	AppID			AppIID of the game
	* @param	TheSessionInvitedTo		the session invited to
	*/
	//virtual void OnSessionInviteReceived(const FUniqueNetId & InvitedPlayer, const FUniqueNetId & InvitingPlayer, const FString & AppID, const FOnlineSessionSearchResult & TheSessionInvitedTo);


	/**
	* Delegate function fired when a session invite is accepted to join the session
	* @param    bWasSuccessful true if the async action completed without error, false if there was an error
	* @param	LocalUserNum	The Local user Number of the player who recived the invite
	* @param	InvitingPlayer	The inviting player
	* @param	TheSessionInvitedTo		the session invited to
	*/
	void OnSessionUserInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> InvitingPlayer, const FOnlineSessionSearchResult& TheSessionInvitedTo);

	/**
	* Delegate fired when the friend list request has been processed
	* @param	LocalUserNum		The local user id (UniqueNetId) of the requesting player
	* @param	bWasSuccessful		true if the async action completed without error, false if there was an error
	* @param	ListName			the friend list name
	* @param	ErrorString			if there is any errors
	*/
	void OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString);
	

public:
	
	//constructor
	UNWGameInstance();

	virtual void Init() override;

	//Lan player name to not use the Computer Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lan")
	FString LanPlayerName;

	/**
	 *	Called from blueprints to host a session and start game
	 *	@Param		ServerName		what name would appear to the players in the Server list
	 *	@Param		MaxNumPlayers		Max Number of Players who can join that session
	 *	@Param		bIsLan			Whether the session is lan or no
	 *  @Param		bIsPresence		whether the session uses presence or no (default true)
	 *  @Param		bIsPasswordProtected		Whether the session is protected by a password or no (default false)
	 *	@Param		SessionPassword		The Password of the ssession if it is protected by a password (default empty string)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Sessions")
	void StartOnlineGame(FString ServerName, int32 MaxNumPlayers, bool bIsLAN, bool bIsPresence = true, bool bIsPasswordProtected = false, FString SessionPassword = "");

	/**
	 *	Called from blueprints to find sessions then call and event to return the session results
	 *	@Param		bIsLAN		wehether the session is lan or no
	 *	@Param		bIsPresence		whether the session uses presence or no
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Sessions")
	void FindOnlineGames(bool bIsLAN, bool bIsPresence);

	/**
	 *	called from blueprints to join a session based on it's index in the returned session array
	 *	@Param	 SessionIndex	 The index of the session in the Session Search Results array
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Sessions")
	void JoinOnlineGame(int32 SessionIndex);

	/**
	 *	called from blueprints to destroy the session and leave game to main menu
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Sessions")
	void DestroySessionAndLeaveGame();


	UFUNCTION(BlueprintCallable, Category = "Network|Friends")
	void SendSessionInviteToFriend(APlayerController* InvitingPlayer, const FBPUniqueNetId & Friend);


	/**
	 *	called when the session search is complete to show the results in UMG
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Network|Sessions")
	void OnFoundSessionsCompleteUMG(const TArray<FCustomBlueprintSessionResult>& CustomSessionResults);


	/**
	 *	called to show an error message in UMG
	 *	@Param	ErrorMessage The Error meaage you want to show
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Errors")
	void ShowErrorMessage(const FText & ErrorMessage);


	/**
	 *	called to show an error message in UMG
	 *	@Param	ErrorMessage The Error meaage you want to show
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Network|Errors")
	void ShowErrorMessageUMG(const FText & ErrorMessage);


	/**
	* Check if steam is running
	* @retrun		whether the current online subsystem is steam or no
	*/
	UFUNCTION(BlueprintPure, Category = "Network|Friends")
	bool IsOnlineSubsystemSteam() const;


	/**
	 * called from the player state to get the player name
	 * @retrun		returns empty string if the player is on steam and retruns the LanPlayerName if he is on Lan
	 */
	FString GetPlayerName() const;

	/**
	 * gets the current session
	 */
	IOnlineSessionPtr GetSession() const;

	/**
	 * gets the max number of players in the session
	 * @return	max number of players in the session
	 */
	UFUNCTION(BlueprintPure, Category = "Network|Sessions")
	FORCEINLINE int32 GetSessionMaxPlayers() const { return MaxPlayersinSession; }


	/**
	 * gets the steam avatar of a player based on his UniqueNetId
	 * @Param		UniqueNetId		the UniqueNetId of the player you want to get his avatar
	 */
	UTexture2D* GetSteamAvatar(const FBPUniqueNetId UniqueNetId);

	/**
	 * called to get the list of steam friends a player has
	 * @Param		PlayerController		the player controller of the player asking for the friend list
	 * @Param		FriendsList				list of friends' info in  bluepritn wrapper structure
	 */
	UFUNCTION(BlueprintCallable, Category = "Network|Friends")
	void GetSteamFriendsList(APlayerController *PlayerController);

	/**
	 * Called from the delegate when getting the friend list request in completed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Network|Friends")
	void OnGetSteamFriendRequestCompleteUMG(const TArray<FSteamFriendInfo>& BPFriendsLists);

};
