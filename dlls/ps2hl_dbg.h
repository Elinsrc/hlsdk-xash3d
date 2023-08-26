/*

27.08.18

*/

// Include guard: start
#ifndef PS2_DBG_H
#define PS2_DBG_H

// Header files
//#include "util.h"
//#include "cbase.h"
#include "extdll.h"

// Source: https://www.moddb.com/games/half-life/tutorials/where-is-poppy-your-first-custom-entity-part-1
extern void DBG_RenderBBox(Vector origin, Vector mins, Vector maxs, int life, BYTE r = 0, BYTE b = 0, BYTE g = 0);

// Include guard: end
#endif
