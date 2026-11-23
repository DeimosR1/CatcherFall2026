// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "CGameInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAuthLoginCompletedDelegate, bool /*bWasSuccessful*/, const FString& /*PlayerNickname*/, const FString& /*ErrorMessage*/);
UCLASS()
class UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	bool IsSessionServer() const;
	void PlayerJoined(const FUniqueNetIdRepl& UniqueId);
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId);

private:
	void CreateSession();
	void TerminateSessionServer();
	
	FOnlineSessionSettings GenerateSessionSetting(const FName& SessionName, const FString& SessionSearchID, int Port);
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeoutDuration = 60.0f;
	
	FTimerHandle WaitPlayerJoinTimeoutHandle;
	void WaitPlayerJoinTimeoutReached();
	
	TSet<FUniqueNetIdRepl> PlayerRecord;
	//--------------------------------------------------------------------//
	//                            Game Client                             //
	//--------------------------------------------------------------------//
public:
	FOnAuthLoginCompletedDelegate OnAuthLoginCompletedDelegate;
	bool IsLoggedIn() const;
	bool IsLoggingIn() const;
	void ClientAuthPortalLogin();
	
	void RequestCreateNewSession();
	void TryFindAndJoinSession();
	
private:
	void ClientLoginCompleted(int LocalPlayerNumber, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorMsg);
	
	bool bIsLoggingIn;
	
		
	FString GetCoordinatorURL();
	UPROPERTY(EditDefaultsOnly, Category = "Coordinator")
	FString DEFAULT_COORDINATOR_URL = "127.0.0.1";
	
	FName COORDINATOR_URL_KEY = "COORDINATOR_URL";
	
	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bProcessedSuccessfully, FGuid SessionSearchID);
	
	void StartFindingAndJoinSession(FGuid SessionSearchID = FGuid{});
	
	FTimerHandle FindSessionTimerHandle;
	FTimerHandle FindSessionTimeoutTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float SessionSearchInterval = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float SessionSearchTimeoutDuration = 30.0f;
	void FindAndJoinSession(FGuid SessionSearchID);
	void StopSessionSearches();
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	void FindSessionCompleted(bool bWasSuccessful);
	void JoinSessionWithSearchResults(const FOnlineSessionSearchResult& SessionSearchResults);
	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port);
	
	//--------------------------------------------------------------------//
	//                          Oline Interfaces                          //
	//--------------------------------------------------------------------//
private:
	IOnlineSessionPtr GetSessionInterface() const;
	IOnlineIdentityPtr GetIdentityInterface() const;
	
	
	//--------------------------------------------------------------------//
	//                         Commandline Utilities                      //
	//--------------------------------------------------------------------//
	
private:
	
	FString GetSessionNameStr() const;
	FString GetSessionSearchIDStr() const;
	int GetSessionPort() const;
	
	FString GetCommandlineArgAsString(const FName& ParamName) const;
	int GetCommandLineAsInt(const FName& ParamName) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Commandline Args")
	FName SessionNameKey = "SESSION_NAME";
	UPROPERTY(EditDefaultsOnly, Category = "Commandline Args")
	FName SessionSearchIDKey = "SESSION_SEARCH_ID";
	UPROPERTY(EditDefaultsOnly, Category = "Commandline Args")
	FName SessionPortKey = "SESSION_PORT";
	
	FString ServerSessionName;
	int ServerSessionPort;
	
	//--------------------------------------------------------------------//
	//                            Map Loading                             //
	//--------------------------------------------------------------------//
private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;
	
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
};
