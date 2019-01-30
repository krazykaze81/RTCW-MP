/*
===========================================================================

Return to Castle Wolfenstein multiplayer GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein multiplayer GPL Source Code (RTCW MP Source Code).

RTCW MP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW MP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW MP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW MP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW MP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
OSPx - g_players.c

Various player's commands

Created: 5 May/14
===========================================================================
*/
#include "g_local.h"



//
// Update info:
//	1. Add line to aCommandInfo w/appropriate info
//	2. Add implementation for specific command (see an existing command for an example)
//

/* Code from ET
typedef struct {
	char *pszCommandName;
	qboolean fAnytime;
	qboolean fValue;
	void(*pCommand)(gentity_t *ent, unsigned int dwCommand, qboolean fValue);
	const char *pszHelpInfo;
} cmd_reference_t;

// VC optimizes for dup strings :)
static const cmd_reference_t aCommandInfo[] = {
	{ "+stats",          qtrue,  qtrue,  NULL, ":^7 HUD overlay showing current weapon stats info" },
	{ "+topshots",       qtrue,  qtrue,  NULL, ":^7 HUD overlay showing current top accuracies of all players" },
	{ "?",       qtrue,  qtrue,  G_commands_cmd, ":^7 Gives a list of OSP-specific commands" },
	{ "autorecord",      qtrue,  qtrue,  NULL, ":^7 Creates a demo with a consistent naming scheme" },
	{ "autoscreenshot",  qtrue,  qtrue,  NULL, ":^7 Creates a screenshot with a consistent naming scheme" },
	{ "bottomshots", qtrue,  qfalse, G_weaponRankings_cmd, ":^7 Shows WORST player for each weapon. Add ^3<weapon_ID>^7 to show all stats for a weapon" },
	//	{ "callvote",		qtrue,	qfalse,	Cmd_CallVote_f, " <params>:^7 Calls a vote" },
		{ "callvote",        qtrue,  qfalse, (void(*) (gentity_t *, unsigned int, qboolean))Cmd_CallVote_f, " <params>:^7 Calls a vote" },
		//	{ "captains",		qtrue,	qtrue,	NULL, ":^7 Shows team captains" },
		//	{ "coach",			qtrue,	qtrue,	NULL, ":^7 Accepts coach invitation/restarts coach view" },
		//	{ "coachdecline",	qtrue,	qtrue,	NULL, ":^7 Declines coach invitation or resigns coach status" },
		//	{ "coachinvite",	qtrue,	qtrue,	NULL, " <player_ID>:^7 Invites a player to coach team" },
		//	{ "coachkick",		qtrue,	qtrue,	NULL, " <player_ID>:^7 Kicks active coach from team" },
			{ "commands",        qtrue,  qtrue,  G_commands_cmd, ":^7 Gives a list of OSP-specific commands" },
			{ "currenttime", qtrue,  qtrue,  NULL, ":^7 Displays current local time" },
			{ "follow",          qfalse, qtrue,  Cmd_Follow_f, " <player_ID|allies|axis>:^7 Spectates a particular player or team" },
			//	{ "invite",			qtrue,	qtrue,	NULL, " <player_ID>:^7 Invites a player to join a team" },
				{ "lock",            qtrue,  qtrue,  G_lock_cmd, ":^7 Locks a player's team to prevent others from joining" },
				{ "notready",        qtrue,  qfalse, G_ready_cmd, ":^7 Sets your status to ^5not ready^7 to start a match" },
				{ "pause",           qfalse, qtrue,  G_pause_cmd, ":^7 Allows a team to pause a match" },
				{ "players",     qtrue,  qtrue,  G_players_cmd, ":^7 Lists all active players and their IDs/information" },
				{ "ready",           qtrue,  qtrue,  G_ready_cmd, ":^7 Sets your status to ^5ready^7 to start a match" },
				{ "readyteam",       qfalse, qtrue,  G_teamready_cmd, ":^7 Sets an entire team's status to ^5ready^7 to start a match" },
				{ "ref",         qtrue,  qtrue,  G_ref_cmd, " <password>:^7 Become a referee (admin access)" },
				//	{ "remove",			qtrue,	qtrue,	NULL, " <player_ID>:^7 Removes a player from the team" },
				//	{ "resign",			qtrue,	qtrue,	NULL, " [player_ID]:^7 Resigns captainship.  Can optionally be given to another teammate" },
					{ "say_teamnl",      qtrue,  qtrue,  G_say_teamnl_cmd, "<msg>:^7 Sends a team chat without location info" },
					{ "scores",          qtrue,  qtrue,  G_scores_cmd, ":^7 Displays current match stat info" },
					{ "specinvite",      qtrue,  qtrue,  G_specinvite_cmd, ":^7 Invites a player to spectate a speclock'ed team" },
					{ "speclock",        qtrue,  qtrue,  G_speclock_cmd, ":^7 Locks a player's team from spectators" },
					//	{ "speconly",		qtrue,	qtrue,	NULL, ":^7 Toggles option to stay as a spectator in 1v1" },
						{ "specunlock",      qtrue,  qfalse, G_speclock_cmd, ":^7 Unlocks a player's team from spectators" },
						{ "statsall",        qtrue,  qfalse, G_statsall_cmd, ":^7 Shows weapon accuracy stats for all players" },
						{ "statsdump",       qtrue,  qtrue,  NULL, ":^7 Shows player stats + match info saved locally to a file" },
						{ "team",            qtrue,  qtrue,  Cmd_Team_f, " <b|r|s|none>:^7 Joins a team (b = allies, r = axis, s = spectator)" },
						//	{ "setclass",		qtrue,	qtrue,	Cmd_SetClass_f, " <classnum>:^7 Selects a class" },
						//	{ "setweapons",		qtrue,	qtrue,	Cmd_SetWeapons_f, " <weapon|weapon2>:^7 Selects your weapon loadout" },
							{ "timein",          qfalse, qfalse, G_pause_cmd, ":^7 Unpauses a match (if initiated by the issuing team)" },
							{ "timeout",     qfalse, qtrue,  G_pause_cmd, ":^7 Allows a team to pause a match" },
							{ "topshots",        qtrue,  qtrue,  G_weaponRankings_cmd, ":^7 Shows BEST player for each weapon. Add ^3<weapon_ID>^7 to show all stats for a weapon" },
							{ "unlock",          qtrue,  qfalse, G_lock_cmd, ":^7 Unlocks a player's team, allowing others to join" },
							{ "unpause",     qfalse, qfalse, G_pause_cmd, ":^7 Unpauses a match (if initiated by the issuing team)" },
							{ "unready",     qtrue,  qfalse, G_ready_cmd, ":^7 Sets your status to ^5not ready^7 to start a match" },
							{ "weaponstats", qtrue,  qfalse, G_weaponStats_cmd, " [player_ID]:^7 Shows weapon accuracy stats for a player" },
							//	{ "viewcam",		qfalse,	qtrue,	NULL, ":^7 Switches to cinematic camera mode" },
							//	{ "vc_follow",		qfalse,	qtrue,	NULL, " [player_ID]:^7 Puts viewcam in follow mode.  Can optionally to follow a specific player" },
							//	{ "vc_free",		qfalse,	qtrue,	NULL, ":^7 Toggle viewcam between manual/automatic change" },
							//	{ "vc_view",		qfalse,	qtrue,	NULL, ":^7 Toggle ViewCam between static/dynamic views" },
							//	{ "viewadd",		qfalse,	qtrue,	NULL, " <player_ID>:^7 Adds a player to multi-screen view" },
							//	{ "viewall",		qfalse,	qtrue,	NULL, ":^7 Adds all active players to a multi-screen view" },
							//	{ "viewallies",		qfalse,	qtrue,	NULL, ": ^7 Views entire allies/axis team" },
							//	{ "viewaxis",		qfalse,	qtrue,	NULL, ": ^7 Views entire allies/axis team" },
							//	{ "viewcyclenext",	qfalse,	qtrue,	NULL, ":^7 Cycles through players in current view" },
							//	{ "viewfollow",		qfalse,	qtrue,	NULL, ":^7 Follows current highlighted view" },
							//	{ "viewnext",		qfalse,	qtrue,	NULL, ":^7 Moves through active screen in a multi-screen display" },
							//	{ "viewnone",		qfalse,	qtrue,	NULL, ":^7 Disables multiview mode and goes back to spectator mode" },
							//	{ "viewremove",		qfalse,	qtrue,	NULL, " [player_ID]:^7 Removes current selected or specific player from multi-screen view" },
								{ 0,                qfalse, qtrue,  NULL, 0 }
};
*/

