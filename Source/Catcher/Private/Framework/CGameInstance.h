// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "CGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	bool IsSessionServer() const;
	
private:
	void CreateSession();
	void TerminateSessionServer();
	
	FOnlineSessionSettings GenerateSessionSetting(const FName& SessionName, const FString& SessionSearchID, int Port);
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	
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
