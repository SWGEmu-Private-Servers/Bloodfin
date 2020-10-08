/**
 * file cloakCommand.h
 * author Heat
 * date 11.04.2015
 */

#ifndef CLOAKCOMMAND_H_
#define CLOAKCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "QueueCommand.h"
#include "server/zone/objects/creature/events/CloakEvent.h"

class CloakCommand : public QueueCommand {
public:

  CloakCommand(const String& name, ZoneProcessServer* server) :
    QueueCommand(name, server) {
  }

  int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
    if (!checkStateMask(creature))
      return INVALIDSTATE;

    if (!checkInvalidLocomotions(creature))
      return INVALIDLOCOMOTION;

    CreatureObject* player = cast<CreatureObject*>(creature);

    //Check for ranger
    /*
     * If you don't have Master Ranger
     * send error message
     */
    if (!creature->hasSkill("outdoors_ranger_master"))
      return GENERALERROR;

    int nSkill = 0;
    if (creature->hasSkill("force_title_jedi_rank_02"))
      nSkill += 1;

    ZoneServer* zserv = creature->getZoneServer();
    PlayerObject* jedi = creature->getPlayerObject();

    if (nSkill > 0){
      creature->sendSystemMessage("You are trying to use a top level skill while being invested in jedi skills, "
        "you must devote yourself to science and not mystic arts before using this command.");
      return GENERALERROR;
    }

    Reference<Task*> task = player->getPendingTask("cloakevent");

    if (task = NULL) {
      player->sendSystemMessage("You can not focus on your task, you have tried to do too much");
      return GENERALERROR;
    }

    if (player->isDead() || player->isIncapacitated()){
      player->sendSystemMessage("You cant hide yourself on your back cowboy");
      return GENERALERROR;
    }
    if (player->getParent() != NULL || player->isInCombat()) {
      player->sendSystemMessage("You cant vanish in combat");
      return GENERALERROR;
    }
    Reference<CloakEvent*> cloakTask = new CloakEvent(player);
    CloseObjectsVector* vec = (CloseObjectsVector*)player->getCloseObjects();
    SortedVector<ManagedReference<QuadTreeEntry*> > closeObjects;

    if (vec == NULL)
      return GENERALERROR;

    closeObjects.removeAll(vec->size(), 10);
    vec->safeCopyTo(closeObjects);
    ManagedReference<Zone*> zone = player->getZone();
    creature->playEffect("clienteffect/pl_force_speed_self.cef", "");
    player->getZone()->getInRangeObjects(player->getWorldPositionX(), player->getWorldPositionY(), 40, &closeObjects, true);
    PlayClientEffectLoc* rangercloakLoc = new PlayClientEffectLoc("clienteffect/pl_force_speed_self.cef", player->getZone()->getZoneName(), player->getPositionX(), player->getPositionZ(), player->getPositionY());
    player->broadcastMessage(rangercloakLoc, true);
    player->addPendingTask("cloakevent", cloakTask, 4600);
    player->sendSystemMessage("You call upon your Master Ranger ability");

    if (zone == NULL)
      return GENERALERROR;

    Locker zoneLocker(zone);

    if (!player->isInvisible()) {
      SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();

      for (int i = 0; i < closeObjects->size(); ++i) {
        SceneObject* scno = cast<SceneObject*>(closeObjects->get(i).get());

        if (scno != player && !scno->isBuildingObject())
          scno->notifyDissapear(player);
      }

      player->setInvisible(true);
      player->setSpeedMultiplierMod(0.70f);
      player->setAccelerationMultiplierMod(0.70f);
    }
    else {
      player->setInvisible(false);
      player->setSpeedMultiplierMod(1.0f);
      player->setAccelerationMultiplierMod(1.0f);
      player->setPvpStatusBitmask(CreatureFlag::PLAYER);
      SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();

      for (int i = 0; i < closeObjects->size(); ++i) {
        SceneObject* scno = cast<SceneObject*>(closeObjects->get(i).get());

        if (scno != player && !scno->isBuildingObject())
          scno->notifyInsert(player);
      }
    }

    UpdatePVPStatusMessage* mess = new UpdatePVPStatusMessage(player, player->getPvpStatusBitmask());
    player->broadcastMessage(mess, true, false);

    return SUCCESS;
  }
};

#endif //CLOAKCOMMAND_H_
