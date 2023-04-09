/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "addition.h"

#define BOOMBOX_VOLUME 1000

class CBoombox : public CBasePlayerWeapon
{
public:
	static const char *pBoomboxSongs[];
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 1; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( int skiplocal = 0 );
	void WeaponIdle( void );
	int m_iSwitchSound;
};

LINK_ENTITY_TO_CLASS( weapon_boombox, CBoombox );

enum gauss_e {
	BOOMBOX_IDLE = 0,
	BOOMBOX_DRAW,
	BOOMBOX_HOLSTER
};


const char *CBoombox::pBoomboxSongs[] =
{
	"bbox/MoonDeityFirstPlace.mp3",
	"bbox/KordhellMurderInMyMind.mp3",
	"bbox/GhostfacePlayaWhyNot.mp3",
	"bbox/DVRSTCloseEyes.mp3",
	"bbox/KordhellLIVEANOTHERDAY.mp3",
	"bbox/ONIMXRUSTRAWANGLEPSYCHOCRUISE.mp3",
	"bbox/PlayaPhonkPHONKYTOWN.mp3",
	"bbox/SHADXWBXRNARCHEZKXNVRAPRINCEOFDARKNESS.mp3",
	"bbox/ZODIVKDevilEyes.mp3"
};

void CBoombox::Spawn( )
{
	Precache( );
	m_iId = WEAPON_BOOMBOX;
	SET_MODEL(ENT(pev), "models/w_boombox.mdl");
	m_iClip = -1;

	m_iSwitchSound = 0;

	FallInit();// get ready to fall down.
}

void CBoombox::Precache( void )
{
	int i;
	PRECACHE_MODEL("models/v_boombox.mdl");
	PRECACHE_MODEL("models/w_boombox.mdl");
	PRECACHE_MODEL("models/p_boombox.mdl");
	for ( i = 0; i < ARRAYSIZE( pBoomboxSongs ); i++ )
		PRECACHE_SOUND(pBoomboxSongs[i]);
	PRECACHE_SOUND("bbox/songrc.wav");
}

int CBoombox::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = WEAPON_BOOMBOX;
	p->iWeight = 0;
	return 1;
}

int CBoombox::AddToPlayer( CBasePlayer *pPlayer )
{
	if( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}


BOOL CBoombox::Deploy( )
{
	PrintMsg(m_pPlayer, "Bass\nSECONDARY FIRE: Switches music!", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
	return DefaultDeploy( "models/v_boombox.mdl", "models/p_boombox.mdl", BOOMBOX_DRAW, "rpg", 0 );
}

void CBoombox::Holster( int skiplocal /* = 0 */ )
{
	int i;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;
	SendWeaponAnim( BOOMBOX_HOLSTER );
	for ( i = 0; i < ARRAYSIZE( pBoomboxSongs ); i++ )
	{
		STOP_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, pBoomboxSongs[i]);
	}
	m_iSwitchSound = 0;
	PrintMsg(m_pPlayer, "", 0, 0.000, 0.000, 0.000, 0.000,  0.000, 4, 0, 0, 0);
}

void CBoombox::PrimaryAttack()
{	
	m_pPlayer->m_iWeaponVolume = BOOMBOX_VOLUME;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5f;

	switch( m_iSwitchSound )
	{
	case 0:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/songrc.wav", 1, ATTN_NORM); //1.83
		UTIL_ScreenShake( m_pPlayer->pev->origin, 25.0, 200.0, 2, 750 );

		RadiusDamage( m_pPlayer->pev->origin, pev, m_pPlayer->pev, 21, 750, CLASS_NONE, DMG_ALWAYSGIB );
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.75f;
		break;
	case 1:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/MoonDeityFirstPlace.mp3", 1, ATTN_NORM);;
		break;
	case 2:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/KordhellMurderInMyMind.mp3", 1, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/GhostfacePlayaWhyNot.mp3", 1, ATTN_NORM);
		break;
	case 4:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/DVRSTCloseEyes.mp3", 1, ATTN_NORM);;
		break;
	case 5:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/KordhellLIVEANOTHERDAY.mp3", 1, ATTN_NORM);
		break;
	case 6:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/ONIMXRUSTRAWANGLEPSYCHOCRUISE.mp3", 1, ATTN_NORM);
		break;
	case 7:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/PlayaPhonkPHONKYTOWN.mp3", 1, ATTN_NORM);;
		break;
	case 8:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/SHADXWBXRNARCHEZKXNVRAPRINCEOFDARKNESS.mp3", 1, ATTN_NORM);
		break;
	case 9:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "bbox/ZODIVKDevilEyes.mp3", 1, ATTN_NORM);
		break;
	}
}

void CBoombox::SecondaryAttack()
{	
	m_iSwitchSound = ( m_iSwitchSound + 1 ) % 11;

	switch( m_iSwitchSound )
	{
	case 0:
		PrintMsg(m_pPlayer, "\nBass", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 1:
		PrintMsg(m_pPlayer, "\nMoonDeity - First Place", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 2:
		PrintMsg(m_pPlayer, "\nKordhell - Murder In My Mind", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 3:
		PrintMsg(m_pPlayer, "\nGhostface Playa - Why Not", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 4:
		PrintMsg(m_pPlayer, "\nDVRST - Close Eyes", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 5:
		PrintMsg(m_pPlayer, "\nKordhell - LIVE ANOTHER DAY", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 6:
		PrintMsg(m_pPlayer, "\nONIMXRU, STRAWANGLE - PSYCHO CRUISE", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 7:
		PrintMsg(m_pPlayer, "\nPlayaPhonk - PHONKY TOWN", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 8:
		PrintMsg(m_pPlayer, "\nSHADXWBXRN, ARCHEZ, KXNVRA - PRINCE OF DARKNESS", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	case 9:
		PrintMsg(m_pPlayer, "\nZODIVK - Devil Eyes", 60, 0.000, 0.000, 0.25, -1,  0.875, 4, 255, 160, 0);
		break;
	}
	m_flNextSecondaryAttack = gpGlobals->time + 0.2f;
}

void CBoombox::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	SendWeaponAnim( BOOMBOX_IDLE );

	m_flTimeWeaponIdle = gpGlobals->time + 8.1f;
}
