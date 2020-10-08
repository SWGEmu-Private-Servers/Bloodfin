/**
 * file PosSecEvent.h
 * author Heat
 * date 11.04.2015
 */

#ifndef POSSECEVENT_H_
#define POSSECEVENT_H_

#include "engine/engine.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"

class PosSecEvent: public Task {
	ManagedReference<CreatureObject*> player;

public:
	PosSecEvent(CreatureObject* pl) {
		player = pl;
	}

	void run() {
		  if (player == NULL)

		   return;

          Locker playerLocker(player);




         if (!player->isOnline() || player->isIncapacitated()) {
        	 player->setSpeedMultiplierMod(1.0f);
        	 player->setAccelerationMultiplierMod(1.0f);
              player->removePendingTask("positionsecuredevent");


				SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();
				Zone* zone = player->getZone();



 				if (zone == NULL)
					return;
			 }
			}



};

#endif /* POSSECEVENT_H_ */
