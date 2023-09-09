#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "admin.h"
#include "addition.h"
#include <ctime>

/*
=====================
ClientPrintf

Print message to player
=====================
*/
void ClientPrintf( CBasePlayer *pPlayer, const char *format, ... )
{
	va_list	argptr;
	char string[256];

	va_start( argptr, format );
	int len = vsnprintf( string, 256, format, argptr );
	va_end( argptr );
	string[len] = 0;

	//ClientPrint( &player->v, HUD_PRINTCONSOLE, string );
	CLIENT_PRINTF( pPlayer->edict(), print_chat, string );
}

/*
=====================
FilterFileName

Allow only safe characters
=====================
*/
bool FilterFileName( const char *name )
{
	while( name && *name )
	{
		if( ( *name >= 'A' && *name <= 'z' ) || ( *name >= '0' && *name <= '9' ) || *name == '_' )
		{
			name++;
			continue;
		}
		return false;
	}

	return true;
}

/*
=====================
GetAuthId

Calculate auth id for connected client
This does not require player state filled
=====================
*/
const char *GetAuthID( CBasePlayer *pPlayer )
{
	static char uid[33];
	const char *authid = GETPLAYERAUTHID( pPlayer->edict() );

	if( !authid || strstr(authid, "PENDING") )
	{
		const char *ip = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "ip" );
		if( ip )
		{
			char *pUid;

			snprintf( uid, 32, "IP_%s", ip );

			for( pUid = uid; *pUid; pUid++ )
				if( *pUid == '.' ) *pUid = '_';
		}
		else
			return "UNKNOWN";
	}
	else strncpy( uid, authid, 32 );

	if( FilterFileName( uid ) )
		return uid;

	return "UNKNOWN";
}

/*
=====================
PlayerName

Return readable player name
=====================
*/
const char *PlayerName( CBaseEntity *pPlayer )
{
	if( !pPlayer )
		return "unnamed(NULL)";
	return (const char*)( ( pPlayer->pev->netname && ( STRING( pPlayer->pev->netname ) )[0] != 0 ) ? STRING( pPlayer->pev->netname ) : "unconnected" );
}

/*
=====================
GetPlayerByUID

Check if it is a player and returns pointer
=====================
*/
CBasePlayer* GetPlayerByUID( int userId )
{
        CBasePlayer *client = NULL;

        while( ( ( client = (CBasePlayer*)UTIL_FindEntityByClassname( client, "player" ) ) != NULL ) )
        {
                if( userId == GETPLAYERUSERID( client->edict() ) )
                        return client;
        }

        return NULL;
}

/*
=====================
Addition_ClientCommand

Handle client commands and redirect to related functions
=====================
*/
bool Addition_ClientCommand( CBasePlayer *pPlayer, const char *pCmd )
{
	if( Admin_ClientCommand( pPlayer->edict() ) )
		return true;
}

/*
=====================
KickCheater

Handle cheater kick
=====================
*/
void KickCheater( CBasePlayer *player, char *CheatType )
{
	char time_str[80];
    time_t date_time = time(0);
    strftime(time_str, 80, "%Y.%m.%d %T", localtime(&date_time));
	FILE *flch = fopen("cheaters.log", "a");
	const char *ip = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( player->edict() ), "ip" );

	if( !ip )
	{
		ip = "UNKNOWN";
	}

	fprintf( flch , "%s name: %s ip: %s id: %s cheat: %s\n", time_str, PlayerName(player), ip, GETPLAYERAUTHID(player->edict()), CheatType);
	SERVER_COMMAND(UTIL_VarArgs("kick #%i %s\n", GETPLAYERUSERID(player->edict()), CheatType ));
	fclose( flch );
}

/*
=====================
PrintMsg
=====================
*/
void PrintMsg(CBasePlayer *player, char *sText, int fHoldTime, int fadeinTime, int fadeoutTime, int fxTime, float x, float y, int iChannel, int r, int g, int b)
{
	char szText[256];
	hudtextparms_t hText = {0};
	hText.channel = iChannel;

	hText.x = x;
	hText.y = y;

	hText.r1 = r;
	hText.g1 = g;
	hText.b1 = b;
	hText.a1 = 0;

	hText.r2 = hText.g2 = hText.b2 = 0;
	hText.a2 = 0;

	hText.holdTime = fHoldTime;

	hText.fadeinTime = fadeinTime; //0.000
	hText.fadeoutTime = fadeoutTime; //0.000
	hText.fxTime = fxTime; //0.25

	UTIL_HudMessage(player, hText, sText);
}

void PrintClientMsg(entvars_t *client, char *sText, int fHoldTime, int fadeinTime, int fadeoutTime, int fxTime, float x, float y, int iChannel, int r, int g, int b)
{
	char szText[256];
	hudtextparms_t hText = {0};
	hText.channel = iChannel;

	hText.x = x;
	hText.y = y;

	hText.r1 = r;
	hText.g1 = g;
	hText.b1 = b;
	hText.a1 = 0;

	hText.r2 = hText.g2 = hText.b2 = 0;
	hText.a2 = 0;

	hText.holdTime = fHoldTime;

	hText.fadeinTime = fadeinTime; //0.000
	hText.fadeoutTime = fadeoutTime; //0.000
	hText.fxTime = fxTime; //0.25

	UTIL_HudMessagePlayer(client, hText, sText);
}
