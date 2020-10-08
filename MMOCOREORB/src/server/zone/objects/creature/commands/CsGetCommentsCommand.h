/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CSGETCOMMENTSCOMMAND_H_
#define CSGETCOMMENTSCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class CsGetCommentsCommand : public QueueCommand {
public:

	CsGetCommentsCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		int nSkill = 0;

		if (creature->hasSkill("admin_server_admin_03"))
			nSkill += 1;


		ZoneServer* zserv = creature->getZoneServer();

		if (nSkill < 1){
			creature->sendSystemMessage("Customer Service Ticket System not active yet");
	    	return INVALIDPARAMETERS;
		}

		ManagedReference<SceneObject* > object = server->getZoneServer()->getObject(target);

		CreatureObject* player = cast<CreatureObject*>(creature);

			Zone* zone = zserv->getZone("kashyyyk");

			String tut = "object/building/general/cave_03_damprock.iff";
			String cell = "object/cell/cell.iff";

			Reference<BuildingObject*> kashyyyk = zserv->createObject(tut.hashCode(), 1).castTo<BuildingObject*>();
			kashyyyk->createCellObjects();
			kashyyyk->setPublicStructure(true);

			float x = creature->getPositionX();
			float y = creature->getPositionY();
			float z = creature->getPositionZ();

			kashyyyk->initializePosition(x,y,z);

			zone->transferObject(kashyyyk, -1, true);

			SceneObject* cellTut = kashyyyk->getCell(11);

			SceneObject* cellTutPlayer = kashyyyk->getCell(1);

/*			Reference<SceneObject*> travelTutorialTerminal = server->createObject((uint32)String("object/tangible/beta/beta_terminal_warp.iff").hashCode(), 1);

			SceneObject* cellTut = tatooine->getCell(1);
			cellTut->transferObject(travelTutorialTerminal, -1);

			travelTutorialTerminal->initializePosition(27.0f, -3.5f, -168.0f); */

			PlayerObject* ghost = player->getPlayerObject();
			ghost->setSavedTerrainName(zone->getZoneName());
			ghost->setSavedParentID(cellTutPlayer->getObjectID());

			kashyyyk->updateToDatabase();


		return SUCCESS;
	}

};

#endif //CSGETCOMMENTSCOMMAND_H_
