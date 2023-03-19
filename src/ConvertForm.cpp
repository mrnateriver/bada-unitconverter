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
#include "ConvertForm.h"
#include "MainForm.h"
#include <FApp.h>
#include <FMedia.h>
#include <FLocales.h>

using namespace Osp::App;
using namespace Osp::Media;
using namespace Osp::Locales;
using namespace Osp::Base::Utility;

ConvertForm::ConvertForm(void) {
	__pScrollPanel = null;
	__pEditField = null;
	__pUnitsList = null;

	__pFormat = null;
	__pSelectedFormat = null;
	__pWideFormat = null;
	__pWideSelectedFormat = null;

	unsigned char *ptr = reinterpret_cast<unsigned char *>(&__spinnerAnimData);
	for (unsigned int i = 0; i < sizeof(__spinnerAnimData); i++) {
		*(ptr + i) = 0;
	}
	__pEmptyBmp = null;
	__pSpinTimer = null;
	__spinItemIndex = -1;
	__spinGroupIndex = -1;

	__pDownloadSession = null;
	__pDownloadedData = null;
	__fetchItemID = -1;

	__pReturnCallback = null;
	__pUnitsCat = null;
	__curText = L"";
	__curUnit = 0;
	__curValue = 0.0;
}

ConvertForm::~ConvertForm(void) {
	if (__pDownloadSession) {
		__pDownloadSession->CloseAllTransactions();
		delete __pDownloadSession;
	}
	if (__pDownloadedData) {
		delete __pDownloadedData;
	}
	if (__pFormat) {
		delete __pFormat;
	}
	if (__pSelectedFormat) {
		delete __pSelectedFormat;
	}
	if (__pSpinTimer) {
		__pSpinTimer->Cancel();
		delete __pSpinTimer;
	}
}

