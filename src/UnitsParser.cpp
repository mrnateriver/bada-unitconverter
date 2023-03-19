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
#include "UnitsParser.h"
#include <FXml.h>
#include <FApp.h>

using namespace Osp::Xml;
using namespace Osp::App;

UnitsCategory::UnitsCategory(void) {
	__pUnits = null;
	__pSystems = null;
	__pRefUnit = 0;
	__spID = -1;
}

UnitsCategory::~UnitsCategory(void) {
	if (__pUnits) {
		delete __pUnits;
	}
	if (__pSystems) {
		delete __pSystems;
	}
}

void UnitsCategory::Construct(const String &name, const String &path, const String &sm_path) {
	__catItem.name = name;
	__catItem.icon_path = path;
	__catItem.small_icon_path = sm_path;

	if (!__pUnits) {
		__pUnits = new LinkedListT<Unit>;
	}
	if (!__pSystems) {
		__pSystems = new LinkedListT<String>;
	}
}

void UnitsCategory::AddUnit(const Unit &unit) {
	__pUnits->Add(unit);
}

void UnitsCategory::AddUnitSystem(const String &sys) {
	__pSystems->Add(sys);
}

ArrayListT<Unit> *UnitsCategory::GetUnitsListN(void) const {
	ArrayListT<Unit> *pRet = new ArrayListT<Unit>;
	pRet->Construct(*__pUnits);
	return pRet;
}

ArrayListT<String> *UnitsCategory::GetSystemsListN(void) const {
	ArrayListT<String> *pRet = new ArrayListT<String>;
	pRet->Construct(*__pSystems);
	return pRet;
}

int UnitsCategory::GetUnitsListLength(void) const {
	return __pUnits->GetCount();
}

int UnitsCategory::GetSystemsListLength(void) const {
	return __pSystems->GetCount();
}

Unit UnitsCategory::GetUnitByListID(int index) const {
	Unit unit; __pUnits->GetAt(index, unit);
	return unit;
}

String UnitsCategory::GetSystemByListID(int index) const {
	String str; __pSystems->GetAt(index, str);
	return str;
}

double UnitsCategory::CalculateNonLinearUnit(Unit src, double src_val, Unit dst) const {
	if (dst.sp_id == src.sp_id) {
		return src_val;
	}
	switch (__spID) {
	//temperature
	case 0: {
		double kelv_val = 0.0;
		if (src.sp_id == 0) {
			//celsius
			kelv_val = src_val + 273.15;
		} else if (src.sp_id == 1) {
			//fahrenheit
			kelv_val = (((src_val - 32) * 5) / 9) + 273.15;
		} else if (src.sp_id == 2) {
			//kelvin
			kelv_val = src_val;
		} else if (src.sp_id == 3) {
			//reaumur
			kelv_val = (src_val * 1.25) + 273.15;
		} else if (src.sp_id == 4) {
			//rankine
			kelv_val = src_val / 1.8;
		} else {
			kelv_val = src_val;
		}

		double res_val = 0.0;
		if (dst.sp_id == 0) {
			//celsius
			res_val = kelv_val - 273.15;
		} else if (dst.sp_id == 1) {
			//fahrenheit
			res_val = (((kelv_val - 273.15) * 9) / 5) + 32;
		} else if (dst.sp_id == 2) {
			//kelvin
			res_val = kelv_val;
		} else if (dst.sp_id == 3) {
			//reaumur
			res_val = (kelv_val - 273.15) * 0.8;
		} else if (dst.sp_id == 4) {
			//rankine
			res_val = kelv_val * 1.8;
		} else {
			res_val = kelv_val;
		}
		return res_val;
	}
	//fuel consumption
	case 1: {
		//litres = 3.785412 * US gallons
		//litres = 4.546099 * UK gallons
		//km = 1.609344 * miles
		double litres_km_val = 0.0;
		if (src.sp_id == 0) {
			//litres per 100 km
			litres_km_val = src_val;
		} else if (src.sp_id == 1) {
			//km per litre
			litres_km_val = 100 / src_val;
		} else if (src.sp_id == 2) {
			//miles per US gallon
			litres_km_val = (3.785412 * 100) / (src_val * 1.609344);
		} else if (src.sp_id == 3) {
			//US gallons per 100 miles
			litres_km_val = (src_val * 3.785412) / 1.609344;
		} else if (src.sp_id == 4) {
			//miles per UK gallon
			litres_km_val = (4.546099 * 100) / (src_val * 1.609344);
		} else if (src.sp_id == 5) {
			//UK gallons per 100 miles
			litres_km_val = (src_val * 4.546099) / 1.609344;
		} else if (src.sp_id == 6) {
			//litres per 100 miles
			litres_km_val = src_val / 1.609344;
		}

		double res_val = 0.0;
		if (dst.sp_id == 0) {
			//litres per 100 km
			res_val = litres_km_val;
		} else if (dst.sp_id == 1) {
			//km per litre
			res_val = 100 / litres_km_val;
		} else if (dst.sp_id == 2) {
			//miles per US gallon
			res_val = (3.785412 * 100) / (litres_km_val * 1.609344);
		} else if (dst.sp_id == 3) {
			//US gallons per 100 miles
			res_val = (litres_km_val * 1.609344) / 3.785412;
		} else if (dst.sp_id == 4) {
			//miles per UK gallon
			res_val = (4.546099 * 100) / (litres_km_val * 1.609344);
		} else if (dst.sp_id == 5) {
			//UK gallons per 100 miles
			res_val = (litres_km_val * 1.609344) / 4.546099;
		} else if (dst.sp_id == 6) {
			//litres per 100 miles
			res_val = litres_km_val * 1.609344;
		}
		return res_val;
	}
	//data transfer rate
	case 2:
	//acceleration
	case 3:
	//speed
	case 4:
	//distance and length
	case 5: {
		if (src.sp_id >= 0 && dst.sp_id >= 0) {
			Unit ref_unit = GetUnitByListID(GetReferenceUnitID());

			double ref = (src.multiplier * ref_unit.multiplier) / src_val;
			ref = (ref_unit.multiplier * dst.multiplier) / ref;

			return ref;
		} else {
			double ref = src_val / src.multiplier;
			return dst.multiplier / ref;
		}
	}
	default: {
		return 0.0;
	}
	}
}

