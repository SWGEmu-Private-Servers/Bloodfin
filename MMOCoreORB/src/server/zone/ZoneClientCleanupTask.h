#ifndef _ZONECLIENTCLEANUPTASK_H_
#define _ZONECLIENTCLEANUPTASK_H_

#include "engine/engine.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/player/PlayerManager.h"

class ZoneClientCleanupTask : public Task {
	ManagedReference<ZoneServer*> zoneServer;
public:
	ZoneClientCleanupTask(ZoneServer* zoneServer) {
		this->zoneServer = zoneServer;
	}

	void run() {
		if (zoneServer == NULL) {
			return;
		}
		ManagedReference<PlayerManager*> playerManager = zoneServer->getPlayerManager();

		if (playerManager == NULL) {
			return;
		}
		
		playerManager->cleanupUnseenClientSessions();	
	}
};


#endif
