/*
 * ChannelForceRegenTask.h
 *
 *  Created on: Aug 18, 2011
 *      Author: swgemu
 */

#ifndef JEDIMINDTRICKTASK_H_
#define JEDIMINDTRICKTASK_H_

#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/commands/CombatQueueCommand.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/chat/ChatSystemMessage.h"
#include "server/zone/packets/tangible/UpdatePVPStatusMessage.h"
#include "server/zone/packets/scene/UpdateTransformMessage.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/intangible/ShipControlDevice.h"
#include "server/zone/managers/space/SpaceManager.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/packets/player/PlayMusicMessage.h"
#include "server/chat/ChatManager.h"
#include "engine/engine.h"
#include "server/zone/Zone.h"
#include "server/zone/objects/creature/CreatureObject.h"

class JediMindTrickTask : public Task {

private:
		enum Phase { INITIAL, SECOND, THIRD, FOURTH, FINAL} currentPhase;
		ManagedReference<CreatureObject*> creature;
		ManagedReference<CreatureObject*> creatureTarget;

public:

		JediMindTrickTask(CreatureObject* creo, CreatureObject* creoTarget) : Task() {
			currentPhase = INITIAL;
			creature = creo;  // This is the Attacker
			creatureTarget = creoTarget; // This is the target (one that takes damage)

		}


		void run() {
			Locker lockerC(creature);
			Locker lockerCT(creatureTarget,creature);

			  if (creatureTarget == NULL)

			   return;


			  creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");
			  creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");
			  creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");

				creatureTarget->removePendingTask("JediMindTrickTask");

			ChatManager* chatManager = creatureTarget->getZoneServer()->getChatManager();
			ManagedReference<PlayerObject*> playerObject = creatureTarget->getPlayerObject();
			switch (currentPhase) {
			case INITIAL:
				creatureTarget->sendSystemMessage("You feel delerious");
				if (playerObject != NULL)
					if (playerObject->getFactionStatus() == FactionStatus::OVERT)
						playerObject->setFactionStatus(FactionStatus::ONLEAVE);
				creatureTarget->setSpeedMultiplierMod(0.222f);
				creatureTarget->setAccelerationMultiplierMod(0.222f);


				currentPhase = SECOND;
				creatureTarget->addPendingTask("mindtrick", this, 5600);
				break;
			case SECOND:
				creatureTarget->sendSystemMessage("Your mind has been invaded");

				currentPhase = THIRD;
				creatureTarget->addPendingTask("mindtrick", this, 5600);
				break;
			case THIRD:
				creatureTarget->sendSystemMessage("Your mind has been invaded");

				currentPhase = FOURTH;
				creatureTarget->addPendingTask("mindtrick", this, 5600);
				break;
			case FOURTH:
				creatureTarget->sendSystemMessage("You are regaining your focus");
				currentPhase = FINAL;
				creatureTarget->addPendingTask("mindtrick", this, 5600);
				break;

			case FINAL:
				if (playerObject != NULL)
					if (playerObject->getFactionStatus() == FactionStatus::ONLEAVE)
						playerObject->setFactionStatus(FactionStatus::OVERT);
				creatureTarget->setSpeedMultiplierMod(1.0f);
				creatureTarget->setAccelerationMultiplierMod(1.0f);

				creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");
				creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");
				creatureTarget->playEffect("clienteffect/pl_force_resist_disease_self.cef", "");
				}



			return;
		}

};


#endif /* JEDIMINDTRICKTASK_H_ */
