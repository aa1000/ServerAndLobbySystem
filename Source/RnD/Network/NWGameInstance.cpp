// Fill out your copyright notice in the Description page of Project Settings.

#include "RnD.h"
#include "NWGameInstance.h"

//we include the steam api here to be able to get the steam avatar
//refresh your visual studio files from editor after adding this to avoid weird redline errors
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

#include <steam/steam_api.h>

#pragma pop_macro("ARRAY_COUNT")

#endif


UNWGameInstance::UNWGameInstance()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnFindSessionsComplete);


	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UNWGameInstance::OnDestroySessionComplete);


	/**Bind Function for receving an invite*/
	//OnSessionInviteReceivedDelegate = FOnSessionInviteReceivedDelegate::CreateUObject(this, &UNWGameInstance::OnSessionInviteReceived);

	/**Bind Function for aceppting an invite*/
	OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &UNWGameInstance::OnSessionUserInviteAccepted);

	/** Bind the function for completing the friend list request*/
	FriendListReadCompleteDelegate = FOnReadFriendsListComplete::CreateUObject(this, &UNWGameInstance::OnReadFriendsListCompleted);

	LanPlayerName = "Player";
}


void UNWGameInstance::Init()
{
	Super::Init();
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can bind the accept delegate to it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//OnSessionInviteReceivedDelegateHandle = Sessions->AddOnSessionInviteReceivedDelegate_Handle(OnSessionInviteReceivedDelegate);
			//we bind the delagate for accepting an invite to the session interface so when you accept an invite, you can join the game.
			OnSessionUserInviteAcceptedDelegateHandle = Sessions->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
		}
	}
}


bool UNWGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FString ServerName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, bool bIsPasswordProtected, FString SessionPassword)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			/*
			Fill in all the Session Settings that we want to use.

			There are more with SessionSettings.Set(...);
			For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			MaxPlayersinSession = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			//setting a value in the FOnlineSessionSetting 's settings array
			SessionSettings->Set(SETTING_MAPNAME, LobbyMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			
			//Making a temporary FOnlineSessionSetting variable to hold the data we want to add to the FOnlineSessionSetting 's settings array
			FOnlineSessionSetting ExtraSessionSetting;
			ExtraSessionSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;

			//setting the temporary data to the ServerName we got from UMG
			ExtraSessionSetting.Data = ServerName;
			
			//adding the Server Name value in the FOnlineSessionSetting 's settings array using the key defined in header
			//the key can be any FNAME but we define it to avoid mistakes
			SessionSettings->Settings.Add(SETTING_SERVER_NAME, ExtraSessionSetting);

			//setting the temporary data to the bIsPasswordProtected we got from UMG
			ExtraSessionSetting.Data = bIsPasswordProtected;
			//adding the bIsPasswordProtected value in the FOnlineSessionSetting 's settings array using the key defined in header
			SessionSettings->Settings.Add(SETTING_SERVER_IS_PROTECTED, ExtraSessionSetting);

			
			//setting the temporary data to the Password we got from UMG
			ExtraSessionSetting.Data = SessionPassword;
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, *ExtraSessionSetting.Data.ToString());
			//adding the Password value in the FOnlineSessionSetting 's settings array using the key defined in header
			SessionSettings->Settings.Add(SETTING_SERVER_PROTECT_PASSWORD, ExtraSessionSetting);
				
			
			
			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}
	return false;
}

void UNWGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}

	}
}

void UNWGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), LobbyMapName, true, "listen");
	}
}


void UNWGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			/*
			Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}
			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}


void UNWGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful));
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			TArray<FCustomBlueprintSessionResult> CustomSessionResults;

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if (SessionSearch->SearchResults.Num() > 0)
			{
				//ULocalPlayer* const Player = GetFirstGamePlayer();

				

				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{

						//temporary Session result to hold our data for this loop
						FCustomBlueprintSessionResult TempCustomSeesionResult;

						//uncomment if you want the session name to always be the name of the owning player (Computer name on lan and Steam name online)
						//TempCustomSeesionResult.SessionName = SessionSearch->SearchResults[SearchIdx].Session.OwningUserName;
						TempCustomSeesionResult.bIsLan = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.bIsLANMatch;
						TempCustomSeesionResult.CurrentNumberOfPlayers = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.NumPublicConnections - SessionSearch->SearchResults[SearchIdx].Session.NumOpenPublicConnections;
						TempCustomSeesionResult.MaxNumberOfPlayers = SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.NumPublicConnections;
						TempCustomSeesionResult.Ping = SessionSearch->SearchResults[SearchIdx].PingInMs;

						// get the server name
						SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_NAME, TempCustomSeesionResult.SessionName);
						

						// get if the server is password protected
						SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_IS_PROTECTED, TempCustomSeesionResult.bIsPasswordProtected);


						// get the Password if the session is Password Protected
						if(TempCustomSeesionResult.bIsPasswordProtected)
							SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get(SETTING_SERVER_PROTECT_PASSWORD, TempCustomSeesionResult.SessionPassword);

						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TempCustomSeesionResult.SessionPassword);



						CustomSessionResults.Add(TempCustomSeesionResult);
					
				}
			}

			//call UMG to show sessions after the search ends
			OnFoundSessionsCompleteUMG(CustomSessionResults);

		}
	}
}


bool UNWGameInstance::JoinASession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}



bool UNWGameInstance::JoinASession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	// Return bool
	bool bSuccessful = false;
	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(LocalUserNum, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}


void UNWGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();
			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}


void UNWGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			// If it was successful, we just load another level (could be a MainMenu!)
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), MainMenuMap, true);
			}
		}
	}
}

// Steam never calls this since the invite is sent through the steam chat and you can only respond to it there by accepting or neglicting
/*void UNWGameInstance::OnSessionInviteReceived(const FUniqueNetId & InvitedPlayer, const FUniqueNetId & InvitingPlayer, const FString & AppID, const FOnlineSessionSearchResult & TheSessionInvitedTo)
{
	if (InvitedPlayer.IsValid() && InvitingPlayer.IsValid())
	{
		SessionInvite = TheSessionInvitedTo;
		ShowInviteReceivedUMG();
	}
}*/

