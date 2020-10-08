/*
 * FactionManager.cpp
 *
 *  Created on: Mar 17, 2011
 *      Author: crush
 */

#include "FactionManager.h"
#include "FactionMap.h"
#include "server/zone/objects/group/GroupObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/managers/templates/TemplateManager.h"
#include "server/ServerCore.h"
#include "server/db/ServerDatabase.h"

FactionManager::FactionManager() {
	setLoggingName("FactionManager");
	setGlobalLogging(false);
	setLogging(false);
}

void FactionManager::loadData() {
	loadLuaConfig();
	loadFactionRanks();
}

void FactionManager::loadFactionRanks() {
	IffStream* iffStream = TemplateManager::instance()->openIffFile("datatables/faction/rank.iff");

	if (iffStream == NULL) {
		warning("Faction ranks could not be found.");
		return;
	}

	DataTableIff dtiff;
	dtiff.readObject(iffStream);

	factionRanks.readObject(&dtiff);

	delete iffStream;

	info("loaded " + String::valueOf(factionRanks.getCount()) + " ranks", true);
}

void FactionManager::loadLuaConfig() {
	info("Loading config file.", true);

	Lua* lua = new Lua();
	lua->init();

	lua_register(lua->getLuaState(), "addFaction", addFaction);

	//Load the faction manager lua file.
	lua->runFile("scripts/managers/faction_manager.lua");

	delete lua;
	lua = NULL;

}

int FactionManager::addFaction(lua_State* L) {
	String allies = Lua::getStringParameter(L);
	String enemies = Lua::getStringParameter(L);
	String factionName = Lua::getStringParameter(L);

	FactionManager* factionManager = FactionManager::instance();

	FactionMap* factionMap = factionManager->getFactionMap();

	if (factionMap == NULL)
		return 1;

	Faction faction(factionName);
	faction.parseEnemiesFromList(enemies);
	faction.parseAlliesFromList(allies);

	factionMap->addFaction(faction);

	return 0;
}

FactionMap* FactionManager::getFactionMap() {
	return &factionMap;
}

