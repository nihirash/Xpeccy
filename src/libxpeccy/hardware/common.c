#include "hardware.h"

// debug


int brkIn(Computer* comp, int port) {
	printf("IN %.4X (dos:rom:cpm = %i:%i:%i)\n",port,comp->dos,comp->rom,comp->cpm);
	assert(0);
	comp->brk = 1;
	return -1;
}

void brkOut(Computer* comp, int port, int val) {
	printf("OUT %.4X,%.2X (dos:rom:cpm = %i:%i:%i)\n",port,val,comp->dos,comp->rom,comp->cpm);
	assert(0);
	comp->brk = 1;
}

int dummyIn(Computer* comp, int port) {
	return -1;
}

void dummyOut(Computer* comp, int port, int val) {

}

// INT handle/check

void zx_sync(Computer* comp, int ns) {
	// devices
	difSync(comp->dif, ns);
	gsSync(comp->gs, ns);
	saaSync(comp->saa, ns);
	tsSync(comp->ts, ns);
	tapSync(comp->tape, ns);
	bcSync(comp->beep, ns);
	// nmi
	if ((comp->cpu->pc > 0x3fff) && comp->nmiRequest) {
		comp->cpu->intrq |= Z80_NMI;	// request nmi
		comp->dos = 1;			// set dos page
		comp->rom = 1;
		comp->hw->mapMem(comp);
	}
	// int
	if (comp->vid->intFRAME) {
		comp->intVector = 0xff;
		comp->cpu->intrq |= Z80_INT;
	} else if (comp->vid->intLINE) {
		comp->intVector = 0xfd;
		comp->cpu->intrq |= Z80_INT;
		comp->vid->intLINE = 0;
	} else if (comp->vid->intDMA) {
		comp->intVector = 0xfb;
		comp->cpu->intrq |= Z80_INT;
		comp->vid->intDMA = 0;
	} else if (comp->cpu->intrq & Z80_INT) {
		comp->cpu->intrq &= ~Z80_INT;
	}
}

void zx_init(Computer* comp) {
	int perNoTurbo = 1e3 / comp->cpuFrq;
	comp->fps = 50;
	vidUpdateTimings(comp->vid, perNoTurbo >> 1);
}

// zx keypress/release

void zx_keyp(Computer* comp, keyEntry ent) {
	kbdPress(comp->keyb, ent);
}

void zx_keyr(Computer* comp, keyEntry ent) {
	kbdRelease(comp->keyb, ent);
}

// volume

sndPair zx_vol(Computer* comp, sndVolume* sv) {
	sndPair vol;
	sndPair svol;
	int lev = 0;
	vol.left = 0;
	vol.right = 0;
	// 1:tape sound
//	if (comp->tape->on) {
		if (comp->tape->rec) {
			lev = comp->tape->levRec ? 0x1000 * sv->tape / 100 : 0;
		} else {
			lev = (comp->tape->volPlay << 8) * sv->tape / 1600;
		}
//	}
	// 2:beeper
	// bcSync(comp->beep, -1);
	lev += comp->beep->val * sv->beep / 6;
	vol.left = lev;
	vol.right = lev;
	// 3:turbo sound
	svol = tsGetVolume(comp->ts);
	vol.left += svol.left * sv->ay / 100;
	vol.right += svol.right * sv->ay / 100;
	// 4:general sound
	svol = gsVolume(comp->gs);
	vol.left += svol.left * sv->gs / 100;
	vol.right += svol.right * sv->gs / 100;
	// 5:soundrive
	svol = sdrvVolume(comp->sdrv);
	vol.left += svol.left * sv->sdrv / 100;
	vol.right += svol.right * sv->sdrv / 100;
	// 6:saa
	svol = saaVolume(comp->saa);
	vol.left += svol.left * sv->saa / 100;
	vol.right += svol.right * sv->saa / 100;
	// end
	return vol;
}

// set std zx palette

void zx_set_pal(Computer* comp) {
	int i;
	for (i = 0; i < 16; i++) {
		comp->vid->pal[i].b = (i & 1) ? ((i & 8) ? 0xff : 0xaa) : 0x00;
		comp->vid->pal[i].r = (i & 2) ? ((i & 8) ? 0xff : 0xaa) : 0x00;
		comp->vid->pal[i].g = (i & 4) ? ((i & 8) ? 0xff : 0xaa) : 0x00;
	}
}

// in

int zx_dev_wr(Computer* comp, int adr, int val, int dos) {
	int res = 0;
	res = gsWrite(comp->gs, adr, val);
	if (!dos) {
		res |= saaWrite(comp->saa, adr, val);
		res |= sdrvWrite(comp->sdrv, adr, val);
	}
	res |= ideOut(comp->ide, adr, val, dos);
	return res;
}

int zx_dev_rd(Computer* comp, int adr, int* ptr, int dos) {
	if (gsRead(comp->gs, adr, ptr)) return 1;
	if (ideIn(comp->ide, adr, ptr, dos)) return 1;
	return 0;
}

int xIn1F(Computer* comp, int port) {
	return joyInput(comp->joy);
}

int xInFE(Computer* comp, int port) {
	unsigned char res = kbdRead(comp->keyb, port) | 0xa0;		// set bits 7,5
	if (comp->tape->volPlay & 0x80)
		res |= 0x40;
	return res;
}

int xInFFFD(Computer* comp, int port) {
	return tsIn(comp->ts, 0xfffd);
}

int xInFADF(Computer* comp, int port) {
	unsigned char res = 0xff;
	comp->mouse->used = 1;
	if (!comp->mouse->enable) return res;
	if (comp->mouse->hasWheel) {
		res &= 0x0f;
		res |= ((comp->mouse->wheel & 0x0f) << 4);
	}
	res ^= comp->mouse->mmb ? 4 : 0;
	if (comp->mouse->swapButtons) {
		res ^= comp->mouse->rmb ? 1 : 0;
		res ^= comp->mouse->lmb ? 2 : 0;
	} else {
		res ^= comp->mouse->lmb ? 1 : 0;
		res ^= comp->mouse->rmb ? 2 : 0;
	}
	return res;
}

int xInFBDF(Computer* comp, int port) {
	comp->mouse->used = 1;
	return comp->mouse->enable ? comp->mouse->xpos : 0xff;
}

int xInFFDF(Computer* comp, int port) {
	comp->mouse->used = 1;
	return comp->mouse->enable ? comp->mouse->ypos : 0xff;
}

// out

void xOutFE(Computer* comp, int port, int val) {
	comp->vid->nextbrd = val & 0x07;
	comp->beep->lev = (val & 0x10) ? 1 : 0;
	comp->tape->levRec = (val & 0x08) ? 1 : 0;
}

void xOutBFFD(Computer* comp, int port, int val) {
	tsOut(comp->ts, 0xbffd, val);
}

void xOutFFFD(Computer* comp, int port, int val) {
	tsOut(comp->ts, 0xfffd, val);
}