void UNWGameInstance::OnSessionUserInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> InvitingPlayer, const FOnlineSessionSearchResult & TheSessionInvitedTo)
{
	if (bWasSuccessful)
	{
		if (TheSessionInvitedTo.IsValid())
		{
			JoinASession(LocalUserNum, GameSessionName, TheSessionInvitedTo);
		}
	}
}

void UNWGameInstance::SendSessionInviteToFriend(APlayerController* InvitingPlayer, const FBPUniqueNetId & Friend)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InvitingPlayer->GetLocalPlayer());
		if (LocalPlayer)
		{
			// Get SessionInterface from the OnlineSubsystem
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && LocalPlayer->GetPreferredUniqueNetId().IsValid() && Friend.IsValid())
			{
				Sessions->SendSessionInviteToFriend(*LocalPlayer->GetPreferredUniqueNetId(), GameSessionName, *Friend.GetUniqueNetId());
			}
		}
	}
}


void UNWGameInstance::StartOnlineGame(FString ServerName, int32 MaxNumPlayers, bool bIsLAN, bool bIsPresence, bool bIsPasswordProtected, FString SessionPassword)
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession(Player->GetPreferredUniqueNetId(), GameSessionName, ServerName, bIsLAN, bIsPresence, MaxNumPlayers, bIsPasswordProtected, SessionPassword);
}


void UNWGameInstance::FindOnlineGames(bool bIsLAN, bool bIsPresence)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	FindSessions(Player->GetPreferredUniqueNetId(), GameSessionName, bIsLAN, bIsPresence);
}


void UNWGameInstance::JoinOnlineGame(int32 SessionIndex)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FOnlineSessionSearchResult SearchResult;
	SearchResult = SessionSearch->SearchResults[SessionIndex];

	MaxPlayersinSession = SearchResult.Session.SessionSettings.NumPublicConnections;

	JoinASession(Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult);

}


void UNWGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			Sessions->DestroySession(GameSessionName);
		}
	}
}

void UNWGameInstance::ShowErrorMessage(const FText & ErrorMessage)
{
	//Show the message on UMG through blueprints
	ShowErrorMessageUMG(ErrorMessage);
}

bool UNWGameInstance::IsOnlineSubsystemSteam() const
{
	//get the steam online subsystem
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

	if (OnlineSub)
		return true;
	else
		return false;
}

