
#ifndef HYPERSPACETASK_H_
#define HYPERSPACETASK_H_

#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"
#include "server/zone/packets/scene/UpdateTransformMessage.h"
#include "server/zone/objects/scene/SceneObject.h"
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

class HyperspaceTask : public Task {


private:
		enum Phase { INITIAL, SECOND, THIRD, FOURTH, FINAL} currentPhase;
		ManagedReference<Creature*> creature;
		ManagedReference<CreatureObject*> player;


public:
	HyperspaceTask(CreatureObject* pl) : Task() {
		currentPhase = INITIAL;
		player = pl;


	}


	void run() {
		  if (player == NULL)

		   return;

		PlayMusicMessage* pmm = new PlayMusicMessage("sound/ship_hyperspace_countdown.snd");
		player->sendMessage(pmm);
		player->playEffect("clienteffect/space_command/scram_reactor_startup_engine.cef", "");
			Locker locker(player);

//			Locker _clocker(player, creature);

			player->removePendingTask("hyperspacetask");


		ChatManager* chatManager = player->getZoneServer()->getChatManager();

		switch (currentPhase) {
		case INITIAL:
			player->sendSystemMessage("Jump Drive NAV Active");

			currentPhase = FINAL; // SECOND;
			player->addPendingTask("hyperspace", this, 4600);
			break;
		case SECOND:
			player->sendSystemMessage("Plotting Hyperspace Route");

			currentPhase = THIRD;
			player->addPendingTask("hyperspace", this, 4600);
			break;
		case THIRD:
			player->sendSystemMessage("NAV System Handoff Complete");

			currentPhase = FOURTH;
			player->addPendingTask("hyperspace", this, 6600);
			break;
		case FOURTH:
			player->sendSystemMessage("Jump Drive Spooling");
			player->playEffect("clienteffect/space_command/sys_engine_startup.cef", "");
			player->playEffect("clienteffect/space_command/sys_engine_startup.cef", "");
			player->playEffect("clienteffect/space_command/sys_engine_startup.cef", "");
			currentPhase = FINAL;
			player->addPendingTask("hyperspace", this, 6000);
			break;

		case FINAL:

			if (Zone* zone = player->getZone()) {

				if (!zone->getZoneName().contains("space")) {
					return;
				}

				ManagedReference<ShipObject*> ship = dynamic_cast<ShipObject*>(player->getParent().get().get());
				if (ship==NULL) {
					return;
				}
				Locker locker(ship,player);
				String spaceZone = "space_corellia";

				ManagedReference<PlayerManager*> playerManager = player->getZoneServer()->getPlayerManager();

				if (!playerManager) {
					return;
				}

				ManagedReference<SceneObject*> datapad = player->getSlottedObject("datapad");

				if (!datapad) {
					return;
				}

				ManagedReference<ShipControlDevice*> shipControlDevice = NULL;

				for (int i = 0; i < datapad->getContainerObjectsSize(); i++) {
					Reference<SceneObject*> obj =  datapad->getContainerObject(i).castTo<SceneObject*>();

					if (obj && obj->isShipControlDevice()) {
						Reference<ShipControlDevice*> controlDevice = obj.castTo<ShipControlDevice*>();
						if (controlDevice->getControlledObject() == ship) {
							shipControlDevice = controlDevice;
							break;
						}
					}
				}

				if (!shipControlDevice) {
					return;
				}

				Locker shipLock(shipControlDevice);

				player->info("entering Hyperspace", true);
				shipControlDevice->storeObject(player, true);

				// get the player in space
				player->switchZone(spaceZone, System::random(1500), System::random(1500), System::random(1500));

				player->info("exiting Hyperspace", true);
				shipControlDevice->generateObject(player);
				player->sendSystemMessage("Hyperspace Jump Complete");

				return;
			}
		}
	}
};

#endif /*  HYPERSPACETASK_H_ */
