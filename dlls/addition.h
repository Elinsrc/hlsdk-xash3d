#ifndef ADDITION_H
#define ADDITION_H
void ClientPrintf( CBasePlayer *pPlayer, const char *format, ... );
const char *GetAuthID( CBasePlayer *pPlayer );
const char *PlayerName( CBaseEntity *pPlayer );
CBasePlayer* GetPlayerByUID( int userId );
bool Addition_ClientCommand( CBasePlayer *player, const char *pCmd );
void KickCheater( CBasePlayer *player, char *CheatType );
void PrintMsg(CBasePlayer *player, char *sText, int fHoldTime, int fadeinTime, int fadeoutTime, int fxTime, float x, float y, int iChannel, int r, int g, int b);
#endif