FString UNWGameInstance::GetPlayerName() const
{
	//if steam is running, return an empty string
	if (IsOnlineSubsystemSteam())
		return "";
	//else retrun the saved player lan name
	else
		return LanPlayerName;
}

IOnlineSessionPtr UNWGameInstance::GetSession() const
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
		// Get the Session Interface
		return OnlineSub->GetSessionInterface();
	else 
		return nullptr;
}


UTexture2D* UNWGameInstance::GetSteamAvatar(const FBPUniqueNetId UniqueNetId)
{
	if (UniqueNetId.IsValid())
	{
		uint32 Width = 0;
		uint32 Height = 0;

		//get the player iID
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

		int Picture = 0;
	
		// get the Avatar ID using the player ID
		Picture = SteamFriends()->GetMediumFriendAvatar(id);

		//if the Avatar ID is not valid retrun null
		if (Picture == -1)
			return nullptr;

		//get the image size from steam
		SteamUtils()->GetImageSize(Picture, &Width, &Height);

		// if the image size is valid (most of this is from the Advanced Seesion Plugin as well, mordentral, THANK YOU!
		if (Width > 0 && Height > 0)
		{
			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			uint8 *oAvatarRGBA = new uint8[Width * Height * 4];


			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));

			UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

			// Switched to a Memcpy instead of byte by byte transer
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			// Original implementation was missing this!!
			// the hell man......
			// deallocating the memory used to get the avatar data
			delete[] oAvatarRGBA;

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();

			return Avatar;
		}

	}

	return nullptr;
}



void UNWGameInstance::GetSteamFriendsList(APlayerController *PlayerController)
{
	//check if the player controller is valid
	if (PlayerController)
	{

		//get the steam online subsystem
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

		//check if the online subsystem is valid
		if (OnlineSub)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Found Steam Online"));
			//get Friends interface
			IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();

			//if the Friends Interface is valid
			if (FriendsInterface.IsValid())
			{
				
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Found friend interface"));
				// get the local player from the player controller
				ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);

				//chaeck if the local player exists
				if (LocalPlayer)
				{
					//read the friend list from the online subsystem then call the delagate when completed
					FriendsInterface->ReadFriendsList(LocalPlayer->GetControllerId(), EFriendsLists::ToString((EFriendsLists::InGamePlayers)), FriendListReadCompleteDelegate);
				}
			}
		}
	}
}


// when reading friend list from the online subsystem is finished, get it and store it then call blueprint to show it on UMG
void UNWGameInstance::OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString & ListName, const FString & ErrorString)
{
	if (bWasSuccessful)
	{
		//get the steam online subsystem
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get(FName("Steam"));

		//check if the online subsystem is valid
		if (OnlineSub)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Found Steam Online"));
			//get Friends interface
			IOnlineFriendsPtr FriendsInterface = OnlineSub->GetFriendsInterface();

			//if the Friends Interface is valid
			if (FriendsInterface.IsValid())
			{

				TArray< TSharedRef<FOnlineFriend> > FriendList;
				//get a list on all online players and store them in the FriendList
				FriendsInterface->GetFriendsList(LocalUserNum, ListName/*EFriendsLists::ToString((EFriendsLists::Default)),*/, FriendList);

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, *FString::Printf(TEXT("Number of friends found is: %d"), FriendList.Num()));

				TArray<FSteamFriendInfo> BPFriendsList;
				//for each loop to convert the FOnlineFriend array into the cuteom BP struct
				for (TSharedRef<FOnlineFriend> Friend : FriendList)
				{
					//temp FSteamFriendInfo variable to add to the array
					FSteamFriendInfo TempSteamFriendInfo;
					//get the friend's User ID
					TempSteamFriendInfo.PlayerUniqueNetID.SetUniqueNetId(Friend->GetUserId());
					//get the friend's avatar as texture 2D and store it
					TempSteamFriendInfo.PlayerAvatar = GetSteamAvatar(TempSteamFriendInfo.PlayerUniqueNetID);
					//get the friend's display name
					TempSteamFriendInfo.PlayerName = Friend->GetDisplayName();
					//add the temp variable to the 
					BPFriendsList.Add(TempSteamFriendInfo);
				}

				//call blueprint to show the info on UMG
				OnGetSteamFriendRequestCompleteUMG(BPFriendsList);
			}
		}
	}
	else
		ShowErrorMessageUMG(FText::FromString(ErrorString));
}


