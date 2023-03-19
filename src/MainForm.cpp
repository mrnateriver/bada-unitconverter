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
#include "MainForm.h"
#include "ConvertForm.h"
#include <FApp.h>
#include <FMedia.h>

using namespace Osp::App;
using namespace Osp::Media;

MainForm::MainForm(void) {
	__pCategoryList = null;
	__pParser = null;
	__pCategoryItemList = null;

	__pFormat = null;
}

MainForm::~MainForm(void) {
	if (__pParser) {
		delete __pParser;
	}
	if (__pCategoryItemList) {
		delete __pCategoryItemList;
	}
	if (__pFormat) {
		delete __pFormat;
	}
}

bool MainForm::Initialize() {
	Construct(FORM_STYLE_TITLE | FORM_STYLE_INDICATOR);
	SetTitleText(GetString(L"IDS_MAINFORM_TITLE"), ALIGNMENT_CENTER);
	SetBackgroundColor(Color(0xdfe4e0, false));

	CustomList *pList = new CustomList;
	pList->Construct(Rectangle(0,0,480,712), CUSTOM_LIST_STYLE_NORMAL, true);
	pList->SetTextOfEmptyList(L" ");
	pList->AddCustomItemEventListener(*this);

	CustomListItemFormat *pFormat = new CustomListItemFormat;
	pFormat->Construct();
	pFormat->AddElement(ID_CATEGORY_ICON, Rectangle(12,12,56,56));
	pFormat->AddElement(ID_CATEGORY_NAME, Rectangle(80,24,390,35), 35, Color(0x3c3c3c, false), Color::COLOR_WHITE);

	AddControl(*pList);
	__pCategoryList = pList;

	__pFormat = pFormat;

	__pConvForm = new ConvertForm;
	__pConvForm->Initialize(this);

	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->AddControl(*__pConvForm);

	return true;
}

void MainForm::DialogCallback(void) {
	Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->SetCurrentForm(*this);

	Draw();
	Show();
}

result MainForm::OnInitializing(void) {
	__pParser = new UnitsParser;
	result res = __pParser->Construct();
	if (IsFailed(res)) {
		AppLogException("Failed to construct units parser, error: [%s]", GetErrorMessage(res));
		return res;
	}

	__pCategoryItemList = __pParser->GetCategoryListN();
	if (__pCategoryItemList) {
		IEnumeratorT<UnitsCategory *> *pEnum = __pCategoryItemList->GetEnumeratorN();
		if (pEnum) {
			int i = 0;
			while (!IsFailed(pEnum->MoveNext())) {
				UnitsCategory *pCat; pEnum->GetCurrent(pCat);

				Bitmap *pBmp = GetBitmapN(pCat->GetCategoryIconPath());
				String name = pCat->GetCategoryName();

				CustomListItem *pItem = new CustomListItem;
				pItem->Construct(80);
				pItem->SetItemFormat(*__pFormat);

				pItem->SetElement(ID_CATEGORY_ICON, *pBmp, null);
				pItem->SetElement(ID_CATEGORY_NAME, name);

				__pCategoryList->AddItem(*pItem, i++);
				if (pBmp) {
					delete pBmp;
				}
			}
		}
		delete pEnum;
	} else {
		AppLogException("Failed to acquire units category list, error: [%s]", GetErrorMessage(E_INVALID_STATE));
		return GetLastResult();
	}

	return res;
}

result MainForm::OnTerminating(void) {
	return E_SUCCESS;
}

void MainForm::OnItemStateChanged(const Control &source, int index, int itemId, ItemStatus status) {
	if (__pCategoryItemList) {
		UnitsCategory *pItem; __pCategoryItemList->GetAt(itemId, pItem);
		__pConvForm->SetData(pItem);

		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		pFrame->SetCurrentForm(*__pConvForm);

		__pConvForm->Draw();
		__pConvForm->Show();
		__pConvForm->RefreshUnitsList();

		AppRegistry *pReg = Application::GetInstance()->GetAppRegistry();
		int read_man = 0;
		if (IsFailed(pReg->Get(L"HAS_READ_MANUAL", read_man))) {
			pReg->Add(L"HAS_READ_MANUAL", 1);
		}

		if (!read_man) {
			MessageBox *pBox = new MessageBox;
			pBox->Construct(GetString(L"IDS_HELP_MSG_TITLE"), GetString(L"IDS_HELP_MSG_TEXT"), MSGBOX_STYLE_OK);
			int res;
			pBox->ShowAndWait(res);
			delete pBox;
		}
	}
}

String MainForm::GetString(const String &id) const {
	String str = id;
	result res = Application::GetInstance()->GetAppResource()->GetString(id, str);
	if (IsFailed(res)) {
		AppLogException("Could not retrieve string by id [%S]", id.GetPointer());
		SetLastResult(res);
	}
	return str;
}

Bitmap *MainForm::GetBitmapN(const String &name) const {
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
