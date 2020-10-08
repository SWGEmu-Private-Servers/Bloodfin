/*
 * knightTrialMenuComponent.h
 *
 *  Created on: 05/18/2014
 *      Author: Skyyyr Death
 */

#include "LightFrsMenuComponent.h"

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/object/ObjectMenuResponse.h"
#include "server/zone/objects/player/sui/messagebox/SuiMessageBox.h"
#include "server/zone/managers/skill/SkillManager.h"
#include "server/zone/packets/player/PlayMusicMessage.h"
#include "server/zone/managers/creature/CreatureManager.h"
#include "server/chat/ChatManager.h"

#include "server/zone/ZoneServer.h"




void LightFrsMenuComponent::fillObjectMenuResponse(SceneObject* sceneObject, ObjectMenuResponse* menuResponse, CreatureObject* player){
	TangibleObjectMenuComponent::fillObjectMenuResponse(sceneObject, menuResponse, player);
	ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
	int ThielDead = 1;
	/*
	if (player->getScreenPlayState("knightTrial") >= 0)
		ThielDead += 0;
		*/

	if (ghost->getJediState() == 2 && player->isRebel() && ThielDead == 0) {
	menuResponse->addRadialMenuItem(213, 3, "@jedi_trials:button_lightside");
	}
	if (ghost->getJediState() >= 2 && player->isRebel() && ThielDead == 1 && !player->hasSkill("force_rank_light_novice")) {
	menuResponse->addRadialMenuItem(216, 3, "Apply for Light Jedi Knight.");
	}
	//if (ghost->getJediState() == 4 && player->isImperial() && ThielDead == 1 && !player->hasSkill("force_rank_light_novice")) {
	//	menuResponse->addRadialMenuItem(216, 3, "Apply for Light Jedi Knight.");
	//}
	if (ghost->getJediState() == 4 && player->isRebel() && ThielDead == 1 && player->hasSkill("force_rank_light_novice")){
		menuResponse->addRadialMenuItem(230, 3, "Request Promotion");


	}

}

