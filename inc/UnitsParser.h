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
#ifndef UNITSPARSER_H_
#define UNITSPARSER_H_

#include <FBase.h>

using namespace Osp::Base;
using namespace Osp::Base::Collection;

struct Unit {
	Unit(void) {
		multiplier = 1.0;
		name = L"";
		system_id = 0;
		sp_id = -1;
	}

	Unit(double ratio, const String &nm, int sys, int sp) {
		multiplier = ratio;
		name = nm;
		system_id = sys;
		sp_id = sp;
	}

	double multiplier;
	String name;
	int system_id;
	int sp_id;

	String ccode;
};

struct UnitsCategoryItem {
	String name;
	String icon_path;
	String small_icon_path;
};

class UnitsCategory {
public:
	UnitsCategory(void);
	~UnitsCategory(void);

	void Construct(const String &name, const String &path, const String &sm_path);
	void AddUnit(const Unit &unit);
	void AddUnitSystem(const String &sys);

	UnitsCategoryItem GetCategoryItem(void) const {
		return __catItem;
	}

	void SetCategoryName(const String &name) {
		__catItem.name = name;
	}
	String GetCategoryName(void) const {
		return __catItem.name;
	}

	void SetCategoryIconPath(const String &path) {
		__catItem.icon_path = path;
	}
	String GetCategoryIconPath(void) const {
		return __catItem.icon_path;
	}

	void SetCategorySmallIconPath(const String &path) {
		__catItem.small_icon_path = path;
	}
	String GetCategorySmallIconPath(void) const {
		return __catItem.small_icon_path;
	}

	void SetReferenceUnitID(int unit_index) {
		__pRefUnit = unit_index;
	}
	int GetReferenceUnitID(void) const {
		return __pRefUnit;
	}

	void SetSpecialConversionID(int index) {
		__spID = index;
	}
	int GetSpecialConversionID(void) const {
		return __spID;
	}

	double CalculateNonLinearUnit(Unit src, double src_val, Unit dst) const;

	ArrayListT<Unit> *GetUnitsListN(void) const;
	ArrayListT<String> *GetSystemsListN(void) const;

	int GetUnitsListLength(void) const;
	int GetSystemsListLength(void) const;

	Unit GetUnitByListID(int index) const;
	String GetSystemByListID(int index) const;

private:
	UnitsCategoryItem __catItem;

	LinkedListT<Unit> *__pUnits;
	LinkedListT<String> *__pSystems;
	int __pRefUnit;
	int __spID;
};

bool operator ==(UnitsCategoryItem item1, UnitsCategoryItem item2);
bool operator !=(UnitsCategoryItem item1, UnitsCategoryItem item2);

bool operator ==(Unit item1, Unit item2);
bool operator !=(Unit item1, Unit item2);

class UnitsParser {
public:
	UnitsParser(void);
	virtual ~UnitsParser(void);

	result Construct(void);

	ArrayListT<UnitsCategory *> *GetCategoryListN(void);

private:
	result LoadFromFile(void);

	LinkedListT<UnitsCategory *> *__pCategories;
};

#endif