bool ConvertForm::Initialize(MainForm *pCallback) {
	Construct(FORM_STYLE_INDICATOR | FORM_STYLE_TITLE | FORM_STYLE_SOFTKEY_1);
	SetBackgroundColor(Color(0xdfe4e0, false));
	SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_SOFTKEY_BACK"));

	ScrollPanel *pPanel = new ScrollPanel;
	pPanel->Construct(Rectangle(0,0,480,712));

	EditField *pField = new EditField;
	pField->Construct(Rectangle(0,0,478,69), EDIT_FIELD_STYLE_NORMAL_SMALL, INPUT_STYLE_OVERLAY, false, 1000, GROUP_STYLE_NONE);
	pField->SetInputModeCategory(EDIT_INPUTMODE_SYMBOL | EDIT_INPUTMODE_NUMERIC, true);
	pField->SetGuideText(GetString(L"IDS_CONVERTFORM_GUIDE_TEXT"));
	pField->SetOverlayKeypadCommandButton(COMMAND_BUTTON_POSITION_LEFT, GetString(L"IDS_CONVERTFORM_KEYPAD_LEFT"), ACTION_KEYPAD_CONVERT);
	pField->SetOverlayKeypadCommandButton(COMMAND_BUTTON_POSITION_RIGHT, GetString(L"IDS_CONVERTFORM_KEYPAD_RIGHT"), ACTION_KEYPAD_CANCEL);
	pField->AddActionEventListener(*this);
	pField->AddScrollPanelEventListener(*this);

	Label *pLabel = new Label;
	pLabel->Construct(Rectangle(0,69,480,1), L"");
	Bitmap *pSep = GetBitmapN(L"list_separator.png");
	pLabel->SetBackgroundBitmap(*pSep);
	if (pSep) {
		delete pSep;
	}

	GroupedList *pList = new GroupedList;
	pList->Construct(Rectangle(0,70,480,642), CUSTOM_LIST_STYLE_NORMAL, true, true);
	pList->SetTextOfEmptyList(L" ");
	pList->AddGroupedItemEventListener(*this);

	CustomListItemFormat *pFormat = new CustomListItemFormat;
	pFormat->Construct();
	pFormat->AddElement(ID_UNIT_LONG_NAME, Rectangle(3,3,281,77), 35, Color(0x5e5e58, false), Color::COLOR_WHITE);
	pFormat->AddElement(ID_UNIT_SHORT_NAME, Rectangle(3,22,281,35), 35, Color(0x5e5e58, false), Color::COLOR_WHITE);
	pFormat->AddElement(ID_UNIT_VALUE, Rectangle(293,22,187,35), 35, Color(0x2c2c2c, false), Color::COLOR_WHITE);
	pFormat->AddElement(ID_UNIT_SPINNER, Rectangle(412,12,56,56));

	CustomListItemFormat *pSelFormat = new CustomListItemFormat;
	pSelFormat->Construct();
	pSelFormat->AddElement(ID_UNIT_LONG_NAME, Rectangle(10,3,281,77), 35, Color::COLOR_BLACK, Color::COLOR_WHITE);
	pSelFormat->AddElement(ID_UNIT_SHORT_NAME, Rectangle(12,22,281,35), 35, Color::COLOR_BLACK, Color::COLOR_WHITE);
	pSelFormat->AddElement(ID_UNIT_VALUE, Rectangle(293,22,187,35), 35, Color(0x2c2c2c, false), Color::COLOR_WHITE);

	CustomListItemFormat *pWideFormat = new CustomListItemFormat;
	pWideFormat->Construct();
	pWideFormat->AddElement(ID_UNIT_LONG_NAME, Rectangle(3,8,281,77), 35, Color(0x5e5e58, false), Color::COLOR_WHITE);
	pWideFormat->AddElement(ID_UNIT_SHORT_NAME, Rectangle(3,27,281,35), 35, Color(0x5e5e58, false), Color::COLOR_WHITE);
	pWideFormat->AddElement(ID_UNIT_VALUE, Rectangle(293,27,187,35), 35, Color(0x2c2c2c, false), Color::COLOR_WHITE);
	pWideFormat->AddElement(ID_UNIT_SPINNER, Rectangle(407,17,56,56));

	CustomListItemFormat *pWideSelFormat = new CustomListItemFormat;
	pWideSelFormat->Construct();
	pWideSelFormat->AddElement(ID_UNIT_LONG_NAME, Rectangle(10,8,281,77), 35, Color::COLOR_BLACK, Color::COLOR_WHITE);
	pWideSelFormat->AddElement(ID_UNIT_SHORT_NAME, Rectangle(12,27,281,35), 35, Color::COLOR_BLACK, Color::COLOR_WHITE);
	pWideSelFormat->AddElement(ID_UNIT_VALUE, Rectangle(293,27,187,35), 35, Color(0x2c2c2c, false), Color::COLOR_WHITE);

	pPanel->AddControl(*pList);
	pPanel->AddControl(*pLabel);
	pPanel->AddControl(*pField);
	AddControl(*pPanel);

	__pScrollPanel = pPanel;
	__pEditField = pField;
	__pUnitsList = pList;

	__pFormat = pFormat;
	__pSelectedFormat = pSelFormat;
	__pWideFormat = pWideFormat;
	__pWideSelectedFormat = pWideSelFormat;

	SetSoftkeyActionId(SOFTKEY_1, ACTION_SOFTKEY_BACK);
	AddSoftkeyActionListener(SOFTKEY_1, *this);

	__pReturnCallback = pCallback;

	__pSpinTimer = new Timer;
	__pSpinTimer->Construct(*this);

	__spinnerAnimData.__pProgressBitmap1 = GetBitmapN("progressing00.png");
	__spinnerAnimData.__pProgressBitmap2 = GetBitmapN("progressing01.png");
	__spinnerAnimData.__pProgressBitmap3 = GetBitmapN("progressing02.png");
	__spinnerAnimData.__pProgressBitmap4 = GetBitmapN("progressing03.png");
	__spinnerAnimData.__pProgressBitmap5 = GetBitmapN("progressing04.png");
	__spinnerAnimData.__pProgressBitmap6 = GetBitmapN("progressing05.png");
	__spinnerAnimData.__pProgressBitmap7 = GetBitmapN("progressing06.png");
	__spinnerAnimData.__pProgressBitmap8 = GetBitmapN("progressing07.png");

	Bitmap *bmps = reinterpret_cast<Bitmap *>(&__spinnerAnimData);
	for (int i = 0; i < 8; i++) {
		if (!(bmps + i)) {
			return E_FAILURE;
		}
	}

	__pEmptyBmp = GetBitmapN(L"empty.png");
	if (!__pEmptyBmp) {
		return E_FAILURE;
	}

	return true;
}

void ConvertForm::SetData(UnitsCategory *pItem) {
	if (pItem) {
		SetTitleText(pItem->GetCategoryName(), ALIGNMENT_LEFT);

		Bitmap *pBmp = GetBitmapN(pItem->GetCategorySmallIconPath());
		SetTitleIcon(pBmp);
		delete pBmp;
	} else {
		SetTitleText(L"", ALIGNMENT_LEFT);
	}
	__pUnitsCat = pItem;
	if (__pUnitsCat->GetSpecialConversionID() != 6) {
		__curUnit = pItem->GetReferenceUnitID();
	} else {
		__curUnit = -1;
	}

	__curValue = 0.0;
	__curText = L"";
	__pEditField->Clear();

	__pUnitsList->ScrollToTop();
}

