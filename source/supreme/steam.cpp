#include "steam.h"
#include <memory>

#ifdef HAS_STEAM_API
#include <steam/steam_api.h>
#include <vector>
#include <string>

class SteamManagerImpl : public SteamManager
{
public:
	std::vector<PublishedFileId_t> subscribedItemIds;

	~SteamManagerImpl()
	{
		SteamAPI_Shutdown();
	}

	SteamManagerImpl()
	{
		uint32_t subscribedCount = SteamUGC()->GetNumSubscribedItems();
		subscribedItemIds.resize(subscribedCount);
		subscribedCount = SteamUGC()->GetSubscribedItems(subscribedItemIds.data(), subscribedCount);
		subscribedItemIds.resize(subscribedCount);

		printf("Num items: %d\n", (int)subscribedItemIds.size());
		for (auto fileId : subscribedItemIds) {
			uint32_t state = SteamUGC()->GetItemState(fileId);
			printf("  Item %llu has state %u\n", fileId, state);
			if ((state & k_EItemStateInstalled) && (state & k_EItemStateSubscribed)) {
				std::string fname(1024, '\0');
				printf("    installinfo? %s\n", SteamUGC()->GetItemInstallInfo(fileId, nullptr, fname.data(), fname.size(), nullptr) ? "true" : "false");
				printf("    fname = %s\n", fname.c_str());
			}
		}
	}

	const char* DescribeEdition()
	{
		return "Steam edition";
	}
};

#endif  // HAS_STEAM_API

static std::unique_ptr<SteamManager> gSteamManager = nullptr;

SteamManager* SteamManager::Init()
{
#ifdef HAS_STEAM_API
	if (SteamAPI_Init())
	{
		gSteamManager = std::make_unique<SteamManagerImpl>();
	}
#endif  // HAS_STEAM_API

	if (!gSteamManager)
	{
		gSteamManager = std::make_unique<SteamManager>();
	}
	return gSteamManager.get();
}

SteamManager* SteamManager::Get()
{
	return gSteamManager.get();
}

void SteamManager::Quit()
{
	gSteamManager.reset();
}
