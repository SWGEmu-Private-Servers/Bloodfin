/*
 * DisconnectClientEvent.h
 *
 *  Created on: Oct 14, 2010
 *      Author: oru
 */

#ifndef DISCONNECTCLIENTEVENT_H_
#define DISCONNECTCLIENTEVENT_H_

#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/templates/tangible/SharedCreatureObjectTemplate.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/planet/PlanetManager.h"
#include "server/zone/objects/ship/ShipObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"

class DisconnectClientEvent : public Task {
	ManagedReference<CreatureObject*> player;
	ManagedReference<ZoneClientSession*> client;
	ManagedReference<Zone*> zone;
	int eventType;

public:
	const static int DISCONNECT = 1;
	const static int LOGOUT = 2;
	const static int SETLINKDEAD = 3;

public:
	DisconnectClientEvent(CreatureObject* pl, ZoneClientSession* cl, int type) : Task() {
		player = pl;
		client = cl;
		eventType = type;
	}

	void run() {
		if (player == NULL || client == NULL)
			return;

		Locker _locker(player);

		PlayerObject* ghost = player->getPlayerObject();

		Zone* zone = player->getZone();

		if (zone == NULL)
			return;

		// store ship if piloting
		if (player->hasState(CreatureState::PILOTINGSHIP)) {
			String planetZone = "corellia";

			if (zone->getZoneName().contains("corellia")) {
				planetZone = "corellia";
			} else if (zone->getZoneName().contains("naboo")) {
				planetZone = "naboo";
			} else if (zone->getZoneName().contains("tatooine")) {
				planetZone = "tatooine";
			} else if (zone->getZoneName().contains("rori")) {
				planetZone = "rori";
			} else if (zone->getZoneName().contains("yavin4")) {
				planetZone = "yavin4";
			} else if (zone->getZoneName().contains("talus")) {
				planetZone = "talus";
			} else if (zone->getZoneName().contains("lok")) {
				planetZone = "lok";
			} else if (zone->getZoneName().contains("endor")) {
				planetZone = "endor";
			} else if (zone->getZoneName().contains("dathomir")) {
				planetZone = "dathomir";
			} else if (zone->getZoneName().contains("dantooine")) {
				planetZone = "dantooine";
			} else if (zone->getZoneName().contains("kaas")) {
				planetZone = "kaas";
			} else if (zone->getZoneName().contains("hoth")) {
				planetZone = "hoth";
			}



			// TODO: Fill in the rest of the zones here

			ManagedReference<ShipObject*> ship = dynamic_cast<ShipObject*>(player->getParent().get().get());

			// toss the player somewhere on the planet
			player->switchZone(planetZone, System::random(5000), 0, System::random(5000));
			player->clearState(CreatureState::PILOTINGSHIP);
			player->addWounds(CreatureAttribute::MIND, 700, true);
			player->addWounds(CreatureAttribute::HEALTH, 700, true);
			player->addWounds(CreatureAttribute::ACTION, 700, true);
			player->addWounds(CreatureAttribute::STRENGTH, 700, true);

			if (ship != NULL) {
				Locker shiplock(ship);

				ship->destroyObjectFromWorld(true);
			}
		}
		if (player->isInvisible()) {
	       	 player->setSpeedMultiplierMod(1.0f);
	       	 player->setAccelerationMultiplierMod(1.0f);
	         player->removePendingTask("forcecloakevent");
	         player->removePendingTask("cloakevent");
			 player->setInvisible(false);
		SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();

		for (int i = 0; i < closeObjects->size(); ++i) {
			SceneObject* scno = cast<SceneObject*>( closeObjects->get(i).get());

			if (scno != player && !scno->isBuildingObject())
					scno->notifyInsert(player);

		}

//		UpdateTransformMessage* msg = new UpdateTransformMessage(player);
//		player->broadcastMessage(msg, true);
		}
		/*
		if (player->hasState(CreatureState::PILOTINGSHIP) ) {

			player->switchZone("corellia", System::random(5000), 0, System::random(5000));
		}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("corellia")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("corellia", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("naboo")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("naboo", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("tatooine")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("tatooine", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("rori")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("rori", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("yavin4")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("yavin4", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("talus")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("talus", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("lok")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("lok", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("endor")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("endor", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("kaas")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("kaas", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("hoth")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("hoth", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("dathomir")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("dathomir", System::random(5000), 0, System::random(5000));
			}
			if (player->hasState(CreatureState::PILOTINGSHIP) && zone->getZoneName().contains("dantooine")) {
				player->clearState(CreatureState::PILOTINGSHIP);
				player->switchZone("dantooine", System::random(5000), 0, System::random(5000));
			}


		if (player->isInCombat()) {
			player->sendSystemMessage("you cant logout in combat");
			return;
		} */
		switch (eventType) {
		case DISCONNECT:
			ghost->disconnect(false, true);
			break;
		case LOGOUT:
			ghost->logout(true);
			break;
		case SETLINKDEAD:
			ghost->setLinkDead();
			break;
		}
	}


};


#endif /* DISCONNECTCLIENTEVENT_H_ */