result ConvertForm::RefreshUnitsList(void) {
	__pUnitsList->RemoveAllItems();
	__pUnitsList->RemoveAllGroups();

	ArrayListT<String> *pSys = __pUnitsCat->GetSystemsListN();
	IEnumeratorT<String> *pSysEnum = pSys->GetEnumeratorN();
	if (pSysEnum) {
		while (!IsFailed(pSysEnum->MoveNext())) {
			String system; pSysEnum->GetCurrent(system);
			__pUnitsList->AddGroup(system, null);
		}
	} else {
		return E_FAILURE;
	}
	delete pSysEnum;

	String local_cur = L"";

	LocaleManager mgr;
	mgr.Construct();
	Currency cur;
	if (!IsFailed(cur.Construct(mgr.GetSystemLocale()))) {
		local_cur = cur.GetCurrencyCode();
	}

	ArrayListT<Unit> *pList = __pUnitsCat->GetUnitsListN();
	IEnumeratorT<Unit> *pEnum = pList->GetEnumeratorN();
	if (pEnum) {
		int itemId = 0;
		if (__curUnit < 0) {
			while (!IsFailed(pEnum->MoveNext())) {
				Unit unit; pEnum->GetCurrent(unit);
				if (unit.ccode.Equals(local_cur, false)) {
					__curUnit = itemId;
					break;
				}
				itemId++;
			}
			pEnum->Reset();
			itemId = 0;
		}

		Font font;
		font.Construct(FONT_STYLE_PLAIN, 35);

		EnrichedText ent;
		ent.Construct(Dimension(460, 50));
		ent.SetTextWrapStyle(TEXT_WRAP_NONE);

		int wid, hei, actLen;

		Bitmap *pBg = GetBitmapN(L"units_list_selected_item.png");
		Bitmap *pBg80 = GetBitmapN(L"units_list_selected_item_80.png");
		while (!IsFailed(pEnum->MoveNext())) {
			Unit unit; pEnum->GetCurrent(unit);

			TextElement elem;
			elem.Construct(unit.name);
			elem.SetFont(font);
			ent.Add(elem);
			ent.GetTextExtent(0, unit.name.GetLength(), wid, hei, actLen);
			ent.RemoveAllTextElements(false);

			CustomListItem *pItem = new CustomListItem;
			int id = ID_UNIT_SHORT_NAME;
			if (wid > 279) {
				id = ID_UNIT_LONG_NAME;
				pItem->Construct(90);
				if (itemId == __curUnit) {
					pItem->SetNormalItemBackgroundBitmap(*pBg);
					pItem->SetItemFormat(*__pWideSelectedFormat);
				} else {
					pItem->SetItemFormat(*__pWideFormat);
				}
			} else {
				pItem->Construct(80);
				if (itemId == __curUnit) {
					pItem->SetNormalItemBackgroundBitmap(*pBg80);
					pItem->SetItemFormat(*__pSelectedFormat);
				} else {
					pItem->SetItemFormat(*__pFormat);
				}
			}
			pItem->SetElement(id, unit.name);

			if (__pUnitsCat->GetSpecialConversionID() != 6 || itemId == __curUnit) {
				double val = CalculateUnit(unit);
				String strVal = L"";
				if (Double::IsInfinity(val)) {
					strVal.Append(L"\u221E");
				} else {
					strVal.Append(Double::ToString(val));
				}

				TextElement elem_val;
				elem_val.Construct(strVal);
				elem_val.SetFont(font);
				ent.Add(elem_val);
				ent.GetTextExtent(0, strVal.GetLength(), wid, hei, actLen);
				ent.RemoveAllTextElements(false);

				int spaces = (int)((double)(187 - wid) / 10);
				for(int i = 0; i < spaces; i++) {
					strVal.Insert(' ', 0);
				}
				pItem->SetElement(ID_UNIT_VALUE, strVal);
			}

			if (__pUnitsCat->GetSpecialConversionID() == 6 && unit.ccode.Equals(local_cur, false)) {
				__pUnitsList->AddItem(0, *pItem, itemId++);
			} else {
				__pUnitsList->AddItem(unit.system_id, *pItem, itemId++);
			}
		}
		if (pBg) {
			delete pBg;
		}
	}
	else {
		return E_FAILURE;
	}
	delete pEnum;

	Draw();
	Show();

	return E_SUCCESS;
}