int LightFrsMenuComponent::handleObjectMenuSelect(SceneObject* sceneObject, CreatureObject* creature, byte selectedID) {

	if (!creature->checkCooldownRecovery("frs_term")) {
				StringIdChatParameter stringId;

				Time* cdTime = creature->getCooldownTime("frs_term");


				int timeLeft = floor((float)cdTime->miliDifference() / 1000) *-1;

				stringId.setStringId("@innate:vit_wait");
				stringId.setDI(timeLeft);
				creature->sendSystemMessage(stringId);
				return 0;}

	//Check for kneeling
	if (creature->getPosture() != CreaturePosture::CROUCHED && !creature->hasSkill("force_rank_light_novice")){
		creature->sendSystemMessage("You feel your knees start to buckle... Forcing you to kneel.");
		creature->setPosture(CreaturePosture::CROUCHED);
		return 0;
	}

	//setting ghost
	ManagedReference<PlayerObject*> ghost = creature->getPlayerObject();
	PlayerObject* player;
	ManagedReference<SceneObject*> inventory = creature->getSlottedObject("inventory");
	//Check if inventory is full.
  if (inventory->isContainerFullRecursive()) {
		creature->sendSystemMessage("@jedi_spam:inventory_full_jedi_robe"); //	You have too many items in your inventory. In order to get your Padawan Robe you must clear out at least one free slot.
		return 0;
	}

	if (ghost == NULL)
		return 0;


	//this is for NON RANKED
	if (selectedID == 213) {
		//Check For non jedi skills
		int nSkill = 0;
		if (creature->hasSkill("combat_brawler_novice"))
			nSkill += 1;
		if (creature->hasSkill("combat_marksman_novice"))
			nSkill += 1;
		if (creature->hasSkill("social_entertainer_novice"))
			nSkill += 1;
		if (creature->hasSkill("crafting_artisan_novice"))
			nSkill += 1;
		if (creature->hasSkill("outdoors_scout_novice"))
			nSkill += 1;

		//If you do... get this.
		if (nSkill > 0){
			ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);
			box->setPromptTitle("The Light Jedi Enclave");
			box->setPromptText("The Light Jedi Order has declined your request due to your current skill set. "
					"Come back when you have more knowledge on Jedi.");
			ghost->addSuiBox(box);
			creature->sendMessage(box->generateMessage());
		}
		//No non Jedi skills... here's your quest.
		if (nSkill <= 0) {
			ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);
			box->setPromptTitle("The Light Jedi Enclave");
			box->setPromptText("The Light Jedi Order has Accepted your request to join the ranks,"
					" to prove your worth the Order has requested the removal of a Jedi that goes by the name Thiel."
					" Come back with his head and you'll have your seat on the Council.");
			ghost->addSuiBox(box);
			creature->sendMessage(box->generateMessage());
			creature->setScreenPlayState("knightTrial", 1);//testing purposes!
		}
	}

	if (selectedID == 216) {
		//setup For non jedi skills
		int nSkill = 0;
		if (creature->hasSkill("combat_brawler_novice"))
			nSkill += 1;
		if (creature->hasSkill("combat_marksman_novice"))
			nSkill += 1;
		if (creature->hasSkill("social_entertainer_novice"))
			nSkill += 1;
		if (creature->hasSkill("crafting_artisan_novice"))
			nSkill += 1;
		if (creature->hasSkill("outdoors_scout_novice"))
			nSkill += 1;

		ZoneServer* zserv = creature->getZoneServer();
		PlayerObject* jedi = creature->getPlayerObject();

		//Check for non jedi skills.
		if (nSkill > 0){
			ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);
			box->setPromptTitle("The Light Jedi Enclave");
			box->setPromptText("You have disgraced the order by using petty skills not meant from Jedi, "
					"you will be stripped of all rank and kicked out of the order if you do it again.");
			ghost->addSuiBox(box);
			creature->sendMessage(box->generateMessage());
			creature->setPosture(CreaturePosture::KNOCKEDDOWN);
			creature->setDizziedState(10);
			creature->setScreenPlayState("warning", 1);
		}
		//You don't have jedi skills... congrats here's knight!
		if (nSkill <= 0 ) {//&& creature->getScreenPlayState("knightTrial") >= 2) {
			ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);
			box->setPromptTitle("The Light Jedi Enclave");
			box->setPromptText("The Light Jedi Order is pleased with your actions. You have earned your seat on our Council!");
			ghost->addSuiBox(box);
			creature->sendMessage(box->generateMessage());
			SkillManager::instance()->awardSkill("force_title_jedi_rank_03", creature, true, true, true);
			SkillManager::instance()->awardSkill("force_rank_light_novice", creature, true, true, true);
			String KnightLightRobe = "object/tangible/wearables/robe/robe_jedi_light_s01.iff";
			ManagedReference<SceneObject*> knightLightRobe = zserv->createObject(KnightLightRobe.hashCode(), 1);
			knightLightRobe->sendTo(creature, true);
			inventory->transferObject(knightLightRobe, -1);
			creature->addCooldown("frs_term", 1 * 6600);
			creature->playEffect("clienteffect/entertainer_dazzle_level_2.cef", ""); // Not sure if it's the right one for this.
			ghost->setJediState(4);
			jedi->setFactionStatus(2);

		}
	}

	if (selectedID == 230) {
		//setup For non jedi skills
		int nSkill = 0;
		if (creature->hasSkill("combat_brawler_novice"))
			nSkill += 1;
		if (creature->hasSkill("combat_marksman_novice"))
			nSkill += 1;
		if (creature->hasSkill("social_entertainer_novice"))
			nSkill += 1;
		if (creature->hasSkill("crafting_artisan_novice"))
			nSkill += 1;
		if (creature->hasSkill("outdoors_scout_novice"))
			nSkill += 1;
		int rankXp = ghost->getExperience("force_rank_xp");

		ZoneServer* zserv = creature->getZoneServer();
		if (creature->hasSkill("force_rank_light_novice") && !creature->hasSkill("force_rank_light_rank_01") && rankXp >= 10000 && nSkill < 1) {
			SkillManager::instance()->awardSkill("force_rank_light_rank_01", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -10000);
			String KnightLightRobe1 = "object/tangible/wearables/robe/robe_jedi_light_s02.iff";
			ManagedReference<SceneObject*> knightLightRobe1 = zserv->createObject(KnightLightRobe1.hashCode(), 1);
			knightLightRobe1->sendTo(creature, true);
			inventory->transferObject(knightLightRobe1, -1);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_01") && !creature->hasSkill("force_rank_light_rank_02") && rankXp >= 20000 && nSkill < 1) {
			SkillManager::instance()->awardSkill("force_rank_light_rank_02", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -20000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_02") && !creature->hasSkill("force_rank_light_rank_03") && rankXp >= 30000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_03", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -30000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_03") && !creature->hasSkill("force_rank_light_rank_04") && rankXp >= 40000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_04", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -40000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_04") && !creature->hasSkill("force_rank_light_rank_05") && rankXp >= 60000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_05", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -60000);
			String KnightLightRobe2 = "object/tangible/wearables/robe/robe_jedi_light_s03.iff";
			ManagedReference<SceneObject*> knightLightRobe2 = zserv->createObject(KnightLightRobe2.hashCode(), 1);
			knightLightRobe2->sendTo(creature, true);
			inventory->transferObject(knightLightRobe2, -1);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_05") && !creature->hasSkill("force_rank_light_rank_06") && rankXp >= 80000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_06", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -80000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_06") && !creature->hasSkill("force_rank_light_rank_07") && rankXp >= 100000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_07", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -100000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_07") && !creature->hasSkill("force_rank_light_rank_08") && rankXp >= 150000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_08", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -150000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_08") && !creature->hasSkill("force_rank_light_rank_09") && rankXp >= 200000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_09", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -200000);
			String KnightLightRobe3 = "object/tangible/wearables/robe/robe_jedi_light_s04.iff";
			ManagedReference<SceneObject*> knightLightRobe3 = zserv->createObject(KnightLightRobe3.hashCode(), 1);
			knightLightRobe3->sendTo(creature, true);
			inventory->transferObject(knightLightRobe3, -1);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_09") && !creature->hasSkill("force_rank_light_rank_10") && rankXp >= 300000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_rank_10", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -300000);
			creature->playEffect("clienteffect/pl_force_absorb_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");
			return 0;
		}

		if (creature->hasSkill("force_rank_light_rank_10") && !creature->hasSkill("force_rank_light_master") && rankXp >= 500000 && nSkill < 1){
			SkillManager::instance()->awardSkill("force_rank_light_master", creature, true, true, true);
			ghost->addExperience("force_rank_xp", -500000);
			creature->playEffect("clienteffect/pl_force_speed_self.cef", "");
			creature->playEffect("clienteffect/item_ring_hero_mark.cef", "");

			return 0;
		}

	}

	return 0;
}