// ************** PLAYERS
//
// Show client info
void pCmd_players(gentity_t *ent, qboolean fParam) {
	int i, idnum, max_rate, cnt = 0, tteam;
	int user_rate, user_snaps;
	gclient_t *cl;
	gentity_t *cl_ent;
	char n1[MAX_NETNAME], ready[16], ref[16], rate[256];
	char *s, *tc, *coach, userinfo[MAX_INFO_STRING];


	if (g_gamestate.integer == GS_PLAYING) {
		if (ent) {
			CP("print \"\n^3 ID^1 : ^3Player                    Nudge  Rate  MaxPkts  Snaps\n\"");
			CP("print \"^1-----------------------------------------------------------^7\n\"");
		}
		else {
			G_Printf(" ID : Player                    Nudge  Rate  MaxPkts  Snaps\n");
			G_Printf("-----------------------------------------------------------\n");
		}
	}
	else {
		if (ent) {
			CP("print \"\n^3Status^1   : ^3ID^1 : ^3Player                    Nudge  Rate  MaxPkts  Snaps\n\"");
			CP("print \"^1---------------------------------------------------------------------^7\n\"");
		}
		else {
			G_Printf("Status   : ID : Player                    Nudge  Rate  MaxPkts  Snaps\n");
			G_Printf("---------------------------------------------------------------------\n");
		}
	}

	max_rate = trap_Cvar_VariableIntegerValue("sv_maxrate");

	for (i = 0; i < level.numConnectedClients; i++) {
		idnum = level.sortedClients[i];
		cl = &level.clients[idnum];
		cl_ent = g_entities + idnum;

		SanitizeString(cl->pers.netname, n1, qtrue);
		Q_CleanStr(n1);
		n1[26] = 0;
		ref[0] = 0;
		ready[0] = 0;

		// Rate info
		if (cl_ent->r.svFlags & SVF_BOT) {
			strcpy(rate, va("%s%s%s%s", "[BOT]", " -----", "       --", "     --"));
		}
		else if (cl->pers.connected == CON_CONNECTING) {
			strcpy(rate, va("%s", "^3>>> CONNECTING <<<"));
		}
		else {
			trap_GetUserinfo(idnum, userinfo, sizeof(userinfo));
			s = Info_ValueForKey(userinfo, "rate");
			user_rate = (max_rate > 0 && atoi(s) > max_rate) ? max_rate : atoi(s);
			s = Info_ValueForKey(userinfo, "snaps");
			user_snaps = atoi(s);

			strcpy(rate, va("%5d%6d%9d%7d", cl->pers.clientTimeNudge, user_rate, cl->pers.clientMaxPackets, user_snaps));
		}

		if (g_gamestate.integer != GS_PLAYING) {
			if (cl->sess.sessionTeam == TEAM_SPECTATOR || cl->pers.connected == CON_CONNECTING) {
				strcpy(ready, ((ent) ? "^5--------^1 :" : "-------- :"));
			}
			else if (cl->pers.ready || (g_entities[idnum].r.svFlags & SVF_BOT)) {
				strcpy(ready, ((ent) ? "^3(READY)^1  :" : "(READY)  :"));
			}
			else {
				strcpy(ready, ((ent) ? "NOTREADY^1 :" : "NOTREADY :"));
			}
		}
		
		if (cl->sess.admin && !cl->sess.incognito) {			
			strcpy(ref, sortTag(ent));
		}
		/*
		if (cl->sess.coach_team) {
			tteam = cl->sess.coach_team;
			coach = (ent) ? "^3C" : "C";
		}
		else {*/
			tteam = cl->sess.sessionTeam;
			coach = " ";
		//}

		tc = (ent) ? "^7 " : " ";
		if (g_gametype.integer >= GT_WOLF) {
			if (tteam == TEAM_RED) {
				tc = (ent) ? "^1X^7" : "X";
			}
			if (tteam == TEAM_BLUE) {
				tc = (ent) ? "^4L^7" : "L";
			}
		}

		if (ent) {
			CP(va("print \"%s%s%2d%s^1:%s %-26s^7%s  ^3%s\n\"", ready, tc, idnum, coach, ((ref[0]) ? "^3" : "^7"), n1, rate, ref));
		}
		else { G_Printf("%s%s%2d%s: %-26s%s  %s\n", ready, tc, idnum, coach, n1, rate, ref); }

		cnt++;
	}

	if (ent) {
		CP(va("print \"\n^3%2d^7 total players\n\n\"", cnt));
	}
	else { G_Printf("\n%2d total players\n\n", cnt); }

	// Team speclock info
	if (g_gametype.integer >= GT_WOLF) {
		for (i = TEAM_RED; i <= TEAM_BLUE; i++) {
			if (teamInfo[i].spec_lock) {
				if (ent) {
					CP(va("print \"** %s team is speclocked.\n\"", aTeams[i]));
				}
				else { G_Printf("** %s team is speclocked.\n", aTeams[i]); }
			}
		}
	}
}