result ConvertForm::FetchCurrencies(void) {
	if (__fetchItemID == __curUnit) {
		__spinItemIndex++;
		if (__spinItemIndex >= __pUnitsList->GetItemCountAt(__spinGroupIndex)) {
			__spinItemIndex = 0;
			__spinGroupIndex++;
			if (__spinGroupIndex >= __pUnitsList->GetGroupCount()) {
				__spinItemIndex = -1;
				__spinGroupIndex = -1;
				__fetchItemID = -1;

				SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_SOFTKEY_BACK"));
				RequestRedraw(true);
			} else {
				__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
			}
		} else {
			__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
		}
	}
	if (__fetchItemID < 0 || __pUnitsCat->GetSpecialConversionID() != 6) {
		return E_SUCCESS;
	}

	result res = E_SUCCESS;
	while(true) {
		Unit curUnit = __pUnitsCat->GetUnitByListID(__curUnit);
		Unit unit = __pUnitsCat->GetUnitByListID(__fetchItemID);

		String fetchUrl = L"http://www.google.com/ig/calculator?q=1";
		fetchUrl.Append(curUnit.ccode);
		fetchUrl.Append(L"=?");
		fetchUrl.Append(unit.ccode);

		if (__pDownloadSession) {
			__pDownloadSession->CloseAllTransactions();
			delete __pDownloadSession;
			__pDownloadSession = null;
		}
		if (__pDownloadedData) {
			delete __pDownloadedData;
			__pDownloadedData = null;
		}

		__pDownloadSession = new HttpSession();
		res = __pDownloadSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, fetchUrl, null);
		if (IsFailed(res)) {
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to construct http session", GetErrorMessage(res));
			break;
		}

		HttpTransaction *pTransaction = __pDownloadSession->OpenTransactionN();
		if (!pTransaction) {
			res = GetLastResult();
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to construct http session", GetErrorMessage(res));
			break;
		}
		pTransaction->AddHttpTransactionListener(*this);

		HttpRequest *pRequest = const_cast<HttpRequest*>(pTransaction->GetRequest());
		pRequest->SetUri(fetchUrl);
		pRequest->GetHeader()->AddField("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)");
		pRequest->SetMethod(NET_HTTP_METHOD_GET);

		res = pTransaction->Submit();
		if (IsFailed(res)) {
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to submit http transaction", GetErrorMessage(res));

			delete pTransaction;
			break;
		}

		//start anim timer
		__pSpinTimer->Start(ANIM_INTERVAL);

		return E_SUCCESS;
	}

	ShowMessageBox(GetString(L"IDS_DOWNLOAD_CURRENCIES_ERROR_TITLE"), GetString(L"IDS_DOWNLOAD_CURRENCIES_ERROR_MSG"), MSGBOX_STYLE_OK);
	return res;
}

result ConvertForm::FetchCurrencies(const String &url) {
	if (url.IsEmpty()) {
		return E_INVALID_ARG;
	}

	result res = E_SUCCESS;
	while(true) {
		if (__pDownloadSession) {
			__pDownloadSession->CloseAllTransactions();
			delete __pDownloadSession;
			__pDownloadSession = null;
		}
		if (__pDownloadedData) {
			delete __pDownloadedData;
			__pDownloadedData = null;
		}

		__pDownloadSession = new HttpSession();
		res = __pDownloadSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, url, null);
		if (IsFailed(res)) {
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to construct http session", GetErrorMessage(res));
			break;
		}

		HttpTransaction *pTransaction = __pDownloadSession->OpenTransactionN();
		if (!pTransaction) {
			res = GetLastResult();
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to construct http session", GetErrorMessage(res));
			break;
		}
		pTransaction->AddHttpTransactionListener(*this);

		HttpRequest *pRequest = const_cast<HttpRequest*>(pTransaction->GetRequest());
		pRequest->SetUri(url);
		pRequest->GetHeader()->AddField("User-Agent", "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)");
		pRequest->SetMethod(NET_HTTP_METHOD_GET);

		res = pTransaction->Submit();
		if (IsFailed(res)) {
			AppLogDebug("ConvertForm::FetchCurrencies: [%s]: failed to submit http transaction", GetErrorMessage(res));

			delete pTransaction;
			break;
		}
		return E_SUCCESS;
	}

	ShowMessageBox(GetString(L"IDS_DOWNLOAD_CURRENCIES_ERROR_TITLE"), GetString(L"IDS_DOWNLOAD_CURRENCIES_ERROR_MSG"), MSGBOX_STYLE_OK);
	return res;
}

