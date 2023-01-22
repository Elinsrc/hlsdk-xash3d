#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "addition.h"
#include <ctime>

extern int gEvilImpulse101;

cvar_t admin_password = { "admin_password", "", FCVAR_SERVER | FCVAR_UNLOGGED };
cvar_t admin_kickonfail = { "admin_kickonfail", "0", FCVAR_SERVER };

void Admin_RegisterCVars( void )
{
	CVAR_REGISTER( &admin_kickonfail );
	CVAR_REGISTER( &admin_password );
}

void Admin_LogAttempt( CBasePlayer *pPlayer, char *LogType )
{
		FILE *fladminlog = fopen("adminattempts.log", "a");
		char time_str[80];
		time_t date_time = time(0);
		strftime(time_str, 80, "%Y.%m.%d %T", localtime(&date_time));
		const char *ip = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "ip" );

		if( !ip || !ip[0] )
			ip = "UNKNOWN";

		fprintf( fladminlog, "%s %s %s %s %s\n", time_str, LogType, ip, GETPLAYERAUTHID( pPlayer->edict() ), PlayerName( pPlayer ) ); // Timestamp, LogType, IP Address, XashID, Nickname
		fclose( fladminlog );

		if( admin_kickonfail.value && !strcmp( LogType, "Failure login:" ) )
		{
			SERVER_COMMAND( UTIL_VarArgs( "kick #%i ""authorization failure""\n", GETPLAYERUSERID( pPlayer->edict() ) ) );
		}
}

