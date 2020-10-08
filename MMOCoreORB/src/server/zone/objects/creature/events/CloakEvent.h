/**
 * file cloakEvent.h
 * author Heat
 * date 11.04.2015
 */

#ifndef CLOAKEVENT_H_
#define CLOAKEVENT_H_

#include "engine/engine.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"

class CloakEvent: public Task {
	ManagedReference<CreatureObject*> player;

public:
	CloakEvent(CreatureObject* pl) {
		player = pl;
	}

	void run() {
		  if (player == NULL)

		   return;

          Locker playerLocker(player);

          PlayerObject* targetGhost = player->getPlayerObject();

         if (!player->isOnline() || player->isIncapacitated() || player->isInCombat()) {
        	 player->setSpeedMultiplierMod(1.0f);
        	 player->setAccelerationMultiplierMod(1.0f);
              player->removePendingTask("cloakevent");
				player->setInvisible(false);

				SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();
				Zone* zone = player->getZone();

				for (int i = 0; i < closeObjects->size(); ++i) {
					SceneObject* scno = cast<SceneObject*>( closeObjects->get(i).get());

					if (scno != player && !scno->isBuildingObject())
							scno->notifyInsert(player);
					PlayClientEffectLoc* cloakdropLoc = new PlayClientEffectLoc("clienteffect/pl_force_speed_self.cef", player->getZone()->getZoneName(), player->getPositionX(), player->getPositionZ(), player->getPositionY());
					player->broadcastMessage(cloakdropLoc, true);

 				if (zone == NULL)
					return;
			 }
			}

		if (player->isInvisible()) {

			player->inflictDamage(player, CreatureAttribute::ACTION, 400, true);
			player->sendSystemMessage("Your energy drains to stay hidden");
			reschedule( 12000 );

		}
	}

};

#endif /* CLOAKEVENT_H_ */
