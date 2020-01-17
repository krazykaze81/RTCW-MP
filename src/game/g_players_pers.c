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

/*
================
Throw knives

Originally BOTW (tho took it from s4ndmod as it's faster to implement..).
================
*/
// Knife "think" function                                                                                                                                                                                                                                                                                               ////////////////////////////////////////////
void Touch_Knife( gentity_t *ent, gentity_t *other, trace_t *trace ) {
	qboolean hurt = qfalse;
	ent->active = qfalse;  

	if ( !other->client ) {
		return;
	}
	if ( other->health < 1 ) {
		return;  
	}

	if ( VectorLength( ent->s.pos.trDelta ) != 0 ) { 		
		if ( ( g_friendlyFire.integer ) || ( !OnSameTeam( other, ent->parent ) ) ) {
			int i;
			int sound;
			int damage = g_knifeDamage.integer;
			int randomize = g_knifeDamageRand.integer;

			// Randomize damage a little if enabled..
			if (randomize) {
				if (randomize > 100)
					randomize = 100;
				else if (randomize < 1)
					randomize = 1;

				damage -= rand() % randomize;
			}

			if ( damage <= 0 ) {
				damage = 1;
			}

			// pick a random sound to play
			i = rand() % 3;
			if ( i == 0 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit1.wav" );
			} else if ( i == 1 ) {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit2.wav" );
			} else {
				sound = G_SoundIndex( "/sound/weapons/knife/knife_hit3.wav" );
			}

			G_Sound( other, sound );  
			G_Damage( other, ent->parent, ent->parent, NULL, trace->endpos, damage, 0, MOD_KNIFETHROW ); 
			hurt = qtrue;
		}
	}

	if ( hurt == qfalse ) { 
		int makenoise = EV_ITEM_PICKUP;

		if ( g_throwKnives.integer > 0 ) {
			other->client->pers.throwingKnives++; 
		}		
		if ( g_throwKnives.integer != 0 ) { 
			if ( other->client->pers.throwingKnives > (g_throwKnives.integer + 5) ) {
				other->client->pers.throwingKnives = (g_throwKnives.integer + 5);
			}
		}		
		if ( ent->noise_index ) {
			makenoise = EV_ITEM_PICKUP_QUIET;
			G_AddEvent( other, EV_GENERAL_SOUND, ent->noise_index );
		}		
		if ( other->client->pers.predictItemPickup ) {
			G_AddPredictableEvent( other, makenoise, ent->s.modelindex );
		} else {
			G_AddEvent( other, makenoise, ent->s.modelindex );
		}
	}

	ent->freeAfterEvent = qtrue; 
	ent->flags |= FL_NODRAW;
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->r.contents = 0;
	ent->nextthink = 0;
	ent->think = 0;
}
// Actual command
void Cmd_ThrowKnives( gentity_t *ent ) {
	vec3_t velocity, angles, offset, org, mins, maxs;
	trace_t tr;
	gentity_t *ent2;
	gitem_t *item = BG_FindItemForWeapon( WP_KNIFE );

	if ( g_throwKnives.integer == 0 ) {
		return;
	}

	if ( level.time < ( ent->thrownKnifeTime + 800 ) ) {   
		return;
	}

	// If out or -1/unlimited
	if ( ( ent->client->pers.throwingKnives == 0 ) && 
		 ( g_throwKnives.integer != -1 ) ) {
		return;
	}
	
	AngleVectors( ent->client->ps.viewangles, velocity, NULL, NULL );
	VectorScale( velocity, 64, offset );
	offset[2] += ent->client->ps.viewheight / 2;
	VectorScale( velocity, 800, velocity );      
	velocity[2] += 50 + random() * 35;
	VectorAdd( ent->client->ps.origin, offset, org );
	VectorSet( mins, -ITEM_RADIUS, -ITEM_RADIUS, 0 );
	VectorSet( maxs, ITEM_RADIUS, ITEM_RADIUS, 2 * ITEM_RADIUS );  
	trap_Trace( &tr, ent->client->ps.origin, mins, maxs, org, ent->s.number, MASK_SOLID );
	VectorCopy( tr.endpos, org );

	G_Sound( ent, G_SoundIndex( "sound/weapons/knife/knife_slash1.wav" ) );  	
	ent2 = LaunchItem( item, org, velocity, ent->client->ps.clientNum ); 
	VectorCopy( ent->client->ps.viewangles, angles );
	angles[1] += 90;
	G_SetAngle( ent2, angles );
	ent2->touch = Touch_Knife;  
	ent2->parent = ent; 

	if ( g_throwKnives.integer > 0 ) {
		ent->client->pers.throwingKnives--; 
	}

	//only show the message if throwing knives are enabled
	if ( g_throwKnives.integer > 0 ) {		
		CP(va( "chat \"^3Knives left:^7 %d\" %i", ent->client->pers.throwingKnives, qfalse ));
	}

	ent->thrownKnifeTime = level.time;  

	// Global Stats - Count it
	ent->client->pers.stats.wShotsFired[STATS_KNIFETHROW]++;
}