/*
===========
Getstatus
===========
*/
void pCmd_getstatus(gentity_t *ent, qboolean fParam) {
	int	j;
	// uptime
	int secs = level.time / 1000;
	int mins = (secs / 60) % 60;
	int hours = (secs / 3600) % 24;
	int days = (secs / (3600 * 24));
	qboolean teamSpecLocked = qfalse;	

	if (teamInfo[TEAM_RED].spec_lock || teamInfo[TEAM_BLUE].spec_lock)
		teamSpecLocked = qtrue;

	CP(va("print \"\n^7Server: %s    %s\n\"", sv_hostname.string, getTime()));
	// N/c..
	if (teamInfo[TEAM_BLUE].spec_lock || teamInfo[TEAM_RED].spec_lock)
		CP(va("print \"Speclocked: %s^7\n\"",
		((teamInfo[TEAM_BLUE].spec_lock && teamInfo[TEAM_RED].spec_lock) ? "^3Both" :
				((teamInfo[TEAM_BLUE].spec_lock ? "^3Allied" : "^1Axis"))
			)));
	if (teamInfo[TEAM_BLUE].team_lock || teamInfo[TEAM_RED].team_lock)
		CP(va("print \"Teamlocked: %s^7\n\"",
				( (teamInfo[TEAM_BLUE].team_lock && teamInfo[TEAM_RED].team_lock) ? "^3Both" :
					( (teamInfo[TEAM_BLUE].team_lock ? "^3Allied" : "^1Axis") )
				)));
	CP("print \"^3-----------------------------------------------------------------------------\n\"");
	CP("print \"^7Slot : Team : Name       : IP              ^7: ^3Nudge  MaxPkts ^7: Status \n\"");
	CP("print \"^3-----------------------------------------------------------------------------\n\"");

	for (j = 0; j <= (MAX_CLIENTS - 1); j++) {

		if (g_entities[j].client) {
			char *team, *slot, *ip, *tag = {""};
			char n1[MAX_NETNAME];

			SanitizeString(ent->client->pers.netname, n1, qtrue);
			Q_CleanStr(n1);
			n1[26] = 0;

			// player is connecting
			if (g_entities[j].client->pers.connected == CON_CONNECTING) {
				CP(va("print \"%3d  : >><< : %-10s : ^d>>Connecting<<  ^7:\n\"", j, n1));
			}

			// player is connected
			if (g_entities[j].client->pers.connected == CON_CONNECTED) {

				// Sort it :C				
				slot = va("%3d", j);
				team = (g_entities[j].client->sess.sessionTeam == TEAM_SPECTATOR) ? "^3SPEC^7" :
					(g_entities[j].client->sess.sessionTeam == TEAM_RED ? "^1Axis^7" : "^4Alld^7");
								
				ip = (ent->client->sess.admin == USER_REGULAR) ?
					va("%s", clientIP(&g_entities[j], qfalse)) :
					va("%s", clientIP(&g_entities[j], qtrue));

				if (g_entities[j].client->sess.admin && !g_entities[j].client->sess.incognito)
					tag = sortTag(&g_entities[j]);
				else if (g_entities[j].client->sess.admin && !g_entities[j].client->sess.incognito && ent->client->sess.admin)
					tag = va("%s^3*", sortTag(&g_entities[j]));
				else if (!g_entities[j].client->sess.admin && g_entities[j].client->sess.ignored)
					tag = "^1Ignored";				
								
				// Specing speclocked team (This will override ignored tag but so be it..).
				if (g_entities[j].client->sess.sessionTeam == TEAM_SPECTATOR &&
					!g_entities[j].client->sess.admin &&
					teamSpecLocked) {
					if (g_entities[j].client->sess.specInvited == 1)
						tag = "Spec. Axis";
					else if (g_entities[j].client->sess.specInvited == 2)
						tag = "Spec. Allies";
					else if (g_entities[j].client->sess.specInvited == 3)
						tag = "Spec. Both";
					else if (teamSpecLocked > 0 && !g_entities[j].client->sess.specInvited)
						tag = "^3Speclocked";
					// Hidden admins
				}
				else if (g_entities[j].client->sess.sessionTeam == TEAM_SPECTATOR &&
					g_entities[j].client->sess.admin &&
					g_entities[j].client->sess.incognito) {
					tag = "^3Spec. Both";
				}

				// Print it now
				CP(va("print \"%-4s : %s : %-10s : %-15s ^7: ^3%5d  %7d ^7%-12s \n\"",
					slot,
					team,
					n1,
					ip,	
					g_entities[j].client->pers.clientTimeNudge, 
					g_entities[j].client->pers.clientMaxPackets,
					(tag ? va(": %s", tag) : "")
					));
			}
		}
	}
	CP("print \"^3-----------------------------------------------------------------------------\n\"");
	CP(va("print \"^7Uptime: ^3%d ^7day%s ^3%d ^7hours ^3%d ^7minutes\n\"", days, (days != 1 ? "s" : ""), hours, mins));
	CP("print \"\n\"");
	return;
}