bool operator ==(UnitsCategoryItem item1, UnitsCategoryItem item2) {
	return (item1.name.Equals(item2.name, true)) && (item1.icon_path.Equals(item2.icon_path, false)) &&
			(item1.small_icon_path.Equals(item2.small_icon_path, false));
}
bool operator !=(UnitsCategoryItem item1, UnitsCategoryItem item2) {
	return (!item1.name.Equals(item2.name, true)) || (!item1.icon_path.Equals(item2.icon_path, false)) ||
			(!item1.small_icon_path.Equals(item2.small_icon_path, false));
}

bool operator ==(Unit item1, Unit item2) {
	return (item1.sp_id == item2.sp_id) && (item1.multiplier == item2.multiplier) &&
			(item1.name.Equals(item2.name, false)) && (item1.system_id == item2.system_id);
}
bool operator !=(Unit item1, Unit item2) {
	return (item1.sp_id != item2.sp_id) || (item1.multiplier != item2.multiplier) ||
			(!item1.name.Equals(item2.name, false)) || (item1.system_id != item2.system_id);
}

UnitsParser::UnitsParser(void) {
	__pCategories = null;
}

UnitsParser::~UnitsParser(void) {
	if (__pCategories) {
		IEnumeratorT<UnitsCategory *> *pEnum = __pCategories->GetEnumeratorN();
		if (pEnum) {
			while (!IsFailed(pEnum->MoveNext())) {
				UnitsCategory *cat; pEnum->GetCurrent(cat);
				if (cat) {
					delete cat;
				}
			}
		}
		delete pEnum;
		delete __pCategories;
	}
}

result UnitsParser::Construct(void) {
	if (!__pCategories) {
		__pCategories = new LinkedListT<UnitsCategory *>;
	}
	LoadFromFile();

	return E_SUCCESS;
}

ArrayListT<UnitsCategory *> *UnitsParser::GetCategoryListN(void) {
	ArrayListT<UnitsCategory *> *pRet = new ArrayListT<UnitsCategory *>;
	pRet->Construct(*__pCategories);
	return pRet;
}

result UnitsParser::LoadFromFile(void) {
	String lang_str = L"en";
	result res = Application::GetInstance()->GetAppResource()->GetString(L"IDS_XML_PARSE_LANG_STR", lang_str);
	if (IsFailed(res)) {
		AppLogException("Could not retrieve language ID string, defaulting to EN");
	}

	xmlDocPtr pDoc = xmlParseFile("/Res/conv_data.xml");
	xmlNodePtr pRoot = xmlDocGetRootElement(pDoc);
	xmlNodePtr pCurNode = null;

	for (pCurNode = pRoot->children; pCurNode; pCurNode = pCurNode->next) {
		if (pCurNode->type != XML_TEXT_NODE) {
			String cat_name;
			String icon_path;
			String small_icon_path;
			int ref_unit = 0;
			int sp_id = -1;

			xmlAttrPtr pCurAttr = null;
			for (pCurAttr = pCurNode->properties; pCurAttr; pCurAttr = pCurAttr->next) {
				String attr_name;
				Utility::StringUtil::Utf8ToString((char*)pCurAttr->name, attr_name);
				String attr_val;
				Utility::StringUtil::Utf8ToString((char*)pCurAttr->children->content, attr_val);

				if (attr_name.Equals(lang_str, false)) {
					cat_name = attr_val;
				} else if (attr_name.Equals(L"icon_path", false)) {
					icon_path = attr_val;
				} else if (attr_name.Equals(L"small_icon_path", false)) {
					small_icon_path = attr_val;
				} else if (attr_name.Equals(L"ref", false)) {
					Integer::Parse(attr_val, ref_unit);
				} else if (attr_name.Equals(L"sp_id", false)) {
					Integer::Parse(attr_val, sp_id);
				}
			}

			UnitsCategory *pCategory = new UnitsCategory;
			pCategory->Construct(cat_name, icon_path, small_icon_path);
			pCategory->SetReferenceUnitID(ref_unit);
			pCategory->SetSpecialConversionID(sp_id);

			int sys_id = 0;
			xmlNodePtr pCurSystem = null;
			for (pCurSystem = pCurNode->children; pCurSystem; pCurSystem = pCurSystem->next) {
				if (pCurSystem->type != XML_TEXT_NODE) {
					for (pCurAttr = pCurSystem->properties; pCurAttr; pCurAttr = pCurAttr->next) {
						String attr_name;
						Utility::StringUtil::Utf8ToString((char*)pCurAttr->name, attr_name);
						String attr_val;
						Utility::StringUtil::Utf8ToString((char*)pCurAttr->children->content, attr_val);

						if (attr_name.Equals(lang_str, false)) {
							pCategory->AddUnitSystem(attr_val);
							break;
						}
					}

					xmlNodePtr pCurUnit = null;
					for (pCurUnit = pCurSystem->children; pCurUnit; pCurUnit = pCurUnit->next) {
						if (pCurUnit->type != XML_TEXT_NODE) {
							String unit_name;
							double ratio = 1.0;
							int usp_id = -1;
							String ccode;

							for (pCurAttr = pCurUnit->properties; pCurAttr; pCurAttr = pCurAttr->next) {
								String attr_name;
								Utility::StringUtil::Utf8ToString((char*)pCurAttr->name, attr_name);
								String attr_val;
								Utility::StringUtil::Utf8ToString((char*)pCurAttr->children->content, attr_val);

								if (attr_name.Equals(lang_str, false)) {
									unit_name = attr_val;
								} else if (attr_name.Equals(L"ratio", false)) {
									res = Double::Parse(attr_val, ratio);
									if (IsFailed(res)) {
										AppLogException("Failed to parse ratio [%S], defaulting to 1.0", attr_val.GetPointer());
									}
								} else if (attr_name.Equals(L"sp_id", false)) {
									Integer::Parse(attr_val, usp_id);
								} else if (attr_name.Equals(L"ccode", false)) {
									ccode = attr_val;
								}
							}

							Unit unit = Unit(ratio, unit_name, sys_id, usp_id);
							if (!ccode.IsEmpty()) {
								unit.ccode = ccode;
							}
							pCategory->AddUnit(unit);
						}
					}

					sys_id++;
				}
			}

			__pCategories->Add(pCategory);
		}
	}

	xmlFreeDoc(pDoc);
	return E_SUCCESS;
}