/*
================
Smoke
================
*/
void Cmd_Smoke_f( gentity_t *ent )
{
	gclient_t *client = ent->client;
	char message[64] = "Smoke grenade ";

	if (client->ps.stats[STAT_PLAYER_CLASS] != PC_LT) {
		return;
	}

	if (!g_smokeGrenades.integer)	
	{
		trap_SendServerCommand(ent-g_entities, va("print \"Smoke grenades are disabled on this server.\n\""));
		return;
	}

	if ((g_smokeGrenadesLmt.integer) && (ent->thrownSmoke >= g_smokeGrenadesLmt.integer))
	{
		trap_SendServerCommand(ent-g_entities, va("cp \"You have used all the Smoke supplies^3!\n\" 1"));
		return;
	}

	client->ps.selectedSmoke = !client->ps.selectedSmoke;
	strcat(message, va("%s", (client->ps.selectedSmoke ? "^2enabled" : "^1disabled")));

	trap_SendServerCommand(ent-g_entities, va("popin \"%s^7!\n\" 1", message));
}

/*
================
Private messages
================
*/
void Cmd_Pmsg( gentity_t *ent )
{
	char cmd[MAX_TOKEN_CHARS];	
	char name[MAX_STRING_CHARS];
	char nameList[MAX_STRING_CHARS];
	char *msg;
	int matchList[MAX_CLIENTS];
	int count, i;

	if (!g_allowPMs.integer)	{	
		CP("print \"Private messages are Disabled^1!\n\"");
		return;
	}

	// L0 - Stupid way of doing things..but ok ..
	if (g_tournamentMode.integer > TOURNY_NONE &&
		ent->client->sess.sessionTeam == TEAM_SPECTATOR &&
		ent->client->sess.admin == ADM_NONE) 
	{
		CP("print \"Specs cannot send PMs in tournament mode^1!\n\"");
		return;
	}

	// L0 - Stupid way of doing things..but ok ..
	if (g_ignoreSpecs.integer &&
		ent->client->sess.sessionTeam == TEAM_SPECTATOR &&
		ent->client->sess.admin == ADM_NONE ) 
	{
		CP("print \"Specs cannot send PMs in tournament mode^1!\n\"");
		return;
	}

	if (trap_Argc() < 3) {
		trap_Argv(0, cmd, sizeof(cmd));			
		CP(va("print \"^3Usage:^7  %s <match> <message>\n\"", cmd));
	return;
	}

	if (ent->client->sess.ignored) {
		if (ent->client->sess.ignored == 1)
			CP( "cp \"You are ignored^1!\n\"2" );
		else
			CP( "print \"You are ^3permanently ^7ignored^1!\n\"" );
	return;
	}

	trap_Argv(1, name, sizeof(name));
	if (strlen(name) < 2) {		
		CP("print \"You must match at least ^32 ^7characters of the name^3!\n\"");
	return;
	}

	count = ClientNumberFromNameMatch(name, matchList);
	if (count == 0) {		
		CP("print \"No matching clients found^3!\n\"");
	return;
	}

	msg = ConcatArgs(2);	   
    if( strlen(msg) >= 700 ){
		G_LogPrintf( "NUKER(pmsg >= 700): %s IP: %i.%i.%i.%i\n", ent->client->pers.netname, ent->client->sess.ip[0], ent->client->sess.ip[1], ent->client->sess.ip[2], ent->client->sess.ip[3] );
	    trap_DropClient( ent-g_entities, "^7Player Kicked: ^3Nuking" );
	return;
    }

	Q_strncpyz ( nameList, "", sizeof( nameList ) );
	for (i = 0; i < count; i++)	{
		strcat(nameList, g_entities[matchList[i]].client->pers.netname);
		if (i != (count-1))
			strcat(nameList, "^7, ");	
			
		// Notify user in chat 
		CPx(matchList[i], va("chat \"^3Private Message from ^7%s^7!\n\"", ent->client->pers.netname));

		// Print full message in console..
		CPx(matchList[i], va("print \"^3Private Message from ^7%s^7: \n^3Message: ^7%.99s\n\"", ent->client->pers.netname, msg));		

		// Send sound to them  as well (keep an eye on this!)
		CPS( g_entities-matchList[i], "xmod/sound/client/pm.wav");
	}

	//let the sender know his message went to
	CP(va("print \"^3Message was sent to: ^7%s \n^3Message: ^7%.99s\n\"", nameList, msg));
}