/*
===================
READY / NOTREADY

Tardo - rewrote this because the parameter handling to the function is different in rtcw.
===================
*/
void pCmd_ready(gentity_t *ent, qboolean state) {
	char *status[2] = { "NOT READY", "READY" };

	if (!g_doWarmup.integer) {
		return;
	}
	if (!state && g_gamestate.integer == GS_WARMUP_COUNTDOWN) {
		CP("print \"Countdown started, ^3notready^7 ignored.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
		CP(va("print \"Specs cannot use ^3%s ^7command.\n\"", status[state]));
		return;
	}
	if (level.readyTeam[ent->client->sess.sessionTeam] == qtrue && !state) { // Doesn't cope with unreadyteam but it's out anyway..
		CP(va("print \"%s ^7ignored. Your team has issued ^3TEAM READY ^7command..\n\"", status[state]));
		return;
	}

	// Move them to correct ready state
	if (ent->client->pers.ready == state) {
		CP(va("print \"You are already ^3%s^7!\n\"", status[state]));
	}
	else {
		ent->client->pers.ready = state;
		if (!level.intermissiontime) {
			if (state) {
				ent->client->pers.ready = qtrue;
				ent->client->ps.powerups[PW_READY] = INT_MAX;
			}
			else {
				ent->client->pers.ready = qfalse;
				ent->client->ps.powerups[PW_READY] = 0;
			}

			// Doesn't rly matter..score tab will show slow ones..
			AP(va("popin \"%s ^7is %s%s!\n\"", ent->client->pers.netname, (state ? "^n" : "^z"), status[state]));
		}
	}
}

/*
===================
TEAM-READY / NOTREADY
===================
*/
void pCmd_teamReady(gentity_t *ent, qboolean ready) {
	char *status[2] = { "NOT READY", "READY" };	
	int i, p = { 0 };
	int team = ent->client->sess.sessionTeam;
	gentity_t *cl;

	if (!g_doWarmup.integer) {
		return;
	}
	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}
	if (ent->client->sess.sessionTeam == TEAM_SPECTATOR) {
		CP("print \"Specs cannot use ^3TEAM ^7commands.\n\"");
		return;
	}
	if (!ready && g_gamestate.integer == GS_WARMUP_COUNTDOWN) {
		CP("print \"Countdown started, ^3notready^7 ignored.\n\"");
		return;
	}

	for (i = 0; i < level.numConnectedClients; i++) {
		cl = g_entities + level.sortedClients[i];

		if (!cl->inuse) {
			continue;
		}

		if (cl->client->sess.sessionTeam != team) {
			continue;
		}

		if ((cl->client->pers.ready != ready) && !level.intermissiontime) {
			cl->client->pers.ready = ready;
			cl->client->ps.powerups[PW_READY] = (ready ? INT_MAX : 0);
			++p;
		}
	}

	if (!p) {
		CP(va("print \"Your team is already ^3%s^7!\n\"", status[ready]));
	}
	else {	
		AP(va("popin \"%s ^7team is %s%s!\n\"", aTeams[team], (ready ? "^n" : "^z"), status[ready]));
		level.readyTeam[team] = ready;
	}
}

