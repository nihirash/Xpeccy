#ifndef _XPGSOUND
#define _XPGSOUND

#include <stdint.h>

#include "spectrum.h"
#include "sound.h"

#define GS_FRQ		12.0

#define GS_ENABLE	1
#define GS_RESET	2

#define	GS_MONO		0
#define	GS_12_34	1

uint8_t gs_in(int32_t);
void gs_out(int32_t,uint8_t);

class GS {
	public:
	GS();
	int flags;
	ZXBase* sys;
	uint32_t t;
	uint8_t pb3_gs;	// gs -> zx
	uint8_t pb3_zx;	// zx -> gs
	uint8_t pbb_zx;
	uint8_t rp0;
	uint8_t pstate;	// state register (d0, d7)
	int vol1,vol2,vol3,vol4;
	int ch1,ch2,ch3,ch4;
	int cnt;
	int stereo;
	void reset();
	bool in(int,uint8_t*);
	bool out(int,uint8_t);
	void sync(uint32_t);
	SndData getvol();
};

// extern GS *gs;

#endif
