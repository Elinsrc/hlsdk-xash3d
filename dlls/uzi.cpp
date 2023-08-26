#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

class Cuzi : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot(void) { return 2; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

};

enum uzi_e
{
	UZI_IDLE,
	UZI_IDLE2,
	UZI_IDLE3,
	UZI_RELOAD,
	UZI_DEPLOY,
	UZI_SHOOT,
	UZI_HOLSTER
};

LINK_ENTITY_TO_CLASS( weapon_uzi, Cuzi );

void Cuzi::Spawn( )
{
	Precache( );
	pev->classname = MAKE_STRING("weapon_uzi");
	SET_MODEL(ENT(pev), "models/w_uzi.mdl");
	m_iId = WEAPON_UZI;
	m_iDefaultAmmo = 64;
	FallInit();
}

void Cuzi::Precache( void )
{
	PRECACHE_MODEL("models/v_uzi.mdl");
	PRECACHE_MODEL("models/w_uzi.mdl");
	PRECACHE_MODEL("models/p_uzi.mdl");

	PRECACHE_SOUND("weapons/uzi_reload1.wav");
	PRECACHE_SOUND("weapons/uzi_reload2.wav");
	PRECACHE_SOUND("weapons/uzi_reload3.wav");

	PRECACHE_SOUND ("weapons/uzi_fire.wav");
}

int Cuzi::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 32;
	p->iSlot = 1;
	p->iPosition = 3;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_UZI;
	p->iWeight = 5;
	return 1;
}

int Cuzi::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL Cuzi::Deploy( )
{
	return DefaultDeploy( "models/v_uzi.mdl", "models/p_uzi.mdl", UZI_DEPLOY, "MP5" );
}

void Cuzi::Holster( )
{
	m_fInReload = FALSE;
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1;
	SendWeaponAnim( UZI_HOLSTER );
}

void Cuzi::PrimaryAttack()
{
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		return;
	}

	if (m_iClip <= 0)
	{
		m_flNextPrimaryAttack = gpGlobals->time+0.5;
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	
	m_iClip --;
	
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecDir;
	
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/uzi_fire.wav", 1, ATTN_NORM);
	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_MP5, 10, 10, m_pPlayer->pev, m_pPlayer->random_seed);
	
	SendWeaponAnim( UZI_SHOOT );
	
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = gpGlobals->time + 0.086;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (5, 8);
}

void Cuzi::Reload( void )
{
  if( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 32 )
		return;
	
	DefaultReload(32, UZI_RELOAD, 2.8, 0.8);
}

void Cuzi::WeaponIdle( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	switch (RANDOM_LONG(0,2))
	{
		case 0:	SendWeaponAnim( UZI_IDLE ); break;
		case 1:	SendWeaponAnim( UZI_IDLE2 ); break;
		case 2:	SendWeaponAnim( UZI_IDLE3 ); break;
	}
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT (8, 12);
}
