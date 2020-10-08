/*
 * PartyCrateMenuComponent.cpp
 *
 *  Created on: 05/02/2014
 *      Author: Heat
 */

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "RemoteWookieeHoloMenuComponent.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/scene/SceneObjectType.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/managers/loot/LootManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/packets/player/PlayMusicMessage.h"

void RemoteWookieeHoloMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {
	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

}

int RemoteWookieeHoloMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {
	if (!sceneObject->isTangibleObject())
		return 0;

	if (!player->isPlayerCreature())
		return 0;

	if (!sceneObject->isASubChildOf(player))
		return 0;

	if (selectedID == 20) {
		ManagedReference<LootManager*> lootManager = sceneObject->getZone()->getZoneServer()->getLootManager();

		if (lootManager == NULL)
			return 0;

		ManagedReference<SceneObject*> inventory = player->getSlottedObject("inventory");


		int rand = System::random(100);
		if (rand >= 50) {
			player->sendSystemMessage("You feel the spirit of the Wookiees in you!");
			player->setAlternateAppearance("object/creature/player/shared_wookiee_male.iff", true);


		}else {
			player->setAlternateAppearance("", true);
		}

		return 0;
	}

	return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
}
