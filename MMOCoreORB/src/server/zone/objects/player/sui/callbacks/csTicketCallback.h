/* Author: Heat
 *
 * 8/20/2015
 *
 */

#ifndef CSTICKETCALLBACK
#define CSTICKETCALLBACK

#include "server/chat/ChatManager.h"
#include "server/zone/managers/guild/GuildManager.h"
#include "server/zone/objects/guild/GuildObject.h"
#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/player/sui/callbacks/csTicketCallback.h"
#include "server/zone/objects/player/sui/inputbox/SuiInputBox.h"
#include "server/zone/ZoneServer.h"

class csTicketCallback : public SuiCallback {

public:
	csTicketCallback(ZoneServer* serv) : SuiCallback(serv) {

	}

  void run(CreatureObject* creature, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
    bool cancelPressed = (eventIndex == 1);

		ManagedReference<SceneObject*> suiObject = sui->getUsingObject();
		CreatureObject* player = cast<CreatureObject*>(suiObject.get());

		if (!player)
		{
			return;
		}

		String message = args->get(0).toString();

		if(!sui->isInputBox() || cancelPressed) {
			player->sendSystemMessage("You have cancelled your Ticket");
			return;
		}

//		if(NameManager::instance()->isProfane(message)) {
//			player->sendSystemMessage("No profanity in your Ticket please, calm down and try again =)");
//			return;
//		}

		ManagedReference<ChatManager*> chatManager = server->getChatManager();
		if (!chatManager)
		{
			return;
		}

		ManagedReference<GuildManager*> guildManager = server->getGuildManager();
		if (!guildManager)
		{
			return;
		}

		const String staffGuildAbbrev = "STAFF";
		GuildObject* staffGuild = guildManager->getGuildFromAbbrev(staffGuildAbbrev);
		if (!staffGuild)
		{
			return;
		}

		const UnicodeString subject = ("OPEN TICKET..." + player->getFirstName());
		const UnicodeString body = ("Player writes:....." + message);

		// send mail to staff guild
		GuildMemberList* memberList = staffGuild->getGuildMemberList();
		if (!memberList)
		{
			return;
		}

		for (int i = 0; i < memberList->size(); ++i) {
			GuildMemberInfo* gmi = &memberList->get(i);

			if (gmi == NULL)
				continue;

			ManagedReference<SceneObject*> obj = server->getObject(gmi->getPlayerID());

			if (obj == NULL || !obj->isPlayerCreature())
				continue;

			CreatureObject* recipient = cast<CreatureObject*>( obj.get());

			const String senderName =  "Player Ticket: " + player->getFirstName();
			chatManager->sendMail(senderName, subject, body, recipient->getFirstName());
		}

		// send copy to player
		chatManager->sendMail("Your Ticket is Submitted", subject, body, player->getFirstName());

		player->sendSystemMessage("Your Ticket has been submitted and will be responded to in the order in which it was recieved.");
	}
};

#endif
