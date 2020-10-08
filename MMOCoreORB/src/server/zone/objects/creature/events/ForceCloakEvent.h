/**
 * file forcecloakEvent.h
 * author Heat
 * date 11.04.2015
 */

#ifndef FORCECLOAKEVENT_H_
#define FORCECLOAKEVENT_H_

#include "engine/engine.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"

class ForceCloakEvent: public Task {
	ManagedReference<CreatureObject*> player;

public:
	ForceCloakEvent(CreatureObject* pl) {
		player = pl;
	}

	void run() {
		  if (player == NULL)

		   return;

          Locker playerLocker(player);




         if (!player->isOnline() || player->isIncapacitated() || player->isInCombat()) {
        	 player->setSpeedMultiplierMod(1.0f);
        	 player->setAccelerationMultiplierMod(1.0f);
              player->removePendingTask("forcecloakevent");
				player->setInvisible(false);

				SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();
				Zone* zone = player->getZone();

				for (int i = 0; i < closeObjects->size(); ++i) {
					SceneObject* scno = cast<SceneObject*>( closeObjects->get(i).get());

					if (scno != player && !scno->isBuildingObject())
							scno->notifyInsert(player);

 				if (zone == NULL)
					return;
			 }
			}
			ManagedReference<PlayerObject*> playerObject = player->getPlayerObject();
   		if (playerObject->getForcePower() < 700 ) {
   			 player->sendSystemMessage("you have no force left to stay invisible");
   	       	 player->setSpeedMultiplierMod(1.0f);
   	       	 player->setAccelerationMultiplierMod(1.0f);
   	         player->removePendingTask("forcecloakevent");
   			 player->setInvisible(false);
 			SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();

 			for (int i = 0; i < closeObjects->size(); ++i) {
 				SceneObject* scno = cast<SceneObject*>( closeObjects->get(i).get());

 				if (scno != player && !scno->isBuildingObject())
 						scno->notifyInsert(player);
				PlayClientEffectLoc* cloakdropLoc = new PlayClientEffectLoc("clienteffect/death_trooper_anti_virus.cef", player->getZone()->getZoneName(), player->getPositionX(), player->getPositionZ(), player->getPositionY());
				player->broadcastMessage(cloakdropLoc, true);
 			}



 			UpdateTransformMessage* msg = new UpdateTransformMessage(player);
 			player->broadcastMessage(msg, true);
   			return;
   			}
		if (player->isInvisible()) {
			playerObject->setForcePower(playerObject->getForcePower() - 400);
			player->sendSystemMessage("Your force drains to keep you hidden");
			reschedule( 12000 );

		}
	}

};

#endif /* FORCECLOAKEVENT_H_ */
