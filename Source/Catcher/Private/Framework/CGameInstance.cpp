// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"

#include "HttpModule.h"
#include "Online.h"
#include "Developer/Windows/LiveCodingServer/Public/ILiveCodingServer.h"
#include "Interfaces/IHttpRequest.h"
#include "Trace/Analysis.h"

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld()){return;}
	
	if (IsSessionServer())
	{
		CreateSession();
	}
	
	if (IOnlineSubsystem* Sub = IOnlineSubsystem::Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("Online subsystem: %s"), *Sub->GetSubsystemName().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No online subsystem!"));
	}
}

bool UCGameInstance::IsSessionServer() const
{
	return GetWorld() && GetWorld()->GetNetMode() == NM_DedicatedServer;
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	}
	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("----All Players Left, Terminating----"));
		TerminateSessionServer();
	}
}

void UCGameInstance::CreateSession()
{
	if (IOnlineSessionPtr SessionPtr = GetSessionInterface())
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------Server Creating Session!------------------"));
		ServerSessionName = GetSessionNameStr();
		FString ServerSessionSearchID = GetSessionSearchIDStr();
		ServerSessionPort = GetSessionPort();
	
		UE_LOG(LogTemp, Warning, TEXT("#### Creating Session with Name: %s, ID: %s, Port: %d"), 
			*ServerSessionName, *ServerSessionSearchID, ServerSessionPort);
		
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), GenerateSessionSetting(FName(ServerSessionName), ServerSessionSearchID, ServerSessionPort)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Creation failed right away!"));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation failed right away!"))
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		TerminateSessionServer();
	}
}

void UCGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr OnlineSessionPtr = GetSessionInterface())
	{
		OnlineSessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		OnlineSessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnEndSessionComplete);
		if (!OnlineSessionPtr->EndSession(FName(ServerSessionName)))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

FOnlineSessionSettings UCGameInstance::GenerateSessionSetting(const FName& SessionName, const FString& SessionSearchID,
                                                              int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false;
	OnlineSessionSettings.NumPublicConnections = 10;
	OnlineSessionSettings.bShouldAdvertise = true;
	OnlineSessionSettings.bUsesPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false;
	OnlineSessionSettings.bAllowInvites = true;
	OnlineSessionSettings.bAllowJoinInProgress = true;
	OnlineSessionSettings.bUseLobbiesIfAvailable = false;
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
	OnlineSessionSettings.bUsesStats = true;
	
	OnlineSessionSettings.Set(SessionNameKey, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(SessionSearchIDKey, SessionSearchID, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(SessionPortKey, Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	return OnlineSessionSettings;
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session %s created successfully!"), *SessionName.ToString());
		LoadLevelAndListen(GameLevel);
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &UCGameInstance::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeoutDuration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session %s creation failed!"), *SessionName.ToString());
		TerminateSessionServer();
	}
	
	IOnlineSessionPtr SessionPtr = GetSessionInterface();
	if (SessionPtr)
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("----------Session Server Shutdown after %f seconds without player joining-------------"), WaitPlayerJoinTimeoutDuration);
	TerminateSessionServer();
}


bool UCGameInstance::IsLoggedIn() const
{
	IOnlineIdentityPtr IdentityInterface = GetIdentityInterface();
	if (IdentityInterface)
	{
		return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
	return false;
}

bool UCGameInstance::IsLoggingIn() const
{
	return bIsLoggingIn;
}

void UCGameInstance::ClientAuthPortalLogin()
{
	bIsLoggingIn = true;
	IOnlineIdentityPtr IdentityInterface = GetIdentityInterface();
	if (IdentityInterface)
	{
		IdentityInterface->OnLoginCompleteDelegates->RemoveAll(this);
		IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UCGameInstance::ClientLoginCompleted);
		if (!IdentityInterface->Login(0, FOnlineAccountCredentials("accountportal","","")))
		{
			bIsLoggingIn = false;
			UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
			UE_LOG(LogTemp, Warning, TEXT("------Login Failed Right Away-----"));
			UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
		}
	}
	else
	{
		bIsLoggingIn = false;
		UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
		UE_LOG(LogTemp, Warning, TEXT("---Identity Interface not found---"));
		UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
	}
}

void UCGameInstance::RequestCreateNewSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting to create new session"));
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FGuid SessionSearchID = FGuid::NewGuid();
	FString CoordinatorURL = GetCoordinatorURL();
	UE_LOG(LogTemp, Warning, TEXT("Coordinator URL is: %s"), *CoordinatorURL);
	
	// 127.0.0.1/Session is the Default
	FString URL = FString::Printf(TEXT("%s/Session"), *CoordinatorURL);
	Request->SetURL(URL);
	//POST means you are requesting something to be created.
	Request->SetVerb("POST");
	
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json")); //Json -> Java Script Object Notation
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable<FJsonObject>(new FJsonObject());
	JsonObject->SetStringField(SessionNameKey.ToString(),FGuid::NewGuid().ToString());
	JsonObject->SetStringField(SessionSearchIDKey.ToString(),SessionSearchID.ToString());
	
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	Request->SetContentAsString(RequestBody);
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCGameInstance::SessionCreationRequestCompleted, SessionSearchID);
	
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed Right away."));
	}
}