double ConvertForm::CalculateUnit(const Unit &unit) const {
	Unit cur = __pUnitsCat->GetUnitByListID(__curUnit);
	if (unit.sp_id < 0 && cur.sp_id < 0) {
		double ref = __curValue / cur.multiplier;
		return ref * unit.multiplier;
	} else {
		return __pUnitsCat->CalculateNonLinearUnit(cur, __curValue, unit);
	}
}

int ConvertForm::GetLineLengthForText(const String &text) const {
	Font font;
	font.Construct(FONT_STYLE_PLAIN, 35);

	TextElement elem;
	elem.Construct(text);
	elem.SetFont(font);

	EnrichedText ent;
	ent.Construct(Dimension(460, 50));
	ent.SetTextWrapStyle(TEXT_WRAP_NONE);
	ent.Add(elem);

	int wid, hei, actLen;
	ent.GetTextExtent(0, text.GetLength(), wid, hei, actLen);

	return wid;
}

result ConvertForm::OnInitializing(void) {
	//__curUnit = __pUnitsCat->GetReferenceUnitID();
	return E_SUCCESS;//RefreshUnitsList();
}

result ConvertForm::OnTerminating(void) {
	return E_SUCCESS;
}

void ConvertForm::OnActionPerformed(const Control &source, int actionId) {
	switch(actionId) {
	case ACTION_KEYPAD_CANCEL: {
		__pEditField->SetText(__curText);

		__pScrollPanel->CloseOverlayWindow();
		__pEditField->Draw();
		__pEditField->Show();
		break;
	}
	case ACTION_KEYPAD_CONVERT: {
		if (__pUnitsCat->GetSpecialConversionID() == 6 && __fetchItemID >= 0) {
			if (__pDownloadSession) {
				__pDownloadSession->CloseAllTransactions();
				delete __pDownloadSession;
				__pDownloadSession = null;
			}
			if (__pDownloadedData) {
				delete __pDownloadedData;
				__pDownloadedData = null;
			}
			__pSpinTimer->Cancel();

			CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
			if (item) {
				item->SetElement(ID_UNIT_SPINNER, *__pEmptyBmp, null);
			}
			__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

			__spinItemIndex = -1;
			__spinGroupIndex = -1;
			__fetchItemID = -1;
		}

		__curValue = 0.0;
		if (__pEditField->GetTextLength() > 0) {
			String text = __pEditField->GetText();
			bool got_sep = false;
			for(int i = 0; i < text.GetLength(); i++) {
				mchar ch; text.GetCharAt(i, ch);
				if (__pUnitsCat->GetSpecialConversionID() == 0 && (ch == L'-' && i == 0)) {
					continue;
				}
				if (!Character::IsDigit(ch)) {
					if (got_sep || (ch != L',' && ch != L'.')) {
						text.SetCharAt(L'*', i);
					} else {
						got_sep = true;
					}
				}
			}
			text.Replace(L"*", L"", 0);
			text.Replace(L',', L'.');
			__pEditField->SetText(text);

			double val;
			if (!IsFailed(Double::Parse(text, val))) {
				__curValue = val;
			}
		}
		__pScrollPanel->CloseOverlayWindow();
		__pEditField->Draw();
		__pEditField->Show();

		RefreshUnitsList();
		if (__pUnitsCat->GetSpecialConversionID() == 6 && (__curValue > 0.0 && !__pEditField->GetText().IsEmpty())) {
			//start spinning from the very first item
			__spinGroupIndex = 0;
			__spinItemIndex = 0;
			//set item ID for fetching
			__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
			//initiate download
			FetchCurrencies();
			//set softkey
			SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_CONVERTFORM_KEYPAD_RIGHT"));
			RequestRedraw(true);
		}
		break;
	}
	case ACTION_SOFTKEY_BACK: {
		if (__pUnitsCat->GetSpecialConversionID() == 6 && __fetchItemID >= 0) {
			if (__pDownloadSession) {
				__pDownloadSession->CloseAllTransactions();
				delete __pDownloadSession;
				__pDownloadSession = null;
			}
			if (__pDownloadedData) {
				delete __pDownloadedData;
				__pDownloadedData = null;
			}
			__pSpinTimer->Cancel();

			CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
			if (item) {
				item->SetElement(ID_UNIT_SPINNER, *__pEmptyBmp, null);
			}
			__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

			__spinItemIndex = -1;
			__spinGroupIndex = -1;
			__fetchItemID = -1;

			SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_SOFTKEY_BACK"));
			RequestRedraw(true);
		} else {
			__pUnitsList->RemoveAllItems();
			__pUnitsList->RemoveAllGroups();

			__pReturnCallback->DialogCallback();
		}
	}
	default: break;
	}
}

