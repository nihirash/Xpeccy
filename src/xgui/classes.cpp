#include "xgui.h"
#include "../xcore/xcore.h"

#include <QDebug>
#include <QPalette>

QString gethexword(int);
QString gethexbyte(uchar);

xHexSpin::xHexSpin(QWidget* p):QLineEdit(p) {
	setMinimumWidth(60);
	setAutoFillBackground(true);
	vldtr.setRegExp(QRegExp(""));
	min = 0x0000;
	max = 0xffff;
	value = 0x0000;
	hsflag = XHS_DEC;
	len = 6;
	setValidator(&vldtr);
	setBase(16);
	setText("000000");
	connect(this, SIGNAL(textChanged(QString)), SLOT(onTextChange(QString)));
}

int xHexSpin::getValue() {
	return value;
}

void xHexSpin::setBase(int b) {
	int mx;
	int tmp = value;
	QString rxp;
	QString digxp;
	switch(b) {
		case 8:
			base = 8;
			digxp = "[0-7]";
			setStyleSheet("border:1px solid red;");
			break;
		case 10:
			base = 10;
			digxp = "[0-9]";
			setStyleSheet("border:1px solid black;");
			break;
		default:
			base = 16;
			digxp = "[A-Fa-f0-9]";
			setStyleSheet("border:1px solid green;");
			break;
	}
	if (conf.prof.cur) {
		if (base == conf.prof.cur->zx->hw->base) {
			setStyleSheet("border:1px solid white;");
		}
	}
	len = 1;
	rxp = digxp;
	mx = base;
	while (mx <= max) {
		rxp.append(digxp);
		mx *= base;
		len++;
	}

	setMaxLength(len);
	setInputMask(QString(len, 'H'));	// to enter overwrite cursor mode. TODO:is there some legit method?
	vldtr.setRegExp(QRegExp(rxp));		// set available chars
	hsflag |= XHS_UPD;			// update even if value doesn't changed
	setValue(tmp);
}

void xHexSpin::setXFlag(int xf) {
	hsflag = xf;
}

int minMaxCorrect(int val, int min, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

void xHexSpin::setMin(int v) {
	min = v;
	setBase(base);
	if (value < min) setValue(min);
}

void xHexSpin::setMax(int v) {
	max = v;
	setBase(base);
	if (value > max) setValue(max);
}

void xHexSpin::setValue(int nval) {
	nval = minMaxCorrect(nval, min, max);
	QPalette pal;
	if ((value == nval) && !(hsflag & XHS_UPD)) {
		pal.setColor(QPalette::Base, conf.pal["dbg.input.bg"].isValid() ? conf.pal["dbg.input.bg"] : pal.base().color());
		pal.setColor(QPalette::Text, conf.pal["dbg.input.txt"].isValid() ? conf.pal["dbg.input.txt"] : pal.text().color());
	} else {
		value = nval;
		if (hsflag & XHS_BGR) {
			pal.setColor(QPalette::Base, conf.pal["dbg.changed.bg"].isValid() ? conf.pal["dbg.changed.bg"] : pal.toolTipBase().color());
			pal.setColor(QPalette::Text, conf.pal["dbg.changed.txt"].isValid() ? conf.pal["dbg.changed.txt"] : pal.toolTipText().color());
		} else {
			pal.setColor(QPalette::Base, conf.pal["dbg.input.bg"].isValid() ? conf.pal["dbg.input.bg"] : pal.base().color());
			pal.setColor(QPalette::Text, conf.pal["dbg.input.txt"].isValid() ? conf.pal["dbg.input.txt"] : pal.text().color());
		}
		emit valueChanged(nval);
		onChange(value);
	}
	setPalette(pal);
}

void xHexSpin::onChange(int val) {
	int pos = cursorPosition();
	QString res = QString::number(val, base).toUpper();
	res = res.rightJustified(len, '0');
	if ((text() != res) || (hsflag & XHS_UPD)) {
		hsflag &= ~XHS_UPD;
		setText(res);
		setCursorPosition(pos);
	}
}

void xHexSpin::onTextChange(QString txt) {
	if (txt.size() < len) {
		txt = txt.leftJustified(len, '0');
	}
	int nval = txt.toInt(NULL, base);
	int xval = minMaxCorrect(nval, min, max);
	if (value != xval)
		setValue(xval);
	else
		onChange(value);
}

void xHexSpin::keyPressEvent(QKeyEvent* ev) {
	switch(ev->key()) {
		case Qt::Key_Up:
			setValue(minMaxCorrect(value + 1, min, max));
			break;
		case Qt::Key_Down:
			setValue(minMaxCorrect(value - 1, min, max));
			break;
		case Qt::Key_PageUp:
			setValue(minMaxCorrect(value + 0x100, min, max));
			break;
		case Qt::Key_PageDown:
			setValue(minMaxCorrect(value - 0x100, min, max));
			break;
		case Qt::Key_X:
			if (hsflag & XHS_DEC) {
				if (base == 8) {
					setBase(10);
				} else if (base == 10) {
					setBase(16);
				} else {
					setBase(8);
				}
			}
			break;
		default:
			QLineEdit::keyPressEvent(ev);
			break;
	}
}

void xHexSpin::wheelEvent(QWheelEvent* ev) {
	if (ev->delta() < 0) {
		setValue(minMaxCorrect(value + 1, min, max));
	} else if (ev->delta() > 0) {
		setValue(minMaxCorrect(value - 1, min, max));
	}
	ev->accept();
}

// xLabel

xLabel::xLabel(QWidget* p):QLabel(p) {}

void xLabel::mousePressEvent(QMouseEvent* ev) {
	emit clicked(ev);
}
