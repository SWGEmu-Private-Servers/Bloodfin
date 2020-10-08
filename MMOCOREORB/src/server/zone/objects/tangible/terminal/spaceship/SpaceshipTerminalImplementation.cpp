/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.
*/

#include "server/zone/objects/tangible/terminal/spaceship/SpaceshipTerminal.h"
#include "server/zone/objects/creature/CreatureObject.h"

int SpaceshipTerminalImplementation::handleObjectMenuSelect(CreatureObject* player, byte selectedID) {

	StringBuffer c;

 	c << "SpaceshipTerminal::handleObjectMenuSelect selectedID=" << selectedID;
              error(c.toString());

          	player->sendSystemMessage("To get into Space, exit the starport and call your ship from your Datapad. Begin your climb in atmospheric flight, and you will transition into space when you have reached a high enough altitude.");

	return 0;
}
