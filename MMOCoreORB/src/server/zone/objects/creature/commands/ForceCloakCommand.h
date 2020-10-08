/**
 * file ForceCloakCommand.h
 * author Heat
 * date 11.04.2015
 */

#ifndef FORCECLOAKCOMMAND_H_
#define FORCECLOAKCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "ForcePowersQueueCommand.h"
#include "server/zone/objects/creature/events/ForceCloakEvent.h"

class ForceCloakCommand : public ForcePowersQueueCommand {
public:

  ForceCloakCommand(const String& name, ZoneProcessServer* server) :
    ForcePowersQueueCommand(name, server) {
  }

  int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
    if (!checkStateMask(creature))
      return INVALIDSTATE;

    if (!checkInvalidLocomotions(creature))
      return INVALIDLOCOMOTION;

    CreatureObject* player = cast<CreatureObject*>(creature);

    if (!creature->hasSkill("force_discipline_powers_master"))
      return GENERALERROR;

    int nSkill = 0;
    if (creature->hasSkill("combat_brawler_novice"))
      nSkill += 1;
    if (creature->hasSkill("combat_marksman_novice"))
      nSkill += 1;
    if (creature->hasSkill("social_entertainer_novice"))
      nSkill += 1;
    if (creature->hasSkill("crafting_artisan_novice"))
      nSkill += 1;
    if (creature->hasSkill("outdoors_scout_novice"))
      nSkill += 1;

    ZoneServer* zserv = creature->getZoneServer();
    PlayerObject* jedi = creature->getPlayerObject();

    if (nSkill > 0){
      creature->sendSystemMessage("You are trying to use a top level skill while still being invested in other non jedi skills, "
        "you must master the force in all its aspects before using this command.");
      return GENERALERROR;
    }

    Reference<Task*> task = player->getPendingTask("forcecloakevent");

    if (task = NULL) {
      player->sendSystemMessage("You can not focus on your task, you have tried to do too much");
      return GENERALERROR;
    }

    ManagedReference<PlayerObject*> playerObject = player->getPlayerObject();
    if (playerObject->getForcePower() < 700) {
      player->sendSystemMessage("You dont have the force power to cloak yourself");
      return GENERALERROR;
    }
    if (player->isDead() || player->isIncapacitated()){
      player->sendSystemMessage("You cant hide yourself on your back cowboy");
      return GENERALERROR;
    }
    if (player->getParent() != NULL || player->isInCombat()) {
      player->sendSystemMessage("You cant cloak right now");
      return GENERALERROR;
    }

    Reference<ForceCloakEvent*> forcecloakTask = new ForceCloakEvent(player);
    CloseObjectsVector* vec = (CloseObjectsVector*)player->getCloseObjects();
    SortedVector<ManagedReference<QuadTreeEntry*> > closeObjects;

    if (vec == NULL)
      return GENERALERROR;

    closeObjects.removeAll(vec->size(), 10);
    vec->safeCopyTo(closeObjects);
    ManagedReference<Zone*> zone = player->getZone();
    creature->playEffect("clienteffect/death_trooper_anti_virus.cef", "");
    player->getZone()->getInRangeObjects(player->getWorldPositionX(), player->getWorldPositionY(), 40, &closeObjects, true);
    PlayClientEffectLoc* cloakLoc = new PlayClientEffectLoc("clienteffect/death_trooper_anti_virus.cef", player->getZone()->getZoneName(), player->getPositionX(), player->getPositionZ(), player->getPositionY());
    player->broadcastMessage(cloakLoc, true);
    player->addPendingTask("forcecloakevent", forcecloakTask, 4600);
    player->sendSystemMessage("the force surges through you");
    //		player->playEffect("clienteffect/pl_force_absorb_self.cef");
    //		ManagedReference<Zone*> zone = player->getZone();

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
      player->setSpeedMultiplierMod(0.50f);
      player->setAccelerationMultiplierMod(0.50f);
    }
    else {
      player->setInvisible(false);
      player->setSpeedMultiplierMod(1.0f);
      player->setAccelerationMultiplierMod(1.0f);
      SortedVector<ManagedReference<QuadTreeEntry*> >* closeObjects = player->getCloseObjects();

      for (int i = 0; i < closeObjects->size(); ++i) {
        SceneObject* scno = cast<SceneObject*>(closeObjects->get(i).get());

        if (scno != player && !scno->isBuildingObject())
          scno->notifyInsert(player);
      }
    }

    UpdateTransformMessage* msg = new UpdateTransformMessage(player);
    player->broadcastMessage(msg, true);

    return SUCCESS;
  }
};

#endif //FORCECLOAKCOMMAND_H_