/*
===================
Invite player to spectate

NOTE: Admin can still be invited..so in case logout occurs..
===================
*/
void pCmd_specInvite(gentity_t *ent, qboolean fParam) {
	int	target;
	gentity_t	*player;
	char arg[MAX_TOKEN_CHARS];
	int team = ent->client->sess.sessionTeam;

	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}

	if (team == TEAM_RED || team == TEAM_BLUE) {
		if (!teamInfo[team].spec_lock) {
			CP("print \"Your team isn't locked from spectators!\n\"");
			return;
		}

		trap_Argv(1, arg, sizeof(arg));
		if ((target = ClientNumberFromString(ent, arg)) == -1) {
			return;
		}

		player = g_entities + target;

		// Can't invite self
		if (player->client == ent->client) {
			CP("print \"You can't specinvite yourself!\n\"");
			return;
		}

		// Can't invite an active player.
		if (player->client->sess.sessionTeam != TEAM_SPECTATOR) {
			CP("print \"You can't specinvite a non-spectator!\n\"");
			return;
		}

		// If player it not viewing anyone, force them..
		if (!player->client->sess.specInvited &&
			!(player->client->sess.spectatorClient == SPECTATOR_FOLLOW)) {
			player->client->sess.spectatorClient = ent->client->ps.clientNum;
			player->client->sess.spectatorState = SPECTATOR_FOLLOW;
		}

		player->client->sess.specInvited |= team;

		// Notify sender/recipient
		CP(va("print \"%s^7 has been sent a spectator invitation.\n\"", player->client->pers.netname));
		CPx(player - g_entities, va("cp \"%s ^7invited you to spec the %s team.\n\"2",
			ent->client->pers.netname, aTeams[team]));
	}
	else { CP("print \"Spectators can't specinvite players!\n\""); }
}