void ConvertForm::OnItemStateChanged(const Control &source, int groupIndex, int itemIndex, int itemId, ItemStatus status) {
	if (itemId >= 0 && itemId < __pUnitsCat->GetUnitsListLength()) {
		__curUnit = itemId;
		RefreshUnitsList();
		if (__pUnitsCat->GetSpecialConversionID() == 6) {
			if (__fetchItemID >= 0) {
				if (__pDownloadSession) {
					__pDownloadSession->CloseAllTransactions();
					delete __pDownloadSession;
					__pDownloadSession = null;
				}
				if (__pDownloadedData) {
					delete __pDownloadedData;
					__pDownloadedData = null;
				}
				__pSpinTimer->Cancel();

				CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
				if (item) {
					item->SetElement(ID_UNIT_SPINNER, *__pEmptyBmp, null);
				}
				__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

				__spinItemIndex = -1;
				__spinGroupIndex = -1;
				__fetchItemID = -1;
			}
			if (__curValue > 0.0 && !__pEditField->GetText().IsEmpty()) {
				//start spinning from the very first item
				__spinGroupIndex = 0;
				__spinItemIndex = 0;
				//set item ID for fetching
				__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
				//initiate download
				FetchCurrencies();
				//set softkey
				SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_CONVERTFORM_KEYPAD_RIGHT"));
				RequestRedraw(true);
			}
		}
	}
}

String ConvertForm::GetString(const String &id) const {
	String str = id;
	result res = Application::GetInstance()->GetAppResource()->GetString(id, str);
	if (IsFailed(res)) {
		AppLogException("Could not retrieve string by id [%S]", id.GetPointer());
		SetLastResult(res);
	}
	return str;
}

Bitmap *ConvertForm::GetBitmapN(const String &name) const {
	Image *pImage = new Image();
	result res = pImage->Construct();
	if (IsFailed(res)) {
		AppLogException("Failed to construct image decoder, error [%s]", GetErrorMessage(res));
		SetLastResult(res);
		return null;
	}

	String fullname = String(L"/Res/480x800/");
	fullname.Append(name);

	Bitmap *pBitmap = null;
	if(fullname.EndsWith(L"jpg")) {
		pBitmap = pImage->DecodeN(fullname, BITMAP_PIXEL_FORMAT_RGB565);
	} else if(fullname.EndsWith(L"bmp")) {
		pBitmap = pImage->DecodeN(fullname, BITMAP_PIXEL_FORMAT_RGB565);
	} else if(fullname.EndsWith(L"png")) {
		pBitmap = pImage->DecodeN(fullname, BITMAP_PIXEL_FORMAT_ARGB8888);
	} else if (fullname.EndsWith(L"gif")) {
		pBitmap = pImage->DecodeN(fullname, BITMAP_PIXEL_FORMAT_RGB565);
	}
	delete pImage;

	if (IsFailed(GetLastResult())) {
		AppLogException("Failed to decode image, error [%s]", GetErrorMessage(GetLastResult()));
		return null;
	}
	return pBitmap;
}

MessageBoxModalResult ConvertForm::ShowMessageBox(const String &title, const String &msg, MessageBoxStyle style) const {
	MessageBox *pBox = new MessageBox;
	int mres = 2;
	result res = pBox->Construct(title, msg, style);
	if (!IsFailed(res)) {
		pBox->ShowAndWait(mres);
	}
	SetLastResult(res);
	delete pBox;

	return (MessageBoxModalResult)mres;
}

