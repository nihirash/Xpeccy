// sprite scanner

#include <QFile>
#include <QFileDialog>

#include "xcore.h"
#include "xgui.h"
#include "dbg_sprscan.h"

MemViewer::MemViewer(QWidget* p):QDialog(p) {
	ui.setupUi(this);

	vis = 0;

	connect(ui.adrHex, SIGNAL(valueChanged(int)), this, SLOT(fillImage()));
	connect(ui.sbWidth, SIGNAL(valueChanged(int)), this, SLOT(fillImage()));
	connect(ui.sbHeight, SIGNAL(valueChanged(int)), this, SLOT(fillImage()));
	connect(ui.sbPage, SIGNAL(valueChanged(int)), this, SLOT(fillImage()));

	connect(ui.cbInvert, SIGNAL(toggled(bool)), this, SLOT(fillImage()));
	connect(ui.cbGrid, SIGNAL(toggled(bool)), this, SLOT(fillImage()));
	connect(ui.cbScreen, SIGNAL(toggled(bool)), this, SLOT(fillImage()));

	connect(ui.adrHex, SIGNAL(valueChanged(int)), this, SLOT(adrChanged(int)));
	connect(ui.scrollbar, SIGNAL(valueChanged(int)), this, SLOT(memScroll(int)));

	connect(ui.tbSave, SIGNAL(released()), this, SLOT(saveSprite()));
}

void MemViewer::wheelEvent(QWheelEvent* ev) {
	int adr = ui.adrHex->getValue();
	if (ev->delta() < 0) {
		adr += (ui.sbWidth->value() << 3);
	} else {
		adr -= (ui.sbWidth->value() << 3);
	}
	ui.adrHex->setValue(adr & 0xffff);
}

void MemViewer::saveSprite() {
	int adr = ui.adrHex->getValue();
	int siz = ui.sbWidth->value() * ui.sbHeight->value() * 8;
	QByteArray spr;
	if (ui.cbScreen->isChecked()) {
		siz = 0x1800;
	}
	for(int i = 0; i < siz; i++) {
		spr.append(rdMem(adr & 0xffff));
		adr++;
	}
	QString path = QFileDialog::getSaveFileName(this, "Save sprite");
	if (path.isEmpty()) return;
	QFile file(path);
	if (file.open(QFile::WriteOnly)) {
		file.write(spr);
		file.close();
	} else {
		shitHappens("Can't write a file");
	}
}

unsigned char MemViewer::rdMem(int adr) {
	adr &= 0xffff;
	unsigned char res;
	int page = ui.sbPage->value();
	if (adr < 0xc000) {
		res = memRd(mem, adr);
	} else {
		res = mem->ramData[((page << 14) | (adr & 0x3fff)) & mem->ramMask];
	}
	return res;
}

void MemViewer::fillImage() {
	QImage img(256,256, QImage::Format_RGB888);
	img.fill(qRgb(64,64,64));
	int adr = ui.adrHex->getValue();
	int wid, high;
	if (ui.cbScreen->isChecked()) {
		wid = 32;
		high = 192;
	} else {
		wid = ui.sbWidth->value();
		high = ui.sbHeight->value() << 3;
	}
	unsigned char byt;
	unsigned char inv = ui.cbInvert->isChecked() ? 0xff : 0x00;
	int bit;
	int row,col;
	QRgb blk = qRgb(0,0,0);
	QRgb wht = qRgb(255,255,255);
	QRgb lgry = qRgb(160,160,160);
	QRgb dgry = qRgb(32,32,32);
	if (!ui.cbGrid->isChecked()) {
		lgry = wht;
		dgry = blk;
	}
	QRgb clr;
	int alt;
	for (row = 0; row < high; row++) {
		for (col = 0; col < wid; col++) {
			byt = rdMem(adr) ^ inv;
			alt = ((row >> 3) ^ col) & 1;
			adr++;
			for (bit = 0; bit < 8; bit++) {
				clr = (byt & 0x80) ? (alt ? wht : lgry) : (alt ? blk : dgry);
				img.setPixel((col << 3) | bit, row, clr);
				byt <<= 1;
			}
		}
		if (ui.cbScreen->isChecked()) {
			adr += 0xe0;
			if ((row & 0x07) == 0x07) {
				adr -= 0x7e0;
				if ((row & 0x3f) == 0x3f) {
					adr += 0x700;
				}
			}
		}
	}
	QPixmap pxm = QPixmap::fromImage(img.scaled(512,512));
	ui.view->setPixmap(pxm);
	int pg = wid << 3;
	ui.scrollbar->setPageStep(pg);
	ui.scrollbar->setSingleStep(pg);
}

void MemViewer::adrChanged(int adr) {
	ui.scrollbar->setValue(adr);
}

void MemViewer::memScroll(int adr) {
	adr = adr - ((adr - ui.adrHex->getValue()) % ui.sbWidth->value());
	ui.adrHex->setValue(adr);
}