/*
===================
unInvite player from spectating
===================
*/
void pCmd_specUnInvite(gentity_t *ent, qboolean fParam) {
	int	target;
	gentity_t	*player;
	char arg[MAX_TOKEN_CHARS];
	int team = ent->client->sess.sessionTeam;

	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}

	if (team == TEAM_RED || team == TEAM_BLUE) {
		if (!teamInfo[team].spec_lock) {
			CP("print \"Your team isn't locked from spectators!\n\"");
			return;
		}

		trap_Argv(1, arg, sizeof(arg));
		if ((target = ClientNumberFromString(ent, arg)) == -1) {
			return;
		}

		player = g_entities + target;

		// Can't uninvite self
		if (player->client == ent->client) {
			CP("print \"You can't specuninvite yourself!\n\"");
			return;
		}

		// Can't uninvite an active player.
		if (player->client->sess.sessionTeam != TEAM_SPECTATOR) {
			CP("print \"You can't specuninvite a non-spectator!\n\"");
			return;
		}

		// Can't uninvite a already speclocked player
		if (player->client->sess.specInvited < team) {
			CP(va("print \"%s ^7already can't spectate your team!\n\"", ent->client->pers.netname));
			return;
		}

		player->client->sess.specInvited &= ~team;
		G_updateSpecLock(team, qtrue);

		// Notify sender/recipient
		CP(va("print \"%s^7 can't any longer spectate your team.\n\"", player->client->pers.netname));
		CPx(player->client->ps.clientNum, va("print \"%s ^7has revoked your ability to spectate the %s team.\n\"",
			ent->client->pers.netname, aTeams[team]));

	}
	else { CP("print \"Spectators can't specuninvite players!\n\""); }
}

/*
===================
Revoke ability from all players to spectate
===================
*/
void pCmd_uninviteAll(gentity_t *ent, qboolean fParam) {
	int team = ent->client->sess.sessionTeam;

	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}

	if (team == TEAM_RED || team == TEAM_BLUE) {
		if (!teamInfo[team].spec_lock) {
			CP("print \"Your team isn't locked from spectators!\n\"");
			return;
		}

		// Remove all specs
		G_removeSpecInvite(team);

		// Notify that team only that specs lost privilage
		TP(team, va("chat \"^3TEAM NOTICE: ^7%s ^7has revoked ALL spec's invites for your team.\n\"", ent->client->pers.netname));
		// Inform specs..
		TP(TEAM_SPECTATOR, va("print \"%s ^7revoked ALL spec invites from %s team.\n\"", ent->client->pers.netname, aTeams[team]));
	}
	else { CP("print \"Spectators can't specuninviteall!\n\""); }
}

/*
===================
Spec lock/unlock team
===================
*/
void pCmd_speclock(gentity_t *ent, qboolean lock) {
	int team = ent->client->sess.sessionTeam;

	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}

	if (team == TEAM_RED || team == TEAM_BLUE) {
		if ((lock && teamInfo[team].spec_lock) || (!lock && !teamInfo[team].spec_lock)) {
			CP(va("print \"Your team is already %s spectators!\n\"",
				(!lock ? "unlocked for" : "locked from")));
			return;
		}

		G_updateSpecLock(team, lock);
		AP(va("cp \"%s is now ^3SPEC%s\"2", aTeams[team], (lock ? "LOCKED" : "UNLOCKED")));

		if (lock) {
			CP("print \"Use ^3specinvite^7 to invite people to spectate.\n\"");
		}
	}
	else { CP(va("print \"Spectators can't use ^3spec%s ^7command!\n\"", (lock ? "lock" : "unlock"))); }
}