bool Admin_ClientCommand( edict_t *pEntity )
{
        const char *pCmd = CMD_ARGV(0);
        CBasePlayer *pPlayer = (CBasePlayer*)GET_PRIVATE(pEntity);

	if( FStrEq(pCmd, "admin_login") )
	{
		if( !admin_password.string[0] || pPlayer->IsAdmin )
		{
			return false;
		}

		if( CMD_ARGC() != 2 )
		{
			ClientPrintf( pPlayer, "Usage: admin_login <password>\n" );
			return true;
		}

		const char *passwordargv = CMD_ARGV( 1 );

		if( !strcmp( passwordargv, admin_password.string ) )
		{
			pPlayer->IsAdmin = true;
			ClientPrintf( pPlayer, "Login successful\n" );
			ALERT( at_console, "Player %s: Became admin\n", PlayerName( pPlayer ) );
			Admin_LogAttempt( pPlayer, "Became admin:" );
		}
		else
		{
			ClientPrintf( pPlayer, "Login failed\n" );
			ALERT( at_console, "Player %s: Login failure\n", PlayerName( pPlayer ) );
			Admin_LogAttempt( pPlayer, "Failure login:" );
		}
		return true;
	}
	if( !pPlayer->IsAdmin )
		return true;

	if( FStrEq(pCmd, "admin_sudo") )
	{
		if( CMD_ARGC() != 3 )
		{
			ClientPrintf( pPlayer, "Usage: admin_sudo <UserID> <Command>\n" );
			return true;
		}

		short int UserID;

		if( CMD_ARGV(1)[0] == '#' )
			UserID = atoi( CMD_ARGV( 1 ) + 1 );
		else
			UserID = atoi( CMD_ARGV( 1 ) );

		const char *Command = (char *)CMD_ARGV( 2 );

		CBasePlayer *pSudoer = GetPlayerByUID( UserID );

		if( !pSudoer )
		{
			ClientPrintf( pPlayer, "Invalid player!\n" );
			return true;
		}

		CLIENT_COMMAND( pSudoer->edict(), "%s\n", Command );
		ClientPrintf( pPlayer, "Command was sent to %s\n", STRING( pSudoer->pev->netname ) );
		return true;
	}
	else if( FStrEq(pCmd, "admin_strip") )
	{
		if( CMD_ARGC() > 3 )
		{
			ClientPrintf( pPlayer, "Usage: admin_strip <1/0> <UserID>\n" );
			return true;
		}

		BOOL StripSuit = atoi( CMD_ARGV( 1 ) );
		short int UserID = atoi( CMD_ARGV( 2 ) );
		CBasePlayer *pStripper = NULL;

		if( CMD_ARGV( 2 ) != NULL )
			pStripper = GetPlayerByUID( UserID );
		else
			pStripper = pPlayer;

		if( !pStripper && !UserID && pStripper != pPlayer )
		{
			ClientPrintf( pPlayer, "Invalid player!\n" );
			return true;
		}

		if( pStripper == pPlayer )
			ClientPrintf( pPlayer, "Removed all your items\n" );
		else
			ClientPrintf( pPlayer, "Removed all %s items\n", STRING( pStripper->pev->netname ) );

		if( StripSuit )
		{
			pStripper->RemoveAllItems( TRUE );
		}
		else
		{
			pStripper->RemoveAllItems( FALSE );
		}

		return true;
	}
	else if( FStrEq(pCmd, "admin_notarget") || FStrEq(pCmd, "admin_notar") )
	{
		if( !FBitSet( pPlayer->pev->flags, FL_NOTARGET ) )
		{
			pPlayer->pev->flags |= FL_NOTARGET;
			ClientPrintf( pPlayer, "Admin notarget ON\n" );
		}
		else
		{
			pPlayer->pev->flags &= ~FL_NOTARGET;
			ClientPrintf( pPlayer, "Admin notarget OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_god") || FStrEq(pCmd, "admin_godmode") )
	{
		if( !FBitSet( pPlayer->pev->flags, FL_GODMODE ) )
		{
			pPlayer->pev->flags |= FL_GODMODE;
			ClientPrintf( pPlayer, "Admin godmode ON\n" );
		}
		else
		{
			pPlayer->pev->flags &= ~FL_GODMODE;
			ClientPrintf( pPlayer, "Admin godmode OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_invis") || FStrEq(pCmd, "admin_invisibility") )
	{
		if( pPlayer->pev->solid != SOLID_NOT )
		{
			pPlayer->pev->movetype = MOVETYPE_NOCLIP;
			pPlayer->pev->solid = SOLID_NOT;
			pPlayer->pev->takedamage = DAMAGE_NO;
			pPlayer->pev->effects |= EF_NODRAW;
			pPlayer->pev->flags |= FL_NOTARGET;
			pPlayer->pev->flags |= FL_GODMODE;
			pPlayer->m_fNoPlayerSound = TRUE;
			ClientPrintf( pPlayer, "Admin invisibility ON\n" );
		}
		else
		{
			pPlayer->pev->takedamage = DAMAGE_YES;
			pPlayer->pev->movetype = MOVETYPE_WALK;
			pPlayer->pev->flags &= ~FL_NOTARGET;
			pPlayer->pev->flags &= ~FL_GODMODE;
			pPlayer->pev->effects &= ~EF_NODRAW;
			pPlayer->pev->solid = SOLID_SLIDEBOX;
			pPlayer->m_fNoPlayerSound = FALSE;
			ClientPrintf( pPlayer, "Admin invisibility OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_hologram") )
	{
		if( pPlayer->pev->renderfx != kRenderFxHologram )
		{
			pPlayer->pev->rendermode = kRenderTransAdd;
			pPlayer->pev->renderfx = kRenderFxHologram;
			pPlayer->pev->renderamt = 255;
			ClientPrintf( pPlayer, "Admin hologram ON\n" );
		}
		else
		{
			pPlayer->pev->rendermode = kRenderNormal;
			pPlayer->pev->renderfx = kRenderFxNone;
			pPlayer->pev->renderamt = 0;
			ClientPrintf( pPlayer, "Admin hologram OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_noclip") )
	{
		if( pPlayer->pev->movetype != MOVETYPE_NOCLIP )
		{
			pPlayer->pev->movetype = MOVETYPE_NOCLIP;
			ClientPrintf( pPlayer, "Admin noclip ON\n" );
		}
		else
		{
			pPlayer->pev->movetype = MOVETYPE_WALK;
			ClientPrintf( pPlayer, "Admin noclip OFF\n" );
		}
		return true;
	}
	else if( FStrEq(pCmd, "admin_101" ) )
	{
		gEvilImpulse101 = true;
		pPlayer->GiveNamedItem( "item_longjump" );
		pPlayer->GiveNamedItem( "item_battery" );
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveNamedItem( "ammo_9mmclip" );
		pPlayer->GiveNamedItem( "weapon_shotgun" );
		pPlayer->GiveNamedItem( "ammo_buckshot" );
		pPlayer->GiveNamedItem( "weapon_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_ARgrenades" );
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
		pPlayer->GiveNamedItem( "weapon_tripmine" );
		pPlayer->GiveNamedItem( "weapon_357" );
		pPlayer->GiveNamedItem( "ammo_357" );
		pPlayer->GiveNamedItem( "weapon_crossbow" );
		pPlayer->GiveNamedItem( "ammo_crossbow" );
		pPlayer->GiveNamedItem( "weapon_egon" );
		pPlayer->GiveNamedItem( "weapon_gauss" );
		pPlayer->GiveNamedItem( "ammo_gaussclip" );
		pPlayer->GiveNamedItem( "weapon_rpg" );
		pPlayer->GiveNamedItem( "ammo_rpgclip" );
		pPlayer->GiveNamedItem( "weapon_satchel" );
		pPlayer->GiveNamedItem( "weapon_snark" );
		pPlayer->GiveNamedItem( "weapon_hornetgun" );
		gEvilImpulse101 = FALSE;
		return true;
	}
	else if( FStrEq(pCmd, "admin_logout" ) )
	{
		pPlayer->IsAdmin = false;
		ClientPrintf( pPlayer, "Successfully logged out\n" );
		ALERT( at_console, "Admin %s: Logging out\n", PlayerName( pPlayer ) );
		Admin_LogAttempt( pPlayer, "Successful logout:" );
		return true;
	}
	return false;
}
