
#include "Gameplay/Utilities/CS_PlayerStart.h"
#include "Gameplay/Utilities/CS_CheckingLoadingOfWorldPartitionChunks.h"

ACS_PlayerStart::ACS_PlayerStart(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) 
{
	WorldPartitionSteamingChecker = CreateDefaultSubobject<UCS_CheckingLoadingOfWorldPartitionChunks>(TEXT("WorldPartitionSteamingChecker"));
	WorldPartitionSteamingChecker->OnStreamingCompleted.AddDynamic(this, &ACS_PlayerStart::MainCourseMapLoadingFinish);
}

UCS_CheckingLoadingOfWorldPartitionChunks* ACS_PlayerStart::GetWorldPartitionSteamingChecker()
{
	return WorldPartitionSteamingChecker;
}

void ACS_PlayerStart::MainCourseMapLoadingFinish(bool WorldIsLoad)
{
	if(WorldIsLoad)
		Destroy();
}
