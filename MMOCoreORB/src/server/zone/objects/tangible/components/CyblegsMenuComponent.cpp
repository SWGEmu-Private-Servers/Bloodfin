/*
 * CyblegsMenuComponent.cpp
 *
 *  Created on: 09/01/2015
 *      Author: Heat
 */

#include "CyblegsMenuComponent.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/tangible/components/CyblegsDataComponent.h"
#include "server/zone/packets/object/PlayClientEffectObjectMessage.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/player/events/RegainConsciousnessRegenTask.h"



void CyblegsMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {
  TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

  TangibleObject* ring = cast<TangibleObject*>(sceneObject);

  if (ring == NULL)
    return;

  CyblegsDataComponent* data = cast<CyblegsDataComponent*>(ring->getDataObjectComponent()->get());

  if (data == NULL || !data->isCyblegsData())
    return;

  if (data->getCharges() > 0)
    menuResponse->addRadialMenuItem(20, 3, "Activate Cybernetic Leg Reactor Overload");
}

int CyblegsMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {
  if (selectedID == 20) {
    if (!sceneObject->isASubChildOf(player))
      return 0;

    WearableObject* wearable = cast<WearableObject*>(sceneObject);

    if (wearable == NULL)
      return 0;

    CyblegsDataComponent* data = cast<CyblegsDataComponent*>(wearable->getDataObjectComponent()->get());

    if (data == NULL || !data->isCyblegsData())
      return 0;

    int charges = data->getCharges();

    if (charges <= 0)
      return 0;

    if (!wearable->isEquipped()) {
      player->sendSystemMessage("These Legs must be attached to your body to use them");
      return 0;
    }

    if (!player->checkCooldownRecovery("cybleg_boost")) {
      Time* timeRemaining = player->getCooldownTime("cybleg_boost");
      StringIdChatParameter cooldown("quest/hero_of_tatooine/system_messages", "restore_not_yet");
      cooldown.setTO(getCooldownString(timeRemaining->miliDifference() * -1));
      player->sendSystemMessage(cooldown);
      return 0;
    }

    PlayerManager* pm = player->getZoneServer()->getPlayerManager();
    ManagedReference<PlayerObject*> playerObject = player->getPlayerObject();

    player->setSpeedMultiplierMod(6.522f);
    player->setAccelerationMultiplierMod(0.522f);

    // BLOODFIN_PUB7_TODO: Add this

     		Reference<RegainConsciousnessRegenTask*> rcTask = new RegainConsciousnessRegenTask(player, playerObject);
     		player->addPendingTask("regainConsciousnessRegenTask", rcTask, (100 * 1000));

    data->setCharges(charges - 1);

    player->playEffect("clienteffect/item_ring_hero_mark.cef", "");
    player->playEffect("clienteffect/trap_electric_01.cef", "");
    player->playEffect("clienteffect/pl_force_run_self.cef", "");
    //		player->broadcastMessage(effect, true);

    player->sendSystemMessage("Cybernetic Legs Reactor Overload Initiated");
    player->addCooldown("cybleg_boost", 3 * 3600 * 1000);

    return 0;
  }
  else {
    return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
  }
}

String CyblegsMenuComponent::getCooldownString(uint32 delta) {
  int seconds = delta / 1000;

  int hours = seconds / 3600;
  seconds -= hours * 3600;

  int minutes = seconds / 60;
  seconds -= minutes * 60;

  StringBuffer buffer;

  if (hours > 0) {
    buffer << hours << " hour";

    if (hours > 1)
      buffer << "s";

    if (minutes > 0 || seconds > 0)
      buffer << ", ";
  }

  if (minutes > 0) {
    buffer << minutes << " minute";

    if (minutes > 1)
      buffer << "s";

    if (seconds > 0)
      buffer << ", ";
  }

  if (seconds > 0) {
    buffer << seconds << " second";

    if (seconds > 1)
      buffer << "s";
  }

  return buffer.toString();
}