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
#ifndef _MAINFORM_H_
#define _MAINFORM_H_

#include "UnitsParser.h"
#include <FBase.h>
#include <FUi.h>
#include <FGraphics.h>

using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Graphics;

class ConvertForm;

class MainForm: public Form, public ICustomItemEventListener {
public:
	MainForm(void);
	virtual ~MainForm(void);

	bool Initialize(void);
	void DialogCallback(void);

private:
	static const int ID_CATEGORY_ICON = 501;
	static const int ID_CATEGORY_NAME = 502;

	virtual result OnInitializing(void);
	virtual result OnTerminating(void);

	virtual void OnItemStateChanged(const Control &source, int index, int itemId, ItemStatus status);
	virtual void OnItemStateChanged(const Control &source, int index, int itemId, int elementId, ItemStatus status) { }

	String GetString(const String &id) const;
	Bitmap *GetBitmapN(const String &name) const;

	CustomList *__pCategoryList;
	UnitsParser *__pParser;
	ArrayListT<UnitsCategory *> *__pCategoryItemList;

	CustomListItemFormat *__pFormat;
	ConvertForm *__pConvForm;
};

#endif