/*
================
Shows time
================
*/
extern int trap_RealTime ( qtime_t * qtime );
const char *aMonths[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void Cmd_Time_f( gentity_t *ent ) {
	qtime_t ct;
	trap_RealTime(&ct);

	CP(va("chat \"%s^7 current time is: %02d:%02d:%02d ^3(^7%02d %s %d^3) \n\"", 
		ent->client->pers.netname , ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday, aMonths[ct.tm_mon], 1900+ct.tm_year ) );
	CPS(ent, "sound/multiplayer/dynamite_01.wav");
}

/*
===================
Drag players 

From BOTW/S4NDMoD 
===================
*/
void Cmd_Drag( gentity_t *ent) {
	gentity_t *target;
	vec3_t start,dir,end;
	trace_t tr;
	target = NULL;

	if (!g_dragBodies.integer)
		return;

	if (level.time < (ent->lastDragTime + 20))		
		return;

	if (ent->client->ps.stats[STAT_HEALTH] <= 0)	
		return;

	AngleVectors(ent->client->ps.viewangles, dir, NULL, NULL);

	VectorCopy(ent->s.pos.trBase, start);	
	start[2] += ent->client->ps.viewheight;
	VectorMA (start, 100, dir, end);

	trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, CONTENTS_CORPSE);

	if (tr.entityNum >= MAX_CLIENTS)
		return;

	target = &(g_entities[tr.entityNum]);

    if ((!target->inuse) || (!target->client))
	return;		

	VectorCopy(target->r.currentOrigin, start); 
	VectorCopy(ent->r.currentOrigin, end); 
	VectorSubtract(end, start, dir); 
	VectorNormalize(dir); 
	VectorScale(dir,100, target->client->ps.velocity);
	VectorCopy(dir, target->movedir); 
       
	ent->lastDragTime = level.time;		
}

