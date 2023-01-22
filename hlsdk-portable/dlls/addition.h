#ifndef ADDITION_H
#define ADDITION_H
void ClientPrintf( CBasePlayer *pPlayer, const char *format, ... );
const char *GetAuthID( CBasePlayer *pPlayer );
const char *PlayerName( CBaseEntity *pPlayer );
CBasePlayer* GetPlayerByUID( int userId );
bool Addition_ClientCommand( CBasePlayer *player, const char *pCmd );
void KickCheater( CBasePlayer *player, char *CheatType );
#endif
