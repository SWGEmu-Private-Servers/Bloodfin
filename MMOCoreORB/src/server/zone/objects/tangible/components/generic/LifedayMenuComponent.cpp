/*
 * LifedayMenuComponent.cpp
 *
 *  Created on: 12/28/2015
 *      Author: Heat
 */

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "LifedayMenuComponent.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/scene/SceneObjectType.h"
#include "server/zone/objects/tangible/TangibleObject.h"
#include "server/zone/managers/loot/LootManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/packets/player/PlayMusicMessage.h"

void LifedayMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {
	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

}

int LifedayMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {
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

		sceneObject->destroyObjectFromWorld(true);
		sceneObject->destroyObjectFromDatabase(true);
		StringBuffer effect;
		effect << "clienteffect/pt_lifeday_forrest_grow"<< ".cef";
		player->playEffect(effect.toString(), "");
		PlayMusicMessage* pmm = new PlayMusicMessage("sound/music_celebration_a_loop.snd");
		player->sendMessage(pmm);

		player->sendSystemMessage("Its a Lifeday Celebration!");
		player->playEffect("clienteffect/droid_effect_dry_ice.cef");
		player->playEffect("clienteffect/droid_effect_jawa_dance.cef");
		player->playEffect("clienteffect/droid_effect_confetti.cef");
		player->playEffect("clienteffect/droid_effect_electric_fog.cef");
		player->playEffect("clienteffect/droid_effect_mind_bloom.cef");
		player->playEffect("clienteffect/droid_effect_doves.cef");
		player->playEffect("clienteffect/droid_effect_bubbles.cef");
		lootManager->createLoot(inventory, "lifeday_crate", 1);


		return 0;
	}

	return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
}
