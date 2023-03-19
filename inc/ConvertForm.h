/*
 * Copyright (c) 2016 Evgenii Dobrovidov
 * This file is part of "Unit Converter".
 *
 * "Unit Converter" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Unit Converter" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Unit Converter".  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _CONVERTFORM_H_
#define _CONVERTFORM_H_

#include "UnitsParser.h"
#include <FBase.h>
#include <FUi.h>
#include <FGraphics.h>
#include <FNet.h>

using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;
using namespace Osp::Base::Runtime;
using namespace Osp::Net::Http;

class MainForm;

class ConvertForm: public Form, public IActionEventListener, public IScrollPanelEventListener, public IGroupedItemEventListener, public ITimerEventListener, public IHttpTransactionEventListener {
public:
	ConvertForm(void);
	virtual ~ConvertForm(void);

	bool Initialize(MainForm *pCallback);
	void SetData(UnitsCategory *pItem);
	result RefreshUnitsList(void);
	result FetchCurrencies(void);
	result FetchCurrencies(const String &url);

private:
	static const int ID_UNIT_LONG_NAME = 501;
	static const int ID_UNIT_SHORT_NAME = 502;
	static const int ID_UNIT_VALUE = 503;
	static const int ID_UNIT_SPINNER = 504;

	static const int ACTION_KEYPAD_CONVERT = 100;
	static const int ACTION_KEYPAD_CANCEL = 101;
	static const int ACTION_SOFTKEY_BACK = 102;

	static const int ANIM_INTERVAL = 500/8;

	double CalculateUnit(const Unit &unit) const;
	int GetLineLengthForText(const String &text) const;

	virtual result OnInitializing(void);
	virtual result OnTerminating(void);

	virtual void OnActionPerformed(const Control &source, int actionId);
	virtual void OnOverlayControlCreated(const Control &source) {}
	virtual void OnOverlayControlOpened(const Control &source) { __curText = __pEditField->GetText(); }
	virtual void OnOverlayControlClosed(const Control &source) { __pUnitsList->SetFocus(); }
	virtual void OnOtherControlSelected(const Control &source) {}

	virtual void OnItemStateChanged(const Control &source, int groupIndex, int itemIndex, int itemId, int elementId, ItemStatus status) { }
	virtual void OnItemStateChanged(const Control &source, int groupIndex, int itemIndex, int itemId, ItemStatus status);

	virtual void OnTimerExpired(Timer &timer);

	virtual void OnTransactionReadyToRead(HttpSession &httpSession, HttpTransaction &httpTransaction, int availableBodyLen);
	virtual void OnTransactionAborted(HttpSession &httpSession, HttpTransaction &httpTransaction, result r);
	virtual void OnTransactionReadyToWrite(HttpSession &httpSession, HttpTransaction &httpTransaction, int recommendedChunkSize) { }
	virtual void OnTransactionHeaderCompleted(HttpSession &httpSession, HttpTransaction &httpTransaction, int headerLen, bool rs);
	virtual void OnTransactionCompleted(HttpSession &httpSession, HttpTransaction &httpTransaction);
	virtual void OnTransactionCertVerificationRequiredN(HttpSession &httpSession, HttpTransaction &httpTransaction, String *pCert) { }

	String GetString(const String &id) const;
	Bitmap *GetBitmapN(const String &name) const;
	MessageBoxModalResult ShowMessageBox(const String &title, const String &msg, MessageBoxStyle style) const;

	ScrollPanel *__pScrollPanel;
	EditField *__pEditField;
	GroupedList *__pUnitsList;

	CustomListItemFormat *__pFormat;
	CustomListItemFormat *__pSelectedFormat;
	CustomListItemFormat *__pWideFormat;
	CustomListItemFormat *__pWideSelectedFormat;

	struct {
		Bitmap *__pProgressBitmap1;
		Bitmap *__pProgressBitmap2;
		Bitmap *__pProgressBitmap3;
		Bitmap *__pProgressBitmap4;
		Bitmap *__pProgressBitmap5;
		Bitmap *__pProgressBitmap6;
		Bitmap *__pProgressBitmap7;
		Bitmap *__pProgressBitmap8;
	} __spinnerAnimData;
	Bitmap *__pEmptyBmp;
	Timer *__pSpinTimer;
	int __spinItemIndex;
	int __spinGroupIndex;

	HttpSession *__pDownloadSession;
	ByteBuffer *__pDownloadedData;
	int __fetchItemID;

	MainForm *__pReturnCallback;
	UnitsCategory *__pUnitsCat;
	String __curText;
	double __curValue;
	int __curUnit;
};

#endif