void ConvertForm::OnTransactionReadyToRead(HttpSession &httpSession, HttpTransaction &httpTransaction, int availableBodyLen) {
	ByteBuffer *pData = null;

	HttpResponse* pHttpResponse = httpTransaction.GetResponse();
	if(!pHttpResponse) {
		return;
	}

	HttpHeader *pHeader = pHttpResponse->GetHeader();
	if (!pHeader) {
		return;
	}

	if (pHttpResponse->GetStatusCode() == NET_HTTP_STATUS_OK) {
		if (!__pDownloadedData) {
			int downloadLen = 0;
			String content_length = L"";
			IList *pFields = pHeader->GetFieldNamesN();
			IEnumerator *pEnum = pFields->GetEnumeratorN();
			while (!IsFailed(pEnum->MoveNext())) {
				String *field = static_cast<String *>(pEnum->GetCurrent());
				if (field->Equals(L"Content-Length", false)) {
					IEnumerator *pVals = pHeader->GetFieldValuesN(*field);
					if (!IsFailed(pVals->MoveNext())) {
						content_length = *static_cast<String *>(pVals->GetCurrent());
					}
					delete pVals;
					break;
				}
			}
			delete pEnum;
			delete pFields;

			result res = E_SUCCESS;
			__pDownloadedData = new ByteBuffer;
			if (content_length.IsEmpty()) {
				res = __pDownloadedData->Construct(availableBodyLen);
			} else {
				res = Integer::Parse(content_length, downloadLen);
				if (!IsFailed(res)) {
					res = __pDownloadedData->Construct(downloadLen);
				}
			}
			if (IsFailed(res)) {
				delete __pDownloadedData;
				__pDownloadedData = null;
			}
		}
		pData = pHttpResponse->ReadBodyN();
		if (!pData) {
			result res = GetLastResult();
			AppLogDebug("ConvertForm::OnTransactionReadyToRead: [%s]: failed to get HTTP body", GetErrorMessage(res));
			return;
		}
	} else {
		AppLogDebug("ConvertForm::OnTransactionReadyToRead: failed to get proper HTTP response, status code: [%d], text: [%S]", (int)pHttpResponse->GetStatusCode(), pHttpResponse->GetStatusText().GetPointer());
	}

	if (pData && availableBodyLen > 0) {
		if (__pDownloadedData) {
			if (__pDownloadedData->GetPosition() >= __pDownloadedData->GetLimit()) {
				ByteBuffer *tmp = new ByteBuffer;
				if (IsFailed(tmp->Construct(__pDownloadedData->GetCapacity() + availableBodyLen))) {
					delete tmp;
				} else {
					__pDownloadedData->SetPosition(0);
					tmp->ReadFrom(*__pDownloadedData);

					delete __pDownloadedData;
					__pDownloadedData = tmp;
				}
			}
			__pDownloadedData->ReadFrom(*pData);
		}
		delete pData;
	}
}

void ConvertForm::OnTransactionAborted(HttpSession &httpSession, HttpTransaction &httpTransaction, result r) {
	__pSpinTimer->Cancel();

	CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
	if (item) {
		Bitmap *bmp = GetBitmapN(L"warning_icon.png");
		if (bmp) {
			item->SetElement(ID_UNIT_SPINNER, *bmp, null);
			delete bmp;
		} else {
			item->SetElement(ID_UNIT_SPINNER, *__pEmptyBmp, null);
		}
	}
	__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

	__spinItemIndex++;
	if (__spinItemIndex >= __pUnitsList->GetItemCountAt(__spinGroupIndex)) {
		__spinItemIndex = 0;
		__spinGroupIndex++;
		if (__spinGroupIndex >= __pUnitsList->GetGroupCount()) {
			__spinItemIndex = -1;
			__spinGroupIndex = -1;
			__fetchItemID = -1;

			SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_SOFTKEY_BACK"));
			RequestRedraw(true);
		}
	}
	if (__spinItemIndex >= 0 && __spinGroupIndex >= 0) {
		//set item ID for fetching
		__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
		//initiate download for next unit
		FetchCurrencies();
	}
}

void ConvertForm::OnTransactionHeaderCompleted(HttpSession &httpSession, HttpTransaction &httpTransaction, int headerLen, bool rs) {
	static int redirect_count = 0;

	HttpResponse* pHttpResponse = httpTransaction.GetResponse();
	if(!pHttpResponse) {
		return;
	}

	HttpHeader *pHeader = pHttpResponse->GetHeader();
	if (!pHeader) {
		return;
	}

	if (pHttpResponse->GetStatusCode() == NET_HTTP_STATUS_MOVED_PERMANENTLY
		|| pHttpResponse->GetStatusCode() == NET_HTTP_STATUS_MOVED_TEMPORARILY) {
		String redirect_url;
		IList *pFields = pHeader->GetFieldNamesN();
		IEnumerator *pEnum = pFields->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			String *field = static_cast<String *>(pEnum->GetCurrent());
			if (field->Equals(L"Location", false)) {
				IEnumerator *pVals = pHeader->GetFieldValuesN(*field);
				if (!IsFailed(pVals->MoveNext())) {
					redirect_url = *static_cast<String *>(pVals->GetCurrent());
				}
				delete pVals;
				break;
			}
		}
		delete pEnum;
		delete pFields;

		if (!redirect_url.IsEmpty()) {
			if (redirect_count >= 10) {
				//too much redirects, report error
				OnTransactionAborted(httpSession, httpTransaction, E_INVALID_ADDRESS);

				redirect_count = 0;
				return;
			}
			redirect_count++;

			//init download from redirected url
			FetchCurrencies(redirect_url);
		} else {
			//received empty redirect url, report error
			OnTransactionAborted(httpSession, httpTransaction, E_INVALID_ADDRESS);
			redirect_count = 0;
		}
		return;
	}
	redirect_count = 0;
}