/*
===================
Pause/Unpause
===================
*/
void pCmd_pauseHandle(gentity_t *ent, qboolean dPause) {
	int team = ent->client->sess.sessionTeam;
	char *status[2] = { "^3UN", "^3" };
	char tName[MAX_NETNAME];

	if (team_nocontrols.integer) {
		CP("print \"Team commands are not enabled on this server.\n\"");
		return;
	}

	if ((!level.alliedPlayers || !level.axisPlayers) && dPause) {		
		CP("print \"^1Error^7: Pause can only be used when both teams have players!\n\"");
		return;
	}

	if ((PAUSE_UNPAUSING >= level.match_pause && !dPause) || (PAUSE_NONE != level.match_pause && dPause)) {
		CP(va("print \"The match is already %sPAUSED^7!\n\"", status[dPause]));
		return;
	}

	Q_decolorString(aTeams[team], tName);

	// Trigger the auto-handling of pauses
	if (dPause) {
		if (!teamInfo[team].timeouts) {
			CP("print \"^3Denied^7: Your team has no more timeouts remaining!\n\"");
			return;
		}
		else {
			teamInfo[team].timeouts--;
			level.match_pause = team + 128;		
			G_spawnPrintf(DP_PAUSEINFO, level.time + 15000, NULL);
			AP(va("chat \"console: %s ^3Paused ^7the match.\n\"", tName));
			AP(va("cp \"[%s^7] %d Timeouts Remaining\n\"3", aTeams[team], teamInfo[team].timeouts));
			
		}
	}
	else if (team + 128 != level.match_pause) {
		CP("print \"^3Denied^7: Your team didn't call the timeout!\n\"");
		return;
	}
	else {
		AP(va("chat \"console: %s ^7have ^3UNPAUSED^7 the match!\n\"", tName));
		level.match_pause = PAUSE_UNPAUSING;
		G_spawnPrintf(DP_UNPAUSING, level.time + 10, NULL);
	}
}

/*------------------------------------------------------------------------------*/

/*
// OSP-specific Commands
qboolean G_commandCheck(gentity_t *ent, char *cmd, qboolean fDoAnytime) {
	unsigned int i, cCommands = sizeof(pCmd_reference_t) / sizeof(pCmd_reference_t[0]);
	const pCmd_reference_t *pCR;

	for (i = 0; i < cCommands; i++) {
		pCR = &pCmd_reference_t[i];
		if (NULL != pCR->pCommand && pCR->fAnytime == fDoAnytime && 0 == Q_stricmp(cmd, pCR->pszCommandName)) {
			if (!G_commandHelp(ent, cmd, i)) {
				pCR->pCommand(ent, i, pCR->fValue);
			}
			return(qtrue);
		}
	}

	return(G_smvCommands(ent, cmd));
}


// Prints specific command help info.
qboolean G_commandHelp(gentity_t *ent, char *pszCommand, unsigned int dwCommand) {
	char arg[MAX_TOKEN_CHARS];

	if (!ent) {
		return(qfalse);
	}
	trap_Argv(1, arg, sizeof(arg));
	if (!Q_stricmp(arg, "?")) {
		CP(va("print \"\n^3%s%s\n\n\"", pszCommand, aCommandInfo[dwCommand].pszHelpInfo));
		return(qtrue);
	}

	return(qfalse);
}

// ************** COMMANDS / ?
//
// Lists server commands.
void G_commands_cmd(gentity_t *ent, unsigned int dwCommand, qboolean fValue) {
	int i, rows, num_cmds = sizeof(aCommandInfo) / sizeof(aCommandInfo[0]) - 1;

	rows = num_cmds / HELP_COLUMNS;
	if (num_cmds % HELP_COLUMNS) {
		rows++;
	}
	if (rows < 0) {
		return;
	}

	CP("cpm \"^5\nAvailable OSP Game-Commands:\n----------------------------\n\"");
	for (i = 0; i < rows; i++) {
		if (i + rows * 3 + 1 <= num_cmds) {
			CP(va("print \"^3%-17s%-17s%-17s%-17s\n\"", aCommandInfo[i].pszCommandName,
				aCommandInfo[i + rows].pszCommandName,
				aCommandInfo[i + rows * 2].pszCommandName,
				aCommandInfo[i + rows * 3].pszCommandName));
		}
		else if (i + rows * 2 + 1 <= num_cmds) {
			CP(va("print \"^3%-17s%-17s%-17s\n\"", aCommandInfo[i].pszCommandName,
				aCommandInfo[i + rows].pszCommandName,
				aCommandInfo[i + rows * 2].pszCommandName));
		}
		else if (i + rows + 1 <= num_cmds) {
			CP(va("print \"^3%-17s%-17s\n\"", aCommandInfo[i].pszCommandName, aCommandInfo[i + rows].pszCommandName));
		}
		else {
			CP(va("print \"^3%-17s\n\"", aCommandInfo[i].pszCommandName));
		}
	}

	CP("cpm \"\nType: ^3\\command_name ?^7 for more information\n\"");
}
*/

char *lock_status[2] = { "unlock", "lock" };