/*
=================
Drop objective

Port from NQ
=================
*/
void Cmd_DropObj(gentity_t *self)
{
	gitem_t *item= NULL;

	// drop flag regardless
	if (self->client->ps.powerups[PW_REDFLAG]) {
		item = BG_FindItem("Red Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_REDFLAG] = 0;
	}
	if (self->client->ps.powerups[PW_BLUEFLAG]) {
		item = BG_FindItem("Blue Flag");
		if (!item)
			item = BG_FindItem("Objective");

		self->client->ps.powerups[PW_BLUEFLAG] = 0;
	}

	if (item) {
		vec3_t launchvel = { 0, 0, 0 };
		vec3_t forward;
		vec3_t origin;
		vec3_t angles;
		gentity_t *flag;

		VectorCopy(self->client->ps.origin, origin);
		// tjw: if the player hasn't died, then assume he's
		//      throwing objective per g_dropObj
		if(self->health > 0) {
			VectorCopy(self->client->ps.viewangles, angles);
			if(angles[PITCH] > 0)
				angles[PITCH] = 0;
			AngleVectors(angles, forward, NULL, NULL);
			VectorMA(self->client->ps.velocity,
				96, forward, launchvel);
			VectorMA(origin, 36, forward, origin);
			origin[2] += self->client->ps.viewheight;
		}

		flag = LaunchItem(item, origin, launchvel, self->s.number);

		flag->s.modelindex2 = self->s.otherEntityNum2;// JPW NERVE FIXME set player->otherentitynum2 with old modelindex2 from flag and restore here
		flag->message = self->message;	// DHM - Nerve :: also restore item name
		// Clear out player's temp copies
		self->s.otherEntityNum2 = 0;
		self->message = NULL;
		self->droppedObj = qtrue;
	} 
	else
	{
		Cmd_ThrowKnives( self );
	}
}

/*
=================
Stats command

TODO: Unlazy my self and add targeted stats (victim, killer, <client number>)
=================
*/
void Cmd_Stats_f(gentity_t *ent) {
	gclient_t *client = ent->client;
	qtime_t ct;
	int eff;
	int deaths = client->pers.stats.deaths;	
	float killRatio = client->pers.stats.kills;
	int shots = client->pers.stats.shotsFired;
	float acc = 0.0f;

	if (deaths > 0)
		killRatio = (float)client->pers.stats.kills / (float)deaths;
	
	if (shots > 0)
		acc = ((float)client->pers.stats.shotsHit / (float)shots) * 100.0f;

	eff = ( client->pers.stats.deaths + client->pers.stats.kills == 0 ) ? 0 : 100 * client->pers.stats.kills / ( client->pers.stats.deaths + client->pers.stats.kills );
	if ( eff < 0 ) {
		eff = 0;
	}	

	trap_RealTime(&ct);

	CP(va("print \"\n"
			"^7Mod: %s \n"
			"^7Server: %s \n"
			"^7Time: ^7%02d:%02d:%02d ^3(^7%02d %s %d^3) \n\n"
			"^7Stats for %s ^7this round: \n"
			"^7-------------------------------------------\n"			
			"^3Kills: ^7%d | ^3TKs: ^7%d | ^3Poisoned: ^7%d\n"			
			"^3Deaths: ^7%d | ^3Suicides: ^7%d\n"			
			"^3Headshots: ^7%d | ^3Gibs: ^7%d | ^3Revived: ^7%d\n"			
			"^3Packs Given: ^7%d ^3Health ^7| %d ^3Ammo \n"
			"^3Accuracy: ^7%2.2f (%d/%d)\n"
			"^3DmgGiv: ^7%d ^| ^3DmgRec: ^7%d | ^3dmgTeam: ^7%d \n"			
			"^3Kill Ratio: ^7%2.2f | ^3Efficency: ^7%d\n"
			"^3Peak: ^7%d ^3Life Kills ^7| %d ^3Death Spree \n"
			"^7-------------------------------------------\n"
			"\n\"",
		GAMEVERSION,
		sv_hostname.string,
		ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday, aMonths[ct.tm_mon], 1900+ct.tm_year,
		client->pers.netname,
		client->pers.stats.kills, client->pers.stats.teamKills, client->pers.stats.poison,
		deaths, client->pers.stats.suicides,
		client->pers.stats.headshots, client->pers.stats.gibs, client->pers.stats.revives,
		client->pers.stats.medGiv, client->pers.stats.ammoGiv,
		acc, client->pers.stats.shotsHit, shots,
		client->pers.stats.dmgGiv, client->pers.stats.dmgRec, client->pers.stats.dmgTeam,
		killRatio, eff,
		client->pers.stats.killPeak, client->pers.stats.deathPeak
	));
}

/*
=================
Hitsounds

Do it like in shrub just permanently 
(A hack tied to color so one doesn't need to type it all the time..)
=================
*/
void Cmd_hitsounds(gentity_t *ent) {
	char *action = (ent->client->sess.clientFlags & CFLAGS_HITSOUNDS ? "^3Disable^7" : "^3Enable^7");
	int	flag = (ent->client->sess.clientFlags & CFLAGS_HITSOUNDS ? 0 : 1);

	CP(va("print \"Bit flag to %s Hitsounds is /color %d \nType ^3/commands bitflags^7 for explanation.\n\"", action, flag));
	return;	
}

/*
=================
GiveAmmo

Borrowed this from S4NDMoD o:)
=================
*/
void Cmd_GiveAmmo(gentity_t* ent) {
	gentity_t *target;
	trace_t tr;
	vec3_t start, end, forward;
	char arg1[MAX_STRING_TOKENS];
	int givenAmmo;
	int weapon, targWeap;
	int maxGive; //the max ammount of ammo you can give this player(just in case a player gives too much ammo they will get it back)

	if (!g_allowGiveAmmo.integer) {
		CPx(ent - g_entities, va("cp \"^3/giveammo^7 is not enabled on this server!\"1"));
		return;
	}

	trap_Argv(1, arg1, sizeof(arg1));
	givenAmmo = atoi(arg1);

	if (ent->client->ps.stats[STAT_HEALTH] <= 0) {
		CPx(ent - g_entities, va("cp \"You have to be alive to do that!\"1"));
		return;
	}

	AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);

	VectorCopy(ent->s.pos.trBase, start);   //set 'start' to the player's position (plus the viewheight)
	start[2] += ent->client->ps.viewheight;
	VectorMA(start, 128, forward, end);    //put 'end' 128 units forward of 'start'

	//see if we hit anything between 'start' and 'end'
	trap_Trace(&tr, start, NULL, NULL, end, ent->s.number, CONTENTS_BODY);

	//if we didn't hit a player, return
	if (tr.entityNum >= MAX_CLIENTS) {
		return;
	}

	target = &(g_entities[tr.entityNum]);

	if ((!target->inuse) || (!target->client)) { //if the player is lagged/disconnected/etc
		return;
	}
	if (target->client->sess.sessionTeam != ent->client->sess.sessionTeam) { //not on same team
		CPx(ent - g_entities, va("cp \"What are you a spy or something!\"1"));
		return;
	}

	if (target->client->ps.stats[STAT_HEALTH] <= 0) {   //if they're dead
		CPx(ent - g_entities, va("cp \"You can not give ammo to the dead!\"1"));
		return;
	}

	//if given ammo is 0 or unset make it the size of a clip
	if (givenAmmo <= 0) {
		givenAmmo = ammoTable[ent->client->ps.weapon].maxclip;
	}

	weapon = ent->client->ps.weapon;

	switch (weapon) {
	case WP_LUGER:
	case WP_COLT:

		if (ent->client->ps.ammo[BG_FindClipForWeapon(ent->client->ps.weapon)] < givenAmmo) {
			CPx(ent - g_entities, va("cp \"You do not have enough ammo to give ^7%i^3!\"2", givenAmmo));
			return;
		}

		if (!COM_BitCheck(target->client->ps.weapons, WP_LUGER) && !COM_BitCheck(target->client->ps.weapons, WP_COLT)) {
			CPx(ent - g_entities, va("cp \"%s ^7does not have a Pistol!\"2", target->client->pers.netname));
			return;
		}

		if (COM_BitCheck(target->client->ps.weapons, WP_LUGER)) {
			targWeap = WP_LUGER;
		}
		else {
			targWeap = WP_COLT;
		}

		maxGive = (ammoTable[targWeap].maxclip * 4) - target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] > ammoTable[targWeap].maxclip * 4) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] = ammoTable[targWeap].maxclip * 4;
		}

		break;

	case WP_MP40:
	case WP_THOMPSON:
	case WP_STEN:

		if (ent->client->ps.ammo[BG_FindClipForWeapon(ent->client->ps.weapon)] < givenAmmo) {
			CPx(ent - g_entities, va("cp \"You do not have enough ammo to give ^7%i^7!\"2", givenAmmo));
			return;
		}

		if (!COM_BitCheck(target->client->ps.weapons, WP_MP40) && !COM_BitCheck(target->client->ps.weapons, WP_THOMPSON) && !COM_BitCheck(target->client->ps.weapons, WP_STEN)) {
			CPx(ent - g_entities, va("cp \"%s ^7does not have a SMG!\"2", target->client->pers.netname));
			return;
		}

		if (COM_BitCheck(target->client->ps.weapons, WP_MP40)) {
			targWeap = WP_MP40;
		}
		else if (COM_BitCheck(target->client->ps.weapons, WP_THOMPSON)) {
			targWeap = WP_THOMPSON;
		}
		else {
			targWeap = WP_STEN;
		}

		maxGive = (ammoTable[targWeap].maxclip * 3) - target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] > ammoTable[targWeap].maxclip * 3) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(targWeap)] = ammoTable[targWeap].maxclip * 3;
		}

		break;

	case WP_MAUSER:
	case WP_SNIPERRIFLE:

		if (ent->client->ps.ammo[BG_FindClipForWeapon(ent->client->ps.weapon)] < givenAmmo) {
			CPx(ent - g_entities, va("cp \"You do not have enough ammo to give ^7%i^7!\"2", givenAmmo));
			return;
		}

		if (!COM_BitCheck(target->client->ps.weapons, WP_SNIPERRIFLE) && !COM_BitCheck(target->client->ps.weapons, WP_MAUSER)) {
			CPx(ent - g_entities, va("cp \"%s ^7does not have a Mauser!\"2", target->client->pers.netname));
			return;
		}
		
		maxGive = (ammoTable[WP_SNIPERRIFLE].maxclip * 3) - target->client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)] > ammoTable[WP_SNIPERRIFLE].maxclip * 3) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(WP_SNIPERRIFLE)] = ammoTable[WP_SNIPERRIFLE].maxclip * 3;
		}

		maxGive = (ammoTable[WP_MAUSER].maxclip * 3) - target->client->ps.ammo[BG_FindAmmoForWeapon(WP_MAUSER)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(WP_MAUSER)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(WP_MAUSER)] > ammoTable[WP_MAUSER].maxclip * 3) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(WP_MAUSER)] = ammoTable[WP_MAUSER].maxclip * 3;
		}
		break;


	case WP_PANZERFAUST:

		if (ent->client->ps.ammo[BG_FindClipForWeapon(ent->client->ps.weapon)] < givenAmmo) {
			CPx(ent - g_entities, va("cp \"You do not have enough ammo to give ^7%i^7!\"2", givenAmmo));
			return;
		}

		if (!COM_BitCheck(target->client->ps.weapons, WP_PANZERFAUST)) {
			CPx(ent - g_entities, va("cp \"%s ^7does not have a Panzerfaust!\"2", target->client->pers.netname));
			return;
		}
		maxGive = (ammoTable[WP_PANZERFAUST].maxclip * 3) - target->client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] > ammoTable[WP_PANZERFAUST].maxclip * 3) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(WP_PANZERFAUST)] = ammoTable[WP_PANZERFAUST].maxclip * 3;
		}

		break;


	case WP_VENOM:

		if (ent->client->ps.ammo[BG_FindClipForWeapon(ent->client->ps.weapon)] < givenAmmo) {
			CPx(ent - g_entities, va("cp \"You do not have enough ammo to give ^7%i^7!\"2", givenAmmo));
			return;
		}

		if (!COM_BitCheck(target->client->ps.weapons, WP_VENOM)) {
			CPx(ent - g_entities, va("cp \"^7%s ^7does not have a Venom!\"2", target->client->pers.netname));
			return;
		}
		maxGive = (ammoTable[WP_VENOM].maxclip * 3) - target->client->ps.ammo[BG_FindAmmoForWeapon(WP_VENOM)];
		target->client->ps.ammo[BG_FindAmmoForWeapon(WP_VENOM)] += givenAmmo;
		if (target->client->ps.ammo[BG_FindAmmoForWeapon(WP_VENOM)] > ammoTable[WP_VENOM].maxclip * 3) {
			target->client->ps.ammo[BG_FindAmmoForWeapon(WP_VENOM)] = ammoTable[WP_VENOM].maxclip * 3;
		}
		break;

	default:
		CPx(ent - g_entities, va("cp \"You can not use ^3/giveammo^7 with this weapon!\"2"));
		return;
	}

	if (maxGive <= 0) {
		CPx(ent - g_entities, va("cp \"%s ^7does not need any ammo!\"3", target->client->pers.netname));
		return;
	}

	if (givenAmmo > maxGive) {
		CPx(ent - g_entities, va("cp \"You gave ^3%s^7 ammo, ^3%i ^7was all he could carry!\"3", target->client->pers.netname, maxGive));
		CPx(target - g_entities, va("cp \"%s^7 gave you ^3%i ^7ammo!\"3", ent->client->pers.netname, maxGive));
		ent->client->ps.ammo[BG_FindClipForWeapon(weapon)] -= maxGive;

	}
	else {

		CPx(ent - g_entities, va("cp \"You gave ^3%i^7 ammo to %s^7!\"3", givenAmmo, target->client->pers.netname));
		CPx(target - g_entities, va("cp \"%s^7 gave you ^3%i ^7ammo!\"3", ent->client->pers.netname, givenAmmo));
		ent->client->ps.ammo[BG_FindClipForWeapon(weapon)] -= givenAmmo;
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
		CP(va("print \"%s^7 sent a spectator invitation to %s^7.\n\"", ent->client->pers.netname, player->client->pers.netname));
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
		CP(va("print \"%s^7 revoked spectate invitation from %s^7.\n\"", ent->client->pers.netname, player->client->pers.netname));
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
		AP(va("print \"%s^7 ^nSPEC%s ^7team %s\"2", ent->client->pers.netname, (lock ? "LOCKED" : "UNLOCKED"), aTeams[team]));
		AP(va("cp \"%s is now ^nSPEC%s\"2", aTeams[team], (lock ? "LOCKED" : "UNLOCKED")));

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

	if (g_gamestate.integer != GS_PLAYING) {
		CP("print \"^1Error^7: Pause feature can only be used during a match!\n\"");
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
			AP(va("@print \"%s ^7has ^3PAUSED^7 the match!\n\"", ent->client->pers.netname));
			//APS("xmodTE/sound/misc/paused.wav");
			
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


// ************** LOCK / UNLOCK
//
// Locks/unlocks a player's team.
void pCmd_Lock(gentity_t *ent, qboolean fLock) { //unsigned int dwCommand,
	char *lock_status[2] = { "unlock", "lock" };
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
		
		if (cl->sess.referee && !cl->sess.incognito) {			
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
	// Just swallow it...
	if (g_gamestate.integer == GS_PLAYING) {		
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
			AP(va("@print \"%s ^7is %s%s.\n\"", ent->client->pers.netname, (state ? "^n" : "^z"), status[state]));
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
		AP(va("@print \"%s ^7set %s team to %s%s.\n\"", ent->client->pers.netname, aTeams[team], (ready ? "^n" : "^z"), status[ready]));
		level.readyTeam[team] = ready;
	}
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
	{ "ref",				G_ref_cmd,			qtrue,		qtrue,	qfalse, qtrue },
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

