#include "loonyArchipelago.h"
struct ArchipelagoLocation
{
	/* data */
	std::string Name;
	bool isQuest;
	std::string mapName;
	int mapId, xcoord, ycoord;
};
static const ArchipelagoLocation basic_locations[] = {

}

int ArchipelagoConnect(void) {
	AP_Init(IPAddress, SlotName, Password);
	AP_SetItemClearCallback(void (*f_itemclr)());
	AP_SetItemRecvCallback(void (*f_itemrecv)(int, bool));
	AP_SetLocationCheckedCallback(void (*f_locrecv)(int));
	AP_SetDeathLinkSupported(bool);
	AP_SetDeathLinkRecvCallback(void (*f_deathrecv)());

	AP_Start();
}

void ItemsClear() {

}
void ItemReceived(int item_id, bool notif) {

}

void SetLocationChecked(int loc_id)
{

}

void DeathLinkReceived()
{

}

void SetupWorld()
{

}