// ************** LOCK / UNLOCK
//
// Locks/unlocks a player's team.
void pCmd_Lock(gentity_t *ent, unsigned int dwCommand, qboolean fLock) {
	int tteam;

	/*if (team_nocontrols.integer) {
		G_noTeamControls(ent); return;
	}
	if (!G_cmdDebounce(ent, pCmd[dwCommand].command)) {
		return;
	}*/

	tteam = G_teamID(ent);
	if (tteam == TEAM_RED || tteam == TEAM_BLUE) {
		if (teamInfo[tteam].team_lock == fLock) {
			CP(va("print \"^3Your team is already %sed!\n\"", lock_status[fLock]));
		}
		else {
			char *info = va("\"The %s team is now %sed!\n\"", aTeams[tteam], lock_status[fLock]);

			teamInfo[tteam].team_lock = fLock;
			AP(va("print %s", info));
			AP(va("cp %s", info));
		}
	}
	else { CP(va("print \"Spectators can't %s a team!\n\"", lock_status[fLock])); }
}

/*
===========
Player's structure
===========
*/
typedef struct {
	char *command;
	void(*pCommand)(gentity_t *ent, qboolean fParam);
	qboolean fParam;
	qboolean nWarmup;		// Not allowed in warmup
	qboolean jWarmup;		// Allowed only in warmup
	qboolean nIntermission;	// Not allowed during intermission..
} pCmd_reference_t;

static const pCmd_reference_t pCmd[] = {					// Properties..
	{ "players",			pCmd_players,		qfalse,		qfalse,	qfalse,	qfalse },
	{ "getstatus",			pCmd_getstatus,		qfalse,		qfalse,	qfalse,	qfalse },
	{ "ready",				pCmd_ready,			qtrue,		qfalse,	qtrue,	qtrue },
	{ "notready",			pCmd_ready,			qfalse,		qfalse,	qtrue,	qtrue },
	{ "readyteam",			pCmd_teamReady,		qtrue,		qfalse,	qtrue,	qtrue },
	//{ "unreadyteam",		pCmd_teamReady,		qfalse,		qfalse,	qtrue,	qtrue }, // It's there if needed..
	{ "speclock",			pCmd_speclock,		qtrue,		qfalse,	qfalse,	qtrue },
	{ "specunlock",			pCmd_speclock,		qfalse,		qfalse,	qfalse,	qtrue },
	{ "specinvite",			pCmd_specInvite,	qtrue,		qfalse,	qfalse,	qtrue },
	{ "specuninvite",		pCmd_specUnInvite,	qtrue,		qfalse,	qfalse,	qtrue },
	{ "specuninviteall",	pCmd_uninviteAll,	qtrue,		qfalse,	qfalse,	qtrue },
	{ "pause",				pCmd_pauseHandle,	qtrue,		qtrue,	qfalse,	qtrue },
	{ "timeout",			pCmd_pauseHandle,	qtrue,		qtrue,	qfalse, qtrue },
	{ "unpause",			pCmd_pauseHandle,	qfalse,		qtrue,	qfalse,	qtrue },
	{ "timein",				pCmd_pauseHandle,	qfalse,		qtrue,	qfalse, qtrue },
	{ "lock",				pCmd_Lock,			qtrue,		qtrue,	qfalse, qtrue },
	{ "unlock",				pCmd_Lock,			qfalse,		qtrue,	qfalse, qtrue },
	{ 0,					NULL,				qfalse,		qtrue,	qtrue,	qtrue }
};

/*
===========
Player commands..no help with this one..
===========
*/
qboolean playerCommandsExt(gentity_t *ent, char *cmd) {
	unsigned int i, \
		uCmd = ARRAY_LEN(pCmd);
	const pCmd_reference_t *uCM;
	qboolean wasUsed = qfalse;

	for (i = 0; i < uCmd; i++) {
		uCM = &pCmd[i];
		if (NULL != uCM->command && 0 == Q_stricmp(cmd, uCM->command)) {

			// Warmup check
			if (level.warmupTime && uCM->nWarmup)
				CP(va("@print \"%s command cannot be used during warmup!\n\"", uCM->command));
			// Vice versa
			else if (g_gamestate.integer != GS_WARMUP && uCM->jWarmup)
				CP(va("@print \"%s command can only be used during warmup!\n\"", uCM->command));
			// Intermission
			else if (level.intermissiontime && uCM->nIntermission)
				CP(va("print \"%s command cannot be used during intermission!\n\"", uCM->command));
			// We're fine with it...so go for it..
			else
				uCM->pCommand(ent, uCM->fParam);

			wasUsed = qtrue;
		}
	}
	return wasUsed;
}

