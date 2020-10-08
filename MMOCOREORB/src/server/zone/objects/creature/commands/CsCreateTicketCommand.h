/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CSCREATETICKETCOMMAND_H_
#define CSCREATETICKETCOMMAND_H_

#include "server/zone/managers/player/PlayerManager.h"

#include "server/login/account/Account.h"
#include "server/zone/packets/charcreation/ClientCreateCharacter.h"
#include "server/zone/packets/charcreation/ClientCreateCharacterCallback.h"
#include "server/zone/packets/charcreation/ClientCreateCharacterSuccess.h"
#include "server/zone/packets/charcreation/ClientCreateCharacterFailed.h"
#include "server/zone/objects/player/Races.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/managers/name/NameManager.h"
#include "server/zone/managers/templates/TemplateManager.h"
#include "server/zone/managers/object/ObjectManager.h"
#include "server/zone/managers/faction/FactionManager.h"
#include "server/db/ServerDatabase.h"
#include "server/db/MantisDatabase.h"
#include "server/chat/ChatManager.h"
#include "server/conf/ConfigManager.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/managers/player/VeteranRewardList.h"
#include "server/zone/managers/combat/CombatManager.h"
#include "server/zone/managers/skill/Performance.h"
#include "server/zone/managers/collision/CollisionManager.h"
#include "server/zone/objects/intangible/VehicleControlDevice.h"
#include "server/zone/objects/tangible/threat/ThreatMap.h"
#include "server/zone/objects/creature/VehicleObject.h"
#include "server/zone/objects/area/ActiveArea.h"
#include "server/login/packets/ErrorMessage.h"
#include "server/zone/packets/player/LogoutMessage.h"
#include "server/zone/objects/player/sessions/TradeSession.h"
#include "server/zone/objects/player/sessions/ProposeUnitySession.h"
#include "server/zone/objects/player/sessions/VeteranRewardSession.h"
#include "server/zone/objects/tangible/OptionBitmask.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/ship/ShipObject.h"

#include "server/zone/objects/group/GroupObject.h"

#include "server/zone/objects/building/BuildingObject.h"
#include "server/zone/templates/tangible/CloningBuildingObjectTemplate.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/tangible/wearables/ArmorObject.h"
#include "server/zone/objects/tangible/weapon/WeaponObject.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"

#include "server/zone/objects/player/events/PlayerIncapacitationRecoverTask.h"
#include "server/zone/objects/creature/events/ProposeUnityExpiredTask.h"
#include "server/zone/objects/player/events/ForceMeditateTask.h"
#include "server/zone/objects/player/events/MeditateTask.h"
#include "server/zone/objects/player/events/LogoutTask.h"
#include "server/zone/objects/player/sessions/EntertainingSession.h"

#include "server/zone/objects/building/cloning/CloneSpawnPoint.h"

#include "server/zone/objects/player/sui/messagebox/SuiMessageBox.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"
#include "server/zone/objects/cell/CellObject.h"
#include "server/zone/managers/skill/SkillManager.h"

#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/managers/planet/PlanetManager.h"

#include "server/zone/packets/trade/AbortTradeMessage.h"
#include "server/zone/packets/trade/AcceptTransactionMessage.h"
#include "server/zone/packets/trade/UnAcceptTransactionMessage.h"
#include "server/zone/packets/trade/AddItemMessage.h"
#include "server/zone/packets/trade/BeginTradeMessage.h"
#include "server/zone/packets/trade/DenyTradeMessage.h"
#include "server/zone/packets/trade/TradeCompleteMessage.h"
#include "server/zone/packets/trade/GiveMoneyMessage.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"

#include "server/zone/packets/tangible/TangibleObjectDeltaMessage3.h"
#include "server/zone/packets/player/PlayerObjectDeltaMessage6.h"
#include "server/zone/packets/object/StartingLocationListMessage.h"

#include "server/zone/objects/region/CityRegion.h"
#include "server/zone/managers/director/DirectorManager.h"

#include "server/zone/objects/player/sui/callbacks/CloningRequestSuiCallback.h"

#include "server/zone/objects/tangible/tool/CraftingStation.h"
#include "server/zone/objects/tangible/tool/CraftingTool.h"

#include "server/zone/Zone.h"
#include "server/zone/managers/player/creation/PlayerCreationManager.h"
#include "server/ServerCore.h"

#include "server/login/account/Account.h"

#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/structure/StructureManager.h"
#include "server/chat/ChatManager.h"
#include "server/zone/objects/building/BuildingObject.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/Zone.h"
#include "server/zone/templates/tangible/SharedStructureObjectTemplate.h"
#include "server/zone/objects/region/CityRegion.h"
#include "server/zone/objects/player/sui/callbacks/PlayerTeachSuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/PlayerTeachConfirmSuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/ProposeUnitySuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/SelectUnityRingSuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/ConfirmDivorceSuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/SelectVeteranRewardSuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/ConfirmVeteranRewardSuiCallback.h"

#include "server/zone/managers/stringid/StringIdManager.h"

#include "server/zone/objects/creature/buffs/PowerBoostBuff.h"

//#include "server/zone/objects/creature/Creature.h"
#include "server/zone/objects/creature/events/DespawnCreatureTask.h"
//#include "server/zone/objects/creature/AiAgent.h"
#include "server/zone/managers/gcw/GCWManager.h"

#include "server/zone/managers/creature/LairObserver.h"
#include "server/zone/objects/intangible/PetControlDevice.h"
#include "server/zone/managers/creature/PetManager.h"
#include "server/zone/packets/player/PlayMusicMessage.h"

#include "server/zone/objects/creature/events/BurstRunNotifyAvailableEvent.h"
//#include "server/zone/objects/creature/DroidObject.h"
#include "server/zone/managers/visibility/VisibilityManager.h"
#include "server/zone/objects/player/sui/callbacks/BountyHuntSuiCallback.h"
#include "server/zone/objects/player/sui/inputbox/SuiInputBox.h"
#include "server/zone/objects/player/sui/callbacks/csTicketCallback.h"
#include "server/zone/objects/scene/SceneObject.h"

class CsCreateTicketCommand : public QueueCommand {
public:

	CsCreateTicketCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* player, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(player))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(player))
			return INVALIDLOCOMOTION;

		ManagedReference<SuiInputBox*> input = new SuiInputBox(player, SuiWindowType::VENDOR_CUSTOM_PHRASE);
		input->setPromptTitle("New CSR/GM Ticket");
		input->setPromptText("Please describe the issue you are having in as much detail as possible. Please do not use profanity when describing your issue. If this ticket concerns an item please make sure to take a screenshot of all relevant data if possible.");
		input->setUsingObject(player);
		input->setMaxInputSize(255);
		input->setCallback(new csTicketCallback(player->getZoneServer()));

		player->getPlayerObject()->addSuiBox(input);
		player->sendMessage(input->generateMessage());

		return SUCCESS;
	}

};

#endif //CSCREATETICKETCOMMAND_H_
