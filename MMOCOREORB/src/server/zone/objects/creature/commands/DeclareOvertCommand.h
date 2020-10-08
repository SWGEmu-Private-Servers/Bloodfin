/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef DECLAREOVERTCOMMAND_H_
#define DECLAREOVERTCOMMAND_H_

#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/creature/commands/CombatQueueCommand.h"
#include "server/zone/objects/scene/SceneObject.h"

class DeclareOvertCommand : public QueueCommand {
public:

	DeclareOvertCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		CreatureObject* player = cast<CreatureObject*>(creature);
		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

		if (playerObject != NULL) {
			if (playerObject->getFactionStatus() == FactionStatus::ONLEAVE)
				playerObject->setFactionStatus(FactionStatus::OVERT);
			else if (playerObject->getFactionStatus() == FactionStatus::COVERT)
				playerObject->setFactionStatus(FactionStatus::OVERT);
		}

		return SUCCESS;
	}

};

#endif //DECLAREOVERTCOMMAND_H_