void UCGameInstance::StartFindingAndJoinSession(FGuid SessionSearchID)
{
		UE_LOG(LogTemp, Warning, TEXT("Start Finding and Join Session with Session Search ID: %s"), *(SessionSearchID.ToString()));
	StopSessionSearches();
	
	GetWorld()->GetTimerManager().SetTimer(
		FindSessionTimerHandle, 
		FTimerDelegate::CreateUObject(this, &UCGameInstance::FindAndJoinSession, SessionSearchID),
		SessionSearchInterval, true);
	
	GetWorld()->GetTimerManager().SetTimer(FindSessionTimeoutTimerHandle,
		this, &UCGameInstance::StopSessionSearches, SessionSearchTimeoutDuration);
}

void UCGameInstance::TryFindAndJoinSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting to find and join session"));
	StartFindingAndJoinSession();
}

void UCGameInstance::ClientLoginCompleted(int LocalPlayerNumber, bool bWasSuccessful, const FUniqueNetId& UserId,
                                          const FString& ErrorMsg)
{
	bIsLoggingIn = false;
	IOnlineIdentityPtr IdentityInterface = GetIdentityInterface();
	if (IdentityInterface)
	{
		IdentityInterface->OnLoginCompleteDelegates->RemoveAll(this);
		if (bWasSuccessful)
		{
			FString PlayerNickname = IdentityInterface->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
			UE_LOG(LogTemp, Warning, TEXT("-----Logged in successful as:-----"));
			UE_LOG(LogTemp, Warning, TEXT("-----%s-----"), *PlayerNickname);
			UE_LOG(LogTemp, Warning, TEXT("----------------------------------"));
			OnAuthLoginCompletedDelegate.Broadcast(bWasSuccessful, PlayerNickname, ErrorMsg);
			return;
		}
	}
	
	OnAuthLoginCompletedDelegate.Broadcast(bWasSuccessful, "", ErrorMsg);
	
}

FString UCGameInstance::GetCoordinatorURL()
{
	FString CoordinatorUrl = GetCommandlineArgAsString(COORDINATOR_URL_KEY);
	if (CoordinatorUrl != "")
	{
		return CoordinatorUrl;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Can't find command coordinator url argument, using default one: %s"), *(DEFAULT_COORDINATOR_URL));
	return DEFAULT_COORDINATOR_URL;
}

void UCGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bProcessedSuccessfully, FGuid SessionSearchID)
{
	if (bProcessedSuccessfully && Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Successful!"));
		int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode == 200)
		{
			FString ResponseString = Response->GetContentAsString();
			
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
			
			int32 Port = 0;
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				Port = JsonObject->GetIntegerField(*(SessionPortKey.ToString()));
				UE_LOG(LogTemp, Warning, TEXT("Created Game in port: %d"), Port);
				StartFindingAndJoinSession(SessionSearchID);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed!"));
	}
}

void UCGameInstance::FindAndJoinSession(FGuid SessionSearchID)
{
	UE_LOG(LogTemp, Warning, TEXT("Retry to Find and join Session Search ID: %s"), *SessionSearchID.ToString());
	if (IOnlineSessionPtr SessionPtr = GetSessionInterface())
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch);
		if (SessionSearch)
		{
			SessionSearch->bIsLanQuery = false;
			SessionSearch->MaxSearchResults = 1;
			if (SessionSearchID.IsValid())
			{
				SessionSearch->QuerySettings.Set(SessionSearchIDKey, SessionSearchID.ToString(), EOnlineComparisonOp::Equals);
			}
			
			SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::FindSessionCompleted);
			if (!SessionPtr->FindSessions(0, SessionSearch.ToSharedRef()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Session Search Failed Right away!"))
				SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
			}
		}
	}
}

