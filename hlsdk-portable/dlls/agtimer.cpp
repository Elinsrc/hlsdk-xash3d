//++ BulliT

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "game.h"
#include "player.h"
#include "agtimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgTimer::AgTimer()
{
	m_fNextTimerUpdate = gpGlobals->time + 3.0; // Dont start timer directly.
	m_fLastTimeCheck = gpGlobals->time;
	m_fEffectiveTime = 0.0;
	m_pmp_timelimit = CVAR_GET_POINTER("mp_timelimit");
}

AgTimer::~AgTimer()
{

}

// TITLES FOR HALF-LIFE
// Position command $position x y
// x & y are from 0 to 1 to be screen resolution independent
// -1 means center in each dimension
// Effect command $effect <effect number>
// effect 0 is fade in/fade out
// effect 1 is flickery credits
// effect 2 is write out (training room)
// Text color r g b command $color
// fadein time fadeout time / hold time
// $fadein (message fade in time - per character in effect 2)
// $fadeout (message fade out time)
// $holdtime (stay on the screen for this long)
void AgTimer::Print(char *sText, int fHoldTime, float x, float y, int iChannel)
{
        hudtextparms_t hText;
        memset(&hText, 0, sizeof(hText));

        hText.channel = iChannel;
        // These X and Y coordinates are just above the health meter.
        hText.x = x;
        hText.y = y;

        hText.r1 = hText.g1 = hText.b1 = 180;
        hText.a1 = 0;

        hText.r2 = hText.g2 = hText.b2 = 0;
        hText.a2 = 0;

        hText.holdTime = fHoldTime - 0.30;

        hText.fadeinTime = 0.000;
        hText.fadeoutTime = 0.000;
        hText.fxTime = 0.25;

	UTIL_HudMessageAll( hText, sText );
}

void AgTimer::Think()
{
	if ( !mp_clock.value )
		return;

	// Calculate effective time
	m_fEffectiveTime += gpGlobals->time - m_fLastTimeCheck;

	m_fLastTimeCheck = gpGlobals->time;

	if( m_fNextTimerUpdate <= m_fEffectiveTime )
	{
		// Sanity time check
		if( timelimit.value > 4320 )
        		CVAR_SET_FLOAT( "mp_timelimit", 4320 ); // Three days maximum

		// Write the time. (negative turns off timer on client)
		long lTime = (m_pmp_timelimit->value * 60) - m_fEffectiveTime;

		if( lTime > 0 )
		{
			char szTime[128];
			if( 86400 < lTime )
			{
				//More than one day. Print days, hour, minutes and seconds
				ldiv_t d1 = ldiv( lTime, 86400 );
				ldiv_t d2 = ldiv( d1.rem, 3600 );
				ldiv_t d3 = ldiv( d2.rem, 60 );
				sprintf( szTime, "%ldd %ldh %02ldm %02lds\n", d1.quot, d2.quot, d3.quot, d3.rem );
			}
			else if( 3600 < lTime )
			{
				// More than one hour. Print hour, minutes and seconds
				ldiv_t d1 = ldiv( lTime, 3600 );
				ldiv_t d2 = ldiv( d1.rem, 60 );
				sprintf( szTime, "%ldh %02ldm %02lds\n", d1.quot, d2.quot, d2.rem );
			}
			else if( 60 < lTime )
			{
				// More than one minute. Print minutes and seconds.
				ldiv_t d = ldiv( lTime, 60 );
				sprintf( szTime, "%ld:%02ld\n", d.quot, d.rem );
			}
			else
			{
				// Less than a minute left. Print seconds.
				sprintf( szTime, "%ld\n", lTime );
			}

			Print( szTime, 60, 0.5, 0.01, 1 );
				m_fNextTimerUpdate += 1;
		}
	}
}
//-- Martin Webrant
