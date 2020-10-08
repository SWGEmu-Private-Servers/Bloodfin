/*
 * DespawnCreatureTask.h
 *
 *  Created on: 2/16/2015
 *      Author: G'Vidon
 */

#ifndef DISMOUNTDELAYEVENT_H_
#define DISMOUNTDELAYEVENT_H_

#include "engine/engine.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"
#include "server/zone/packets/scene/UpdateTransformMessage.h"


class DismountDelayEvent: public Task {
   ManagedReference<CreatureObject*> player;



public:
  DismountDelayEvent(CreatureObject* pl) {
        player = pl;

        }

  void run() {

			  if (player == NULL)

			   return;

                Locker playerLocker(player);

                PlayerObject* targetGhost = player->getPlayerObject();

                try {

                if (player->isOnline() && !targetGhost->isLoggingOut()) {
                    player->removePendingTask("dismountdelayevent");


                    ManagedReference<Zone*> zone = player->getZone();

                if (zone == NULL)

                return;


                Locker zoneLocker(zone);



                if (!player->isInvisible()) {


                    SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();


                for (int i = 0; i < closeObjects->size(); ++i) {

                	SceneObject* scno = cast<SceneObject*>( closeObjects->get(i).get());

                if (scno != player && !scno->isBuildingObject())
                    scno->notifyInsert(player);

                  }
                }
    			UpdateTransformMessage* msg = new UpdateTransformMessage(player);
    			player->broadcastMessage(msg, true);
    //            	zone->transferObject(player, -1, false);
   //                UpdatePVPStatusMessage* mess = new UpdatePVPStatusMessage(player, player->getPvpStatusBitmask());

   //                player->broadcastMessage(mess, true, false);

                  }


                } catch (Exception& e) {


                  player->error("unreported exception caught in DismountDelayEvent::activate");

                }

        }


};


 


#endif /* DISMOUNTDELAYEVENT_H_ */


