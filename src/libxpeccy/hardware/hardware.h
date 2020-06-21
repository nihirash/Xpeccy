#ifndef _LIBXPECCY_HARDWARE
#define _LIBXPECCY_HARDWARE

#ifdef __cplusplus
extern "C" {
#endif

#include "../spectrum.h"
#include <assert.h>

// hw type
enum {
	HW_NULL = 0,
	HW_DUMMY,	// nothing-to-do-here
	HW_ZX48,	// ZX48K (Classic)
	HW_PENT,	// Pentagon
	HW_P1024,	// Pentagon1024SL
	HW_SCORP,	// ZS Scorpion
	HW_PLUS2,	// Spectrum 2+
	HW_PLUS3,	// Spectrum 3+
	HW_ATM1,	// ATM 1
	HW_ATM2,	// ATM 2+
	HW_PENTEVO,	// ZXEvo (Baseconf)
	HW_TSLAB,	// PentEvo (TSConf)
	HW_PROFI,	// Profi
	HW_PHOENIX,	// ZXM Phoenix
	HW_MSX,		// MSX 1
	HW_MSX2,	// MSX 2
	HW_GBC,		// Game boy color (gameboy capatible)
	HW_NES,		// Nintendo Entertaiment System (Dendy)
	HW_C64,		// Commodore 64
	HW_BK0010,	// BK0010
	HW_BK0011M,	// BK0011m
	HW_SPCLST,	// PC Specialist
};

// hw group
enum {
	HWG_NULL = 0,
	HWG_ZX,
	HWG_MSX,
	HWG_GB,
	HWG_NES,
	HWG_COMMODORE,
	HWG_BK,
	HWG_SPCLST
};

enum {
	NES_DENDY = 0,
	NES_NTSC,
	NES_PAL
};

// Hardware callbacks

// std callback
typedef void(*cbhwcomp)(Computer*);
// sync: calls after every CPU command
typedef void(*cbHwSnc)(Computer*, int);
// memory read
typedef int(*cbHwMrd)(Computer*, int, int);
// memory write
typedef void(*cbHwMwr)(Computer*, int, int);
// io read; TODO:remove last argument (bdi activity)
typedef int(*cbHwIrd)(Computer*, int, int);
// io write
typedef void(*cbHwIwr)(Computer*, int, int, int);
// key press/release
typedef void(*cbHwKey)(Computer*, keyEntry);
// get volume
typedef sndPair(*cbHwVol)(Computer*, sndVolume*);

struct HardWare {
	int id;			// id
	int grp;
	const char* name;	// name used for conf file
	const char* optName;	// name used for setup window
	int base;		// numbers base (8/10/16)
	int mask;		// mem size bits (see memory.h)
	double xscale;		// pixel ratio (x:y)
	vLayout* lay;		// fixed layout ptr. if NULL, use from config
	cbhwcomp init;		// init (call on setting comp hardware)
	cbhwcomp mapMem;	// map memory
	cbHwIwr out;		// io wr
	cbHwIrd in;		// io rd
	cbHwMrd mrd;		// mem rd
	cbHwMwr mwr;		// mem wr
	cbhwcomp reset;		// reset
	cbHwSnc sync;		// sync time
	cbHwKey keyp;		// key press
	cbHwKey keyr;		// key release
	cbHwVol vol;		// read volume
};

typedef struct {
	int mask;		// if (port & mask == value & mask) port is catched
	int value;
	unsigned dos:2;		// 00:!dos only; 01:dos only; 1x:nevermind
	unsigned rom:2;		// same: b4,7FFD
	unsigned cpm:2;		// cpm mode (for profi)
	int (*in)(Computer*, int);
	void (*out)(Computer*, int, int);
} xPort;

int hwIn(xPort*, Computer*, int, int);
void hwOut(xPort*, Computer*, int, int, int);

typedef struct HardWare HardWare;
extern HardWare hwTab[];

HardWare* findHardware(const char*);
int stdMRd(Computer*, int, int);
void stdMWr(Computer*, int, int);

void zx_sync(Computer*, int);
sndPair zx_vol(Computer*, sndVolume*);
void zx_set_pal(Computer*);

// debug IO

int brkIn(Computer*, int);
void brkOut(Computer*, int, int);

int dummyIn(Computer*, int);
void dummyOut(Computer*, int, int);

// common IO

int zx_dev_wr(Computer*, int, int, int);
int zx_dev_rd(Computer*, int, int*, int);

void xOutFE(Computer*, int, int);
void xOutBFFD(Computer*, int, int);
void xOutFFFD(Computer*, int, int);

int xIn1F(Computer*, int);
int xInFE(Computer*, int);
int xInFFFD(Computer*, int);
int xInFADF(Computer*, int);
int xInFBDF(Computer*, int);
int xInFFDF(Computer*, int);

// dummy

void hw_dum_map(Computer*);
int hw_dum_mrd(Computer*, int, int);
void hw_dum_mwr(Computer*, int, int);
int hw_dum_ird(Computer*, int, int);
void hw_dum_iwr(Computer*, int, int, int);
sndPair hw_dum_vol(Computer*, sndVolume*);

// zx48
void zx_init(Computer*);
void speMapMem(Computer*);
void speOut(Computer*, int, int, int);
int speIn(Computer*, int, int);
void speReset(Computer*);
void zx_keyp(Computer*, keyEntry);
void zx_keyr(Computer*, keyEntry);

// pentagon
void penMapMem(Computer*);
void penOut(Computer*, int, int, int);
int penIn(Computer*, int, int);

// p1024sl
void p1mMapMem(Computer*);
void p1mOut(Computer*, int, int, int);
int p1mIn(Computer*, int, int);

// scorpion
void scoMapMem(Computer*);
void scoOut(Computer*, int, int,int);
int scoIn(Computer*, int, int);
int scoMRd(Computer*, int, int);

// plus 2
void pl2MapMem(Computer*);
void pl2Out(Computer*, int, int, int);
int pl2In(Computer*, int, int);

// plus 3
void pl3Out(Computer*, int, int, int);
int pl3In(Computer*, int, int);
void plusRes(Computer*);

// atm 2
void atm2MapMem(Computer*);
void atm2Out(Computer*, int, int, int);
int atm2In(Computer*, int, int);
void atm2Reset(Computer*);
void atm2_keyp(Computer*, keyEntry);
void atm2_keyr(Computer*, keyEntry);

// Evo baseconf
void evoMapMem(Computer*);
void evoOut(Computer*, int, int, int);
int evoIn(Computer*, int, int);
int evoMRd(Computer*, int, int);
void evoMWr(Computer*, int, int);
void evoReset(Computer*);

// Evo tsconf
void tslMapMem(Computer*);
void tslOut(Computer*, int, int, int);
int tslIn(Computer*, int, int);
int tslMRd(Computer*, int, int);
void tslMWr(Computer*, int, int);
void tslReset(Computer*);

// Profi
void prfMapMem(Computer*);
void prfOut(Computer*, int, int, int);
int prfIn(Computer*, int, int);
void prfReset(Computer*);
int prfMRd(Computer*, int, int);
void prf_keyp(Computer*, keyEntry);
void prf_keyr(Computer*, keyEntry);

// ZXM Phoenix
void phxMapMem(Computer*);
void phxOut(Computer*, int, int, int);
int phxIn(Computer*, int, int);
void phxReset(Computer*);

// msx
void msx_init(Computer*);
void msxMapMem(Computer*);
void msxOut(Computer*, int, int, int);
int msxIn(Computer*, int, int);
void msxReset(Computer*);
int msxMRd(Computer*, int, int);
void msxMWr(Computer*, int, int);
void msx_sync(Computer*, int);
void msx_keyp(Computer*, keyEntry);
void msx_keyr(Computer*, keyEntry);
sndPair msx_vol(Computer*, sndVolume*);

// msx2
void msx2_init(Computer*);
void msx2mapper(Computer*);
void msx2Out(Computer*, int, int, int);
int msx2In(Computer*, int, int);
void msx2Reset(Computer*);
int msx2mrd(Computer*, int, int);
void msx2mwr(Computer*, int, int);

// gameboy
void gbc_init(Computer*);
void gbMaper(Computer*);
void gbReset(Computer*);
int gbMemRd(Computer*, int, int);
void gbMemWr(Computer*, int, int);
void gbcSync(Computer*, int);
void gbc_keyp(Computer*, keyEntry);
void gbc_keyr(Computer*, keyEntry);
sndPair gbc_vol(Computer*, sndVolume*);

// nes
void nes_init(Computer*);
void nesMaper(Computer*);
void nesReset(Computer*);
int nesMemRd(Computer*, int, int);
void nesMemWr(Computer*, int, int);
void nesSync(Computer*, int);
void nes_keyp(Computer*, keyEntry);
void nes_keyr(Computer*, keyEntry);
sndPair nes_vol(Computer*, sndVolume*);
int nes_apu_ext_rd(int, void*);

// c64
void c64_init(Computer*);
void c64_maper(Computer*);
void c64_reset(Computer*);
void c64_mwr(Computer*, int, int);
int c64_mrd(Computer*, int, int);
sndPair c64_vol(Computer*, sndVolume*);
void c64_sync(Computer*, int);
void c64_keyp(Computer*, keyEntry);
void c64_keyr(Computer*, keyEntry);
// int c64_vic_mrd(int, void*);

// bk0010
void bk_init(Computer*);
void bk_mem_map(Computer*);
void bk_reset(Computer*);
void bk_mwr(Computer*, int, int);
int bk_mrd(Computer* comp, int, int);
void bk_iowr(Computer*, int, int, int);
void bk_sync(Computer*, int);
void bk_keyp(Computer*, keyEntry);
void bk_keyr(Computer*, keyEntry);
sndPair bk_vol(Computer*, sndVolume*);

// pc specialist
void spc_init(Computer*);
void spc_mem_map(Computer*);
void spc_reset(Computer*);
int spc_mrd(Computer*, int, int);
int spc_vid_rd(int, void*);
void spc_mwr(Computer*, int, int);
void spc_sync(Computer*, int);
void spc_keyp(Computer*, keyEntry);
void spc_keyr(Computer*, keyEntry);
sndPair spc_vol(Computer*, sndVolume*);

#ifdef __cplusplus
}
#endif

#endif