/*





		//}
	}

	if (selectedID == 230) {
				ZoneServer* zserv = creature->getZoneServer();
				PlayerObject* jedi = creature->getPlayerObject();
						ManagedReference<SceneObject*> inventory = creature->getSlottedObject("inventory");
						SkillManager::instance()->awardSkill("force_title_jedi_rank_03", creature, true, true, true);
						SkillManager::instance()->awardSkill("force_rank_light_novice", creature, true, true, true);
						ManagedReference<SuiMessageBox*> box2 = new SuiMessageBox(creature, SuiWindowType::NONE);
						String KnightRobe = "object/tangible/wearables/robe/robe_jedi_light_s01.iff";
						ManagedReference<SceneObject*> knightRobe = zserv->createObject(KnightRobe.hashCode(), 1);
						knightRobe->sendTo(creature, true);
						inventory->transferObject(knightRobe, -1);
						creature->playEffect("clienteffect/entertainer_dazzle_level_2.cef", ""); // Not sure if it's the right one for this.
						ghost->setJediState(4);
						jedi->setFactionStatus(2);
						box2->setPromptTitle("@jedi_trials:knight_trials_title"); // Jedi Trials
						box2->setPromptText("@jedi_trials:knight_trials_completed_light");
						ghost->addSuiBox(box2);
						creature->sendMessage(box2->generateMessage());
					}


			if (selectedID == 221) {
						ZoneServer* zserv = creature->getZoneServer();
						ManagedReference<SceneObject*> inventory = creature->getSlottedObject("inventory");
						PlayerObject* jedi = creature->getPlayerObject();
						SkillManager::instance()->awardSkill("force_title_jedi_rank_03", creature, true, true, true);
						SkillManager::instance()->awardSkill("force_rank_light_novice", creature, true, true, true);
						String KnightLightRobe = "object/tangible/wearables/robe/robe_jedi_light_s01.iff";
						ManagedReference<SceneObject*> knightLightRobe = zserv->createObject(KnightLightRobe.hashCode(), 1);
						ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, SuiWindowType::NONE);
						knightLightRobe->sendTo(creature, true);
						inventory->transferObject(knightLightRobe, -1);
						creature->playEffect("clienteffect/entertainer_dazzle_level_2.cef", ""); // Not sure if it's the right one for this.
						ghost->setJediState(8);
						jedi->setFactionStatus(2);
						box->setPromptTitle("@jedi_trials:knight_trials_title"); // Jedi Trials
						box->setPromptText("@jedi_trials:knight_trials_completed_light");
						ghost->addSuiBox(box);
						creature->sendMessage(box->generateMessage());
					}
	return 0;

}
*/