void UCGameInstance::StopSessionSearches()
{
	if (FindSessionTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FindSessionTimerHandle);
	}
	if (FindSessionTimeoutTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FindSessionTimeoutTimerHandle);
	}
	
	if (IOnlineSessionPtr SessionPtr = GetSessionInterface())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionSearch->SearchResults.Num() == 0) {return;}
	
	UE_LOG(LogTemp, Warning, TEXT("Found Session to Join"))
	StopSessionSearches();
	JoinSessionWithSearchResults(SessionSearch->SearchResults[0]);
}

void UCGameInstance::JoinSessionWithSearchResults(const FOnlineSessionSearchResult& SessionSearchResults)
{
	UE_LOG(LogTemp, Warning, TEXT("Join Session with search result!"));
	IOnlineSessionPtr SessionPtr = GetSessionInterface();
	
	if (SessionPtr)
	{
		FString SessionName = "";
		SessionSearchResults.Session.SessionSettings.Get<FString>(SessionNameKey, SessionName);
		
		int64 Port = 0;
		const FOnlineSessionSetting* PortSetting = SessionSearchResults.Session.SessionSettings.Settings.Find(SessionPortKey);
		if (PortSetting) {PortSetting->Data.GetValue(Port);}
		
		UE_LOG(LogTemp, Warning, TEXT("Trying to join session: %s, at port: %d"), *SessionName, (int)Port);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UCGameInstance::JoinSessionCompleted, (int)Port);
		if (!SessionPtr->JoinSession(0, FName(SessionName), SessionSearchResults))
		{
			UE_LOG(LogTemp, Warning, TEXT("Joining Session Failed Right Away!"));
			SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		}
	}
}

void UCGameInstance::JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port)
{
	IOnlineSessionPtr SessionPtr = GetSessionInterface();
	if (SessionPtr && JoinResult == EOnJoinSessionCompleteResult::Success)
	{
		StopSessionSearches();
		UE_LOG(LogTemp, Warning, TEXT("Join Session Successful! with port: %d"), Port);
		
		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);
		FString TestingURL = GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
		}
		
		FURL URL(nullptr, *TravelURL, TRAVEL_Absolute);
		URL.Port = Port;
		TravelURL = URL.ToString();
		
		UE_LOG(LogTemp, Warning, TEXT("Traveling to session at: %s"), *TravelURL);
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Session failed."));
	}
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), ServerSessionPort);
		UE_LOG(LogTemp, Warning, TEXT("Travel: %s"), *TravelStr);
		GetWorld()->ServerTravel(TravelStr);
	}
}

FString UCGameInstance::GetTestingURL() const
{
	FString TestingURL = GetCommandlineArgAsString(TestingURLKey);
	return TestingURL;
}

FString UCGameInstance::GetCommandlineArgAsString(const FName& ParamName) const
{
	FString OutVal = "";
	FString CommandlineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	if (!FParse::Value(FCommandLine::Get(), *CommandlineArg, OutVal))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find command line argument: %s"), *(ParamName.ToString()));
	}
	
	return OutVal;
}

int UCGameInstance::GetCommandLineAsInt(const FName& ParamName) const
{
	int OutVal = 0;
	FString CommandlineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	if (!FParse::Value(FCommandLine::Get(), *CommandlineArg, OutVal))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find command line argument: %s"), *(ParamName.ToString()));
	}
	
	return OutVal;
}

IOnlineSessionPtr UCGameInstance::GetSessionInterface() const
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetSessionInterface();
	}
	return nullptr;
}

IOnlineIdentityPtr UCGameInstance::GetIdentityInterface() const
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		return OnlineSubsystem->GetIdentityInterface();
	}
	return nullptr;
}

FString UCGameInstance::GetSessionNameStr() const
{
	return GetCommandlineArgAsString(SessionNameKey);
}

FString UCGameInstance::GetSessionSearchIDStr() const
{
	return GetCommandlineArgAsString(SessionSearchIDKey);
}

int UCGameInstance::GetSessionPort() const
{
	return GetCommandLineAsInt(SessionPortKey);
}
