/*
 * ResarmMenuComponent.cpp
 *
 *  Created on: 08/19/2015
 *      Author: Heat
 */

#include "ResarmMenuComponent.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/CreatureAttribute.h"
#include "server/zone/objects/tangible/wearables/WearableObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/scene/components/ObjectMenuComponent.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/tangible/components/ResarmDataComponent.h"
#include "server/zone/packets/object/PlayClientEffectObjectMessage.h"
#include "server/zone/objects/scene/SceneObject.h"



void ResarmMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player) {

	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);

	TangibleObject* ring = cast<TangibleObject*>(sceneObject);

	if (ring == NULL)
		return;

	ResarmDataComponent* data = cast<ResarmDataComponent*>(ring->getDataObjectComponent()->get());

	if (data == NULL || !data->isResarmData())
		return;

	if (data->getCharges() > 0)
		menuResponse->addRadialMenuItem(20, 3, "Release the Infusion of Life");

}

int ResarmMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* player, byte selectedID) {

	if (selectedID == 20) {

		if (!sceneObject->isASubChildOf(player))
			return 0;

		WearableObject* wearable = cast<WearableObject*>(sceneObject);

		if (wearable == NULL)
			return 0;

		ResarmDataComponent* data = cast<ResarmDataComponent*>(wearable->getDataObjectComponent()->get());

		if (data == NULL || !data->isResarmData())
			return 0;

		int charges = data->getCharges();

		if (charges <= 0)
			return 0;

		if (!wearable->isEquipped()) {
			player->sendSystemMessage("This arm must be attached to your body to use its power");
			return 0;
		}


		if (!player->isDead()) {
			player->sendSystemMessage("You are still alive and this would have no effect on you.");
			String hardpoint = "hold_r";

			PlayClientEffectObjectMessage* effect = new PlayClientEffectObjectMessage(player, "clienteffect/pl_force_healing.cef", hardpoint);
			player->broadcastMessage(effect, true);
			return 0;
		}

		if (!player->checkCooldownRecovery("life_infusion")) {
			Time* timeRemaining = player->getCooldownTime("life_infusion");
			StringIdChatParameter cooldown("quest/hero_of_tatooine/system_messages", "restore_not_yet");
			cooldown.setTO(getCooldownString(timeRemaining->miliDifference() * -1));
			player->sendSystemMessage(cooldown);
			return 0;
		}

		PlayerManager* pm = player->getZoneServer()->getPlayerManager();

		player->setPosture(CreaturePosture::UPRIGHT);
		pm->enhanceCharacter(player);

		player->healDamage(player, CreatureAttribute::HEALTH, 6200);
		player->healDamage(player, CreatureAttribute::ACTION, 6200);
		player->healDamage(player, CreatureAttribute::MIND, 6200);

		data->setCharges(charges - 1);

		String hardpoint = "hold_r";

		PlayClientEffectObjectMessage* effect = new PlayClientEffectObjectMessage(player, "clienteffect/pl_force_healing.cef", hardpoint);
		player->broadcastMessage(effect, true);

		player->sendSystemMessage("The arm of A'Sharad Hetts has returned your target to life");
		player->addCooldown("life_infusion", 3 * 3600 * 1000);

		// remove pvp death debuff
		{
			uint32 buffCRC = String("pvpDeathDebuff").hashCode();
			if (player->hasBuff(buffCRC))
			{
				player->removeBuff(buffCRC);
			}
		}

		return 0;
	} else {
		return TangibleObjectMenuComponent::handleObjectMenuSelect(sceneObject, player, selectedID);
	}

}

String ResarmMenuComponent::getCooldownString(uint32 delta) {

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