void ConvertForm::OnTransactionCompleted(HttpSession &httpSession, HttpTransaction &httpTransaction) {
	__pSpinTimer->Cancel();

	CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
	if (item) {
		item->SetElement(ID_UNIT_SPINNER, *__pEmptyBmp, null);

		char *data = new char[__pDownloadedData->GetLimit() + 1];
		data[__pDownloadedData->GetLimit()] = 0;
		memcpy((void*)data, (const void*)__pDownloadedData->GetPointer(), __pDownloadedData->GetLimit());
		String recv_str = String(data);

		double res_val = -1.0;
		int rhs_index = -1;
		if (!IsFailed(recv_str.IndexOf(L"rhs: ", 0, rhs_index)) && rhs_index >= 0) {
			int sp_index = -1;
			if (!IsFailed(recv_str.IndexOf(L" ", rhs_index + 6, sp_index)) && sp_index >= 0) {
				String val;
				recv_str.SubString(rhs_index + 6, sp_index - rhs_index - 6, val);
				val.Replace(L" ", L"");//replacing non-breaking space, not a usual one

				Double::Parse(val, res_val);
				res_val = res_val * __curValue;
			}
		}
		if (res_val < 0.0) {
			Bitmap *bmp = GetBitmapN(L"warning_icon.png");
			if (bmp) {
				item->SetElement(ID_UNIT_SPINNER, *bmp, null);
				delete bmp;
			}
		} else {
			Font font;
			font.Construct(FONT_STYLE_PLAIN, 35);

			EnrichedText ent;
			ent.Construct(Dimension(460, 50));
			ent.SetTextWrapStyle(TEXT_WRAP_NONE);

			int wid, hei, actLen;

			String strVal = L"";
			if (Double::IsInfinity(res_val)) {
				strVal.Append(L"\u221E");
			} else {
				strVal.Append(Double::ToString(res_val));
			}

			TextElement elem_val;
			elem_val.Construct(strVal);
			elem_val.SetFont(font);
			ent.Add(elem_val);
			ent.GetTextExtent(0, strVal.GetLength(), wid, hei, actLen);

			int spaces = (int)((double)(187 - wid) / 10);
			for(int i = 0; i < spaces; i++) {
				strVal.Insert(' ', 0);
			}
			item->SetElement(ID_UNIT_VALUE, strVal);
		}
	}
	__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

	__spinItemIndex++;
	if (__spinItemIndex >= __pUnitsList->GetItemCountAt(__spinGroupIndex)) {
		__spinItemIndex = 0;
		__spinGroupIndex++;
		if (__spinGroupIndex >= __pUnitsList->GetGroupCount()) {
			__spinItemIndex = -1;
			__spinGroupIndex = -1;
			__fetchItemID = -1;

			SetSoftkeyText(SOFTKEY_1, GetString(L"IDS_SOFTKEY_BACK"));
			RequestRedraw(true);
		}
	}
	if (__spinItemIndex >= 0 && __spinGroupIndex >= 0) {
		//set item ID for fetching
		__fetchItemID = __pUnitsList->GetItemIdAt(__spinGroupIndex, __spinItemIndex);
		//initiate download for next unit
		FetchCurrencies();
	}
}

void ConvertForm::OnTimerExpired(Timer &timer) {
	if (__spinItemIndex >= 0 && __spinGroupIndex >= 0) {
		static int progress = 0;
		int index = progress % 8;

		CustomListItem *item = const_cast<CustomListItem *>(__pUnitsList->GetItemAt(__spinGroupIndex, __spinItemIndex));
		if (item) {
			switch (index)
			{
			case 0:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap1, null);
				break;
			case 1:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap2, null);
				break;
			case 2:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap3, null);
				break;
			case 3:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap4, null);
				break;
			case 4:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap5, null);
				break;
			case 5:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap6, null);
				break;
			case 6:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap7, null);
				break;
			case 7:
				item->SetElement(ID_UNIT_SPINNER, *__spinnerAnimData.__pProgressBitmap8, null);
				break;
			default:
				break;
			}
		}
		__pUnitsList->RefreshItem(__spinGroupIndex, __spinItemIndex);

		progress++;
		__pSpinTimer->Start(ANIM_INTERVAL);
	}
}