void locRemoveFrsSkills(int rankMod, int rankXp, CreatureObject* jediObject, const String& type)
{
  if (rankMod == 5 && rankXp < 0) { jediObject->removeSkill("force_rank_" + type + "_novice"); }
  SkillModManager::instance()->verifySkillBoxSkillMods(jediObject);
  if (rankMod == 5 && rankXp < 0) { jediObject->removeSkill("force_title_jedi_rank_03"); }
  SkillModManager::instance()->verifySkillBoxSkillMods(jediObject);
  if (rankMod == 8 && rankXp <= 0) { jediObject->removeSkill("force_ rank_" + type + "_rank_01"); }
  if (rankMod == 12 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_02"); }
  if (rankMod == 16 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_03"); }
  if (rankMod == 20 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_04"); }
  if (rankMod == 25 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_05"); }
  if (rankMod == 30 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_06"); }
  if (rankMod == 35 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_07"); }
  if (rankMod == 45 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_08"); }
  if (rankMod == 55 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_09"); }
  if (rankMod == 65 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_rank_10"); }
  if (rankMod == 80 && rankXp <= 0) { jediObject->removeSkill("force_rank_" + type + "_master"); }
  SkillModManager::instance()->verifySkillBoxSkillMods(jediObject);
}

void FactionManager::awardForceRankPoints(TangibleObject* killer, CreatureObject* destructedObject) {
	if (killer->isPlayerCreature() && destructedObject->isPlayerCreature() && destructedObject->hasSkill("force_title_jedi_rank_02")) {
		CreatureObject* killerCreature = cast<CreatureObject*>(killer);
		ManagedReference<PlayerObject*> killerGhost = killerCreature->getPlayerObject();

		ManagedReference<PlayerObject*> killedGhost = destructedObject->getPlayerObject();
		int rankModD = destructedObject->getSkillMod("force_manipulation_dark");
		int rankModL = destructedObject->getSkillMod("force_manipulation_light");
		int rankXp = killedGhost->getExperience("force_rank_xp");
		int rankXpMultiplier = 1;

		if (rankModD > rankModL)
			rankXpMultiplier = (rankModD * .25);
		if (rankModL > rankModD)
			rankXpMultiplier = (rankModL * .25);

 //BOOSTED FOR XP WEEK

    int frsXpGain = rankXpMultiplier * 2000;
    int frsXpLoss = rankXpMultiplier * -1000;

    bool lightJediInKillingGroup = false;
    bool darkJediInKillingGroup = false;
    int groupMembersInRange = 1; // includes the original killer

    // set the flag based on whether or not the killer is a light / dark jedi
    if ( killerCreature->hasSkill("force_rank_light_novice") ) {
      lightJediInKillingGroup |= true;
    }
    else if ( killerCreature->hasSkill("force_rank_dark_novice") ) {
      darkJediInKillingGroup |= true;
    }

    // only award frs xp to group members within 256m
    const float frsXpRange = 256.0f;
   
    // if the killer is in a group we need to determine how many group members are in range
    // as that will determine the amount of frs xp awarded.  We also need to track if there
    // are any light / dark jedi in the group.
    if ( killerCreature->isGrouped() )
    {
      ManagedReference<GroupObject*> group = killerCreature->getGroup();
      int groupSize = group->getGroupSize();

      // iterate over the group members
      for (int i = 0; i < groupSize; i++) {
        ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

        // skip the killer passed in
        if (killerCreature->getObjectID() == groupMember->getObjectID())
          continue;

        // if the group member is in range...
        if (killerCreature->isInRange(groupMember, frsXpRange)) {
          CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

          if ( groupMemberCreature == NULL )
            continue;

          // increment group counter
          groupMembersInRange++;

          // flag based on frs
          if ( groupMemberCreature->hasSkill("force_rank_light_novice") ) {
            lightJediInKillingGroup |= true;
          }
          else if ( groupMemberCreature->hasSkill("force_rank_dark_novice") ) {
            darkJediInKillingGroup |= true;
          }
        }
      }
    }

    // For now, the killing group needs to contain an opposing jedi
    
    const bool bValidFrsKill = ((destructedObject->hasSkill("force_rank_dark_novice") && lightJediInKillingGroup) ||
                                (destructedObject->hasSkill("force_rank_light_novice") && darkJediInKillingGroup));

    if (!bValidFrsKill)
    {
      return;
    }

    const int32 frsEventID = trackFrsKillEvent(destructedObject);

    // determine the amount of frs xp to award..
    const int frsXpToAward = frsXpGain / groupMembersInRange;
    const String fightClubMessage = "Fight Clubbing is not tolerated within the FRS.  You have been penalized.";

    // if the dead jedi is a dark jedi, and there where light jedi involved in the kill...
    if ( destructedObject->hasSkill("force_rank_dark_novice") && lightJediInKillingGroup ) {

      // Handle the killer themselves
      {
        const bool isOpposingJedi = killerCreature->hasSkill("force_rank_light_novice");
        if (isOpposingJedi)
        {
          const bool isFightClubbing = killerGhost->getAccountID() == killedGhost->getAccountID();
          const int actualXpAward = isFightClubbing ? -frsXpGain : frsXpToAward;

          const int fcRankMod = killerCreature->getSkillMod("force_manipulation_light");
          const int fcRankXp = killerGhost->getExperience("force_rank_xp");

          // award xp to killer jedi
          killerGhost->addExperience("force_rank_xp", actualXpAward);
          trackFrsXpChange(frsEventID, killerCreature, actualXpAward);

          trackFrsKillParticipant(frsEventID, killerCreature);

          if (isFightClubbing)
          {
            killerCreature->sendSystemMessage(fightClubMessage);
            locRemoveFrsSkills(fcRankMod, fcRankXp, killerCreature, "light");
          }
        }
        else
        {
          trackFrsKillParticipant(frsEventID, killerCreature);
        }
      }

      // iterate group and award xp to any valid group members
      if ( killerCreature->isGrouped() )
      {
        ManagedReference<GroupObject*> group = killerCreature->getGroup();
        int groupSize = group->getGroupSize();

        // iterate over the group members
        for (int i = 0; i < groupSize; i++) {
          ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

          // skip the killer passed in
          if (killerCreature->getObjectID() == groupMember->getObjectID())
            continue;

          // if the group member is in range...
          if (killerCreature->isInRange(groupMember, frsXpRange)) {
            CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

            if (groupMemberCreature == NULL )
              continue;

            if (!groupMemberCreature->isPlayerCreature())
              continue;

            trackFrsKillParticipant(frsEventID, groupMemberCreature);

            ManagedReference<PlayerObject*> groupMemberGhost = groupMemberCreature->getPlayerObject();
            const bool isOpposingJedi = groupMemberCreature->hasSkill("force_rank_light_novice");

            // only light jedi are valid for a dark jedi kill
            if (isOpposingJedi)
            {
              const bool isFightClubbing = groupMemberGhost->getAccountID() == killedGhost->getAccountID();
              const int actualXpAward = isFightClubbing ? -frsXpGain : frsXpToAward;

              const int fcRankMod = killerCreature->getSkillMod("force_manipulation_light");
              const int fcRankXp = killerGhost->getExperience("force_rank_xp");

              groupMemberGhost->addExperience("force_rank_xp", actualXpAward);
              trackFrsXpChange(frsEventID, groupMemberCreature, actualXpAward);

              if (isFightClubbing)
              {
                killerCreature->sendSystemMessage(fightClubMessage);
                locRemoveFrsSkills(fcRankMod, fcRankXp, groupMemberCreature, "light");
              }
            }
          }
        }
      }

      // remove xp from the dead dark jedi
      killedGhost->addExperience("force_rank_xp", frsXpLoss, true);
      trackFrsXpChange(frsEventID, destructedObject, frsXpLoss);

      locRemoveFrsSkills(rankModD, rankXp, destructedObject, "dark");

		} else if ( destructedObject->hasSkill("force_rank_light_novice") && darkJediInKillingGroup ) {

      // Handle the killer themselves
      {
        const bool isOpposingJedi = killerCreature->hasSkill("force_rank_dark_novice");
        if (isOpposingJedi)
        {
          const bool isFightClubbing = killerGhost->getAccountID() == killedGhost->getAccountID();
          const int actualXpAward = isFightClubbing ? -frsXpGain : frsXpToAward;

          const int fcRankMod = killerCreature->getSkillMod("force_manipulation_dark");
          const int fcRankXp = killerGhost->getExperience("force_rank_xp");

          // award xp to killer jedi
          killerGhost->addExperience("force_rank_xp", actualXpAward);
          trackFrsXpChange(frsEventID, killerCreature, actualXpAward);

          trackFrsKillParticipant(frsEventID, killerCreature);

          if (isFightClubbing)
          {
            killerCreature->sendSystemMessage(fightClubMessage);
            locRemoveFrsSkills(fcRankMod, fcRankXp, killerCreature, "dark");
          }
        }
        else 
        {
          trackFrsKillParticipant(frsEventID, killerCreature);
        }
      }

      // iterate group and award xp to any valid group members
      if ( killerCreature->isGrouped() )
      {
        ManagedReference<GroupObject*> group = killerCreature->getGroup();
        int groupSize = group->getGroupSize();

        // iterate over the group members
        for (int i = 0; i < groupSize; i++) {
          ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

          // skip the killer passed in
          if (killerCreature->getObjectID() == groupMember->getObjectID())
            continue;

          // if the group member is in range...
          if (killerCreature->isInRange(groupMember, frsXpRange)) {
            CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

            if (groupMemberCreature == NULL )
              continue;

            if (!groupMemberCreature->isPlayerCreature())
              continue;

            trackFrsKillParticipant(frsEventID, groupMemberCreature);

            ManagedReference<PlayerObject*> groupMemberGhost = groupMemberCreature->getPlayerObject();
            const bool isOpposingJedi = groupMemberCreature->hasSkill("force_rank_dark_novice");           

            // only light jedi are valid for a dark jedi kill
            if (isOpposingJedi)
            {
              const bool isFightClubbing = groupMemberGhost->getAccountID() == killedGhost->getAccountID();
              const int actualXpAward = isFightClubbing ? -frsXpGain : frsXpToAward;

              const int fcRankMod = killerCreature->getSkillMod("force_manipulation_dark");
              const int fcRankXp = killerGhost->getExperience("force_rank_xp");

              groupMemberGhost->addExperience("force_rank_xp", actualXpAward);
              trackFrsXpChange(frsEventID, groupMemberCreature, actualXpAward);

              if (isFightClubbing)
              {
                killerCreature->sendSystemMessage(fightClubMessage);              
                locRemoveFrsSkills(fcRankMod, fcRankXp, groupMemberCreature, "dark");
              }
            }
          }
        }
      }

      // remove xp from the dead light jedi
      killedGhost->addExperience("force_rank_xp", frsXpLoss, true);
      trackFrsXpChange(frsEventID, destructedObject, frsXpLoss);

      locRemoveFrsSkills(rankModL, rankXp, destructedObject, "light");
		}
	}
}

void FactionManager::awardFactionStanding(CreatureObject* player, const String& factionName, int level) {
	if (player == NULL)
		return;

	ManagedReference<PlayerObject*> ghost = player->getPlayerObject();

	if (!factionMap.contains(factionName))
		return;

	float gain = level;
	float lose = gain * 2;

	Faction faction = factionMap.get(factionName);
	SortedVector<String>* enemies = faction.getEnemies();
	SortedVector<String>* allies = faction.getAllies();

	ghost->decreaseFactionStanding(factionName, lose);

	//Lose faction standing to allies of the creature.
	for (int i = 0; i < allies->size(); ++i) {
		String ally = allies->get(i);

		if ((ally == "rebel" || ally == "imperial")) {
			continue;
		}

		ghost->decreaseFactionStanding(ally, lose);
	}

	bool gcw = false;
	if (factionName == "rebel" || factionName == "imperial") {
		gcw = true;
	}

	//Gain faction standing to enemies of the creature.
	for (int i = 0; i < enemies->size(); ++i) {
		String enemy = enemies->get(i);

		if ((enemy == "rebel" || enemy == "imperial") && !gcw) {
			continue;
		}

		ghost->increaseFactionStanding(enemy, gain);
	}
}

void FactionManager::awardGcwRankPoints(TangibleObject* killer, CreatureObject* destructedObject) {
  if (killer->isPlayerCreature() && destructedObject->isPlayerCreature() && (destructedObject->isImperial() || destructedObject->isRebel())) {
    CreatureObject* killerCreature = cast<CreatureObject*>(killer);

    // no gcw points for jedi
    if (killerCreature->hasSkill("force_title_jedi_rank_02")) {
      return;
    }

    ManagedReference<PlayerObject*> killerGhost = killerCreature->getPlayerObject();
    ManagedReference<PlayerObject*> killedGhost = destructedObject->getPlayerObject();

    int bonusMod = 1;
    if (Zone* zone = killerGhost->getZone()) {

      // TODO: Do we want this to be for ALL of Rori, or just the pvp areas?
      if (zone->getZoneName().contains("rori")) {
        bonusMod += 1;
      }
    }

    int gcwXpGain = bonusMod * 2000;
    int gcwXpLoss = bonusMod * -500;

    bool rebelInKillingGroup = false;
    bool imperialInKillingGroup = false;
    int groupMembersInRange = 1; // includes the original killer

    // set the flag based on whether or not the killer is a rebel / imperial
    if (killerCreature->isRebel()) {
      rebelInKillingGroup |= true;
    }
    else if (killerCreature->isImperial()) {
      imperialInKillingGroup |= true;
    }

    // only award gcw xp to group members within 76m
    const float gcwXpRange = 76.0f;

    // if the killer is in a group we need to determine how many group members are in range
    // as that will determine the amount of gcw xp awarded.  We also need to track if there
    // are any rebels / imperials in the group.
    if (killerCreature->isGrouped())
    {
      ManagedReference<GroupObject*> group = killerCreature->getGroup();
      int groupSize = group->getGroupSize();

      // iterate over the group members
      for (int i = 0; i < groupSize; i++) {
        ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

        // skip the killer passed in
        if (killerCreature->getObjectID() == groupMember->getObjectID())
          continue;

        // if the group member is in range...
        if (killerCreature->isInRange(groupMember, gcwXpRange)) {
          CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

          if (groupMemberCreature == NULL)
            continue;

          // increment group counter
          groupMembersInRange++;

          // flag based on faction
          if (groupMemberCreature->isRebel()) {
            rebelInKillingGroup |= true;
          }
          else if (groupMemberCreature->isImperial()) {
            imperialInKillingGroup |= true;
          }
        }
      }
    }

    // For now, the killing group needs to contain an opposing faction

    const bool bValidGcwKill = ((destructedObject->isImperial() && rebelInKillingGroup) ||
      (destructedObject->isRebel() && imperialInKillingGroup));

    if (!bValidGcwKill)
    {
      return;
    }

    const int32 gcwEventID = trackGcwKillEvent(destructedObject);

    // determine the amount of gcw xp to award..
    const int gcwXpToAward = gcwXpGain / groupMembersInRange;
    const String fightClubMessage = "Fight Clubbing is not tolerated within the Galactic Civil War.  You have been awarded no GCW experience.";

    // if the dead player is imperial, and there where rebels involved in the kill...
    if (destructedObject->isImperial() && rebelInKillingGroup) {

      // Handle the killer themselves
        {
          const bool isOpposingFaction = killerCreature->isRebel();
          if (isOpposingFaction)
          {
            const bool isFightClubbing = killerGhost->getAccountID() == killedGhost->getAccountID();
            const int actualXpAward = isFightClubbing ? 0 : gcwXpToAward;

            // award xp to killer
            killerGhost->addExperience("gcw_skill_xp", actualXpAward);
            trackGcwXpChange(gcwEventID, killerCreature, actualXpAward);

            if (isFightClubbing)
            {
              killerCreature->sendSystemMessage(fightClubMessage);
            }
          }

          trackGcwKillParticipant(gcwEventID, killerCreature);
        }

      // iterate group and award xp to any valid group members
      if (killerCreature->isGrouped())
      {
        ManagedReference<GroupObject*> group = killerCreature->getGroup();
        int groupSize = group->getGroupSize();

        // iterate over the group members
        for (int i = 0; i < groupSize; i++) {
          ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

          // skip the killer passed in
          if (killerCreature->getObjectID() == groupMember->getObjectID())
            continue;

          // if the group member is in range...
          if (killerCreature->isInRange(groupMember, gcwXpRange)) {
            CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

            if (groupMemberCreature == NULL)
              continue;

            if (!groupMemberCreature->isPlayerCreature())
              continue;

            trackGcwKillParticipant(gcwEventID, groupMemberCreature);

            ManagedReference<PlayerObject*> groupMemberGhost = groupMemberCreature->getPlayerObject();
            const bool isOpposingFaction = groupMemberCreature->isRebel();

            // only rebels are valid for an imperial kill
            if (isOpposingFaction)
            {
              const bool isFightClubbing = groupMemberGhost->getAccountID() == killedGhost->getAccountID();
              const int actualXpAward = isFightClubbing ? 0 : gcwXpToAward;

              groupMemberGhost->addExperience("gcw_skill_xp", actualXpAward);
              trackGcwXpChange(gcwEventID, groupMemberCreature, actualXpAward);

              if (isFightClubbing)
              {
                groupMemberCreature->sendSystemMessage(fightClubMessage);
              }
            }
          }
        }
      }

      // remove xp from the dead imperial
      killedGhost->addExperience("gcw_skill_xp", gcwXpLoss, true);
      trackGcwXpChange(gcwEventID, destructedObject, gcwXpLoss);
    }
    else if (destructedObject->isRebel() && imperialInKillingGroup) {

      // Handle the killer themselves
        {
          const bool isOpposingFaction = killerCreature->isImperial();
          if (isOpposingFaction)
          {
            const bool isFightClubbing = killerGhost->getAccountID() == killedGhost->getAccountID();
            const int actualXpAward = isFightClubbing ? 0 : gcwXpToAward;

            // award xp to killer
            killerGhost->addExperience("gcw_skill_xp", actualXpAward);
            trackGcwXpChange(gcwEventID, killerCreature, actualXpAward);

            if (isFightClubbing)
            {
              killerCreature->sendSystemMessage(fightClubMessage);
            }
          }

          trackGcwKillParticipant(gcwEventID, killerCreature);
        }

      // iterate group and award xp to any valid group members
      if (killerCreature->isGrouped())
      {
        ManagedReference<GroupObject*> group = killerCreature->getGroup();
        int groupSize = group->getGroupSize();

        // iterate over the group members
        for (int i = 0; i < groupSize; i++) {
          ManagedReference<SceneObject*> groupMember = group->getGroupMember(i);

          // skip the killer passed in
          if (killerCreature->getObjectID() == groupMember->getObjectID())
            continue;

          // if the group member is in range...
          if (killerCreature->isInRange(groupMember, gcwXpRange)) {
            CreatureObject* groupMemberCreature = dynamic_cast<CreatureObject*>(groupMember.get());

            if (groupMemberCreature == NULL)
              continue;

            if (!groupMemberCreature->isPlayerCreature())
              continue;

            trackGcwKillParticipant(gcwEventID, groupMemberCreature);

            ManagedReference<PlayerObject*> groupMemberGhost = groupMemberCreature->getPlayerObject();
            const bool isOpposingFaction = groupMemberCreature->isImperial();

            // only imperials are valid for a rebel kill
            if (isOpposingFaction)
            {
              const bool isFightClubbing = groupMemberGhost->getAccountID() == killedGhost->getAccountID();
              const int actualXpAward = isFightClubbing ? 0 : gcwXpToAward;

              groupMemberGhost->addExperience("gcw_skill_xp", actualXpAward);
              trackGcwXpChange(gcwEventID, groupMemberCreature, actualXpAward);

              if (isFightClubbing)
              {
                groupMemberCreature->sendSystemMessage(fightClubMessage);
              }
            }
          }
        }
      }

      // remove xp from the dead rebel
      killedGhost->addExperience("gcw_skill_xp", gcwXpLoss, true);
      trackGcwXpChange(gcwEventID, destructedObject, gcwXpLoss);
    }
  }
}


void FactionManager::awardPvpFactionPoints(TangibleObject* killer, CreatureObject* destructedObject) {
	if (killer->isPlayerCreature() && destructedObject->isPlayerCreature()) {
		CreatureObject* killerCreature = cast<CreatureObject*>(killer);
		ManagedReference<PlayerObject*> ghost = killerCreature->getPlayerObject();

		ManagedReference<PlayerObject*> killedGhost = destructedObject->getPlayerObject();

		if (killer->isRebel() && destructedObject->isImperial()) {
			ghost->increaseFactionStanding("rebel", 275);
			ghost->decreaseFactionStanding("imperial", 75);

			killedGhost->decreaseFactionStanding("imperial", 75); //TODO: find formulas
		} else if (killer->isImperial() && destructedObject->isRebel()) {
			ghost->increaseFactionStanding("imperial", 275);
			ghost->decreaseFactionStanding("rebel", 75);

			killedGhost->decreaseFactionStanding("rebel", 75);
		}
	}
}

String FactionManager::getRankName(int idx) {
	if (idx >= factionRanks.getCount())
		return "";

	return factionRanks.getRank(idx).getName();
}

int FactionManager::getRankCost(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getCost();
}

int FactionManager::getRankDelegateRatioFrom(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getDelegateRatioFrom();
}

int FactionManager::getRankDelegateRatioTo(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getDelegateRatioTo();
}

int FactionManager::getFactionPointsCap(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return MAX(1000, getRankCost(rank) * 20);
}

bool FactionManager::isFaction(const String& faction) {
	if (factionMap.contains(faction))
		return true;

	return false;
}

bool FactionManager::isEnemy(const String& faction1, const String& faction2) {

	if (!factionMap.contains(faction1) || !factionMap.contains(faction2))
		return false;

	Faction* faction = factionMap.getFaction(faction1);

	return faction->getEnemies()->contains(faction2);
}

bool FactionManager::isAlly(const String& faction1, const String& faction2) {

	if (!factionMap.contains(faction1) || !factionMap.contains(faction2))
		return false;

	Faction* faction = factionMap.getFaction(faction1);

	return faction->getAllies()->contains(faction2);
}

int32 FactionManager::trackFrsKillEvent(CreatureObject* killedCreature) const
{
  if (!killedCreature)
  {
    return -1;
  }

  if (!killedCreature->isPlayerCreature())
  {
    return -1;
  }

  ManagedReference<PlayerObject*> ghost = killedCreature->getPlayerObject();
  const bool bIsLightJedi = killedCreature->hasSkill("force_rank_light_novice");

  StringBuffer query;
  query << "INSERT INTO bloodfin_frs_kill_events (account_id, character_oid, timestamp, light_jedi) VALUES (";
  query << ghost->getAccountID() << ",";
  query << killedCreature->getObjectID() << ",";
  query << "NOW(),";
  query << bIsLightJedi << ");";

  ResultSet* result = ServerDatabase::instance()->executeQuery(query.toString());
  return result->getLastAffectedRow();
}

void FactionManager::trackFrsKillParticipant(int32 frsKillEventID, CreatureObject* participantCreature) const
{
  if (frsKillEventID < 0 || !participantCreature)
  {
    return;
  }

  if (!participantCreature->isPlayerCreature())
  {
    return;
  }

  const bool isLightJedi = participantCreature->hasSkill("force_rank_light_novice");
  const bool isDarkJedi = participantCreature->hasSkill("force_rank_dark_novice");

  // TODO: Do we only want to track bounty hunters who have a mission on the killed jedi?
  const bool isBh = participantCreature->hasSkill("combat_bountyhunter_novice");

  ManagedReference<PlayerObject*> ghost = participantCreature->getPlayerObject();

  StringBuffer query;
  query << "INSERT INTO bloodfin_frs_kill_participants (event_id, account_id, character_oid, is_light_jedi, is_dark_jedi, is_bh) VALUES (";
  query << frsKillEventID << ", ";
  query << ghost->getAccountID() << ", ";
  query << participantCreature->getObjectID() << ",";
  query << isLightJedi << ",";
  query << isDarkJedi << ",";
  query << isBh << ");";

  ServerDatabase::instance()->executeQuery(query.toString());
}

void FactionManager::trackFrsXpChange(int32 frsKillEventID, CreatureObject* jediCreature, int32 frsXpChange) const
{
  if (frsKillEventID < 0 || !jediCreature)
  {
    return;
  }

  if (!jediCreature->isPlayerCreature())
  {
    return;
  }

  ManagedReference<PlayerObject*> ghost = jediCreature->getPlayerObject();

  StringBuffer query;
  query << "INSERT INTO bloodfin_frs_xpchange (event_id, account_id, character_oid, xp_change) VALUES (";
  query << frsKillEventID << ",";
  query << ghost->getAccountID() << ",";
  query << jediCreature->getObjectID() << ",";
  query << frsXpChange << ");";

  ServerDatabase::instance()->executeQuery(query.toString());

  // notify player
  String notifyMessage = "You have ";
  notifyMessage += (frsXpChange < 0 ? "lost " : "been awarded ");
  notifyMessage += String::valueOf(abs(frsXpChange)) + " points of Force Rank Experience.";
  jediCreature->sendSystemMessage(notifyMessage);
}

int32 FactionManager::trackGcwKillEvent(CreatureObject* killedCreature) const
{
  if (!killedCreature)
  {
    return -1;
  }

  if (!killedCreature->isPlayerCreature())
  {
    return -1;
  }

  ManagedReference<PlayerObject*> ghost = killedCreature->getPlayerObject();
  const bool bIsRebel = killedCreature->isRebel();

  StringBuffer query;
  query << "INSERT INTO bloodfin_gcw_kill_events (account_id, character_oid, timestamp, rebel) VALUES (";
  query << ghost->getAccountID() << ",";
  query << killedCreature->getObjectID() << ",";
  query << "NOW(),";
  query << bIsRebel << ");";

  ResultSet* result = ServerDatabase::instance()->executeQuery(query.toString());
  return result->getLastAffectedRow();
}

void FactionManager::trackGcwKillParticipant(int32 gcwKillEventID, CreatureObject* participantCreature) const
{
  if (gcwKillEventID < 0 || !participantCreature)
  {
    return;
  }

  if (!participantCreature->isPlayerCreature())
  {
    return;
  }

  const bool isRebel = participantCreature->isRebel();
  const bool isImperial = participantCreature->isImperial();

  ManagedReference<PlayerObject*> ghost = participantCreature->getPlayerObject();

  StringBuffer query;
  query << "INSERT INTO bloodfin_gcw_kill_participants (event_id, account_id, character_oid, is_rebel, is_imperial) VALUES (";
  query << gcwKillEventID << ", ";
  query << ghost->getAccountID() << ", ";
  query << participantCreature->getObjectID() << ",";
  query << isRebel << ",";
  query << isImperial << ");";

  ServerDatabase::instance()->executeQuery(query.toString());
}

void FactionManager::trackGcwXpChange(int32 gcwKillEventID, CreatureObject* playerCreature, int32 gcwXpChange) const
{
  if (gcwKillEventID < 0 || !playerCreature)
  {
    return;
  }

  if (!playerCreature->isPlayerCreature())
  {
    return;
  }

  ManagedReference<PlayerObject*> ghost = playerCreature->getPlayerObject();

  StringBuffer query;
  query << "INSERT INTO bloodfin_gcw_xpchange (event_id, account_id, character_oid, xp_change) VALUES (";
  query << gcwKillEventID << ",";
  query << ghost->getAccountID() << ",";
  query << playerCreature->getObjectID() << ",";
  query << gcwXpChange << ");";

  ServerDatabase::instance()->executeQuery(query.toString());

  String notifyMessage = "You have ";
  notifyMessage += (gcwXpChange < 0 ? "lost " : "been awarded ");
  notifyMessage += String::valueOf(abs(gcwXpChange)) + " points of GCW Experience.";
  playerCreature->sendSystemMessage(notifyMessage);
}