#include "hardware.h"

Z80EX_BYTE xInFE(ZXComp* comp, Z80EX_WORD port) {
	Z80EX_BYTE res = keyInput(comp->keyb, (port & 0xff00) >> 8);
	res |= (comp->tape->levPlay ? 0x40 : 0x00);
	return res;
}

Z80EX_BYTE xInFFFD(ZXComp* comp, Z80EX_WORD port) {
	return tsIn(comp->ts, 0xfffd);
}

Z80EX_BYTE xInFADF(ZXComp* comp, Z80EX_WORD port) {
	comp->mouse->used = 1;
	return comp->mouse->enable ? comp->mouse->buttons : 0xff;
}

Z80EX_BYTE xInFBDF(ZXComp* comp, Z80EX_WORD port) {
	comp->mouse->used = 1;
	return comp->mouse->enable ? comp->mouse->xpos : 0xff;
}

Z80EX_BYTE xInFFDF(ZXComp* comp, Z80EX_WORD port) {
	comp->mouse->used = 1;
	return comp->mouse->enable ? comp->mouse->ypos : 0xff;
}

// out

void xOutBFFD(ZXComp* comp, Z80EX_WORD port, Z80EX_BYTE val) {
	tsOut(comp->ts, 0xbffd, val);
}

void xOutFFFD(ZXComp* comp, Z80EX_WORD port, Z80EX_BYTE val) {
	tsOut(comp->ts, 0xfffd, val);
}