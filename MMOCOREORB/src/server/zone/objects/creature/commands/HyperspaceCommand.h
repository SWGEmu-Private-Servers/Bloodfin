/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef HYPERSPACE_H_
#define HYPERSPACE_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/creature/VehicleObject.h"
#include "server/zone/objects/intangible/ControlDevice.h"
#include "server/zone/templates/tangible/SharedCreatureObjectTemplate.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"
#include "server/zone/packets/scene/UpdateTransformMessage.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/managers/space/SpaceManager.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/packets/player/PlayMusicMessage.h"
#include "server/chat/ChatManager.h"
#include "engine/engine.h"
#include "server/zone/Zone.h"
#include "server/zone/managers/combat/CombatManager.h"
#include "CombatQueueCommand.h"
#include "server/zone/objects/creature/events/HyperspaceTask.h"

class HyperspaceCommand : public QueueCommand {
public:

	HyperspaceCommand(const String& name, ZoneProcessServer* server)
	: QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

        Reference<Task*> task = creature->getPendingTask("hyperspacetask");

		ChatManager* chatManager = creature->getZoneServer()->getChatManager();

		if (!creature->checkCooldownRecovery("used_hyperdrive")) {
					StringIdChatParameter stringId;

					Time* cdTime = creature->getCooldownTime("used_hyperdrive");


					int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

					stringId.setStringId("You must waiting....");
					stringId.setDI(timeLeft);
					creature->sendSystemMessage("**ERROR** JUMP IN PROGRESS");
					return 0;
		}

		creature->sendSystemMessage("Initializing Drive System");
		PlayMusicMessage* pmm = new PlayMusicMessage("sound/ship_hyperspace_countdown.snd");
		creature->sendMessage(pmm);
		creature->addCooldown("used_hyperdrive", 20 * 1000);

        if (task == NULL) {

                Reference<HyperspaceTask*> hyperspace = new HyperspaceTask(creature);
                creature->addPendingTask("hyperspacetask", hyperspace, 3600);

        }

//		creature->playEffect("clienteffect/space_command/scram_reactor_startup_engine.cef", "");
		return SUCCESS;
	}

};

#endif //HYPERSPACE_H_
