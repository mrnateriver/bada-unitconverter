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
#include "UnitConverter.h"
#include "MainForm.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;

UnitConverter::UnitConverter(void) {
}

UnitConverter::~UnitConverter(void) {
}

Application* UnitConverter::CreateInstance(void) {
	return new UnitConverter();
}

bool UnitConverter::OnAppInitializing(AppRegistry& appRegistry) {
	MainForm *pMainForm = new MainForm();
	pMainForm->Initialize();

	Frame *pFrame = GetAppFrame()->GetFrame();
	pFrame->AddControl(*pMainForm);

	pFrame->SetCurrentForm(*pMainForm);

	pMainForm->Draw();
	pMainForm->Show();

	return true;
}

bool UnitConverter::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination) {
	return true;
}

void UnitConverter::OnForeground(void) {
}

void UnitConverter::OnBackground(void) {
}

void UnitConverter::OnLowMemory(void) {
}

void UnitConverter::OnBatteryLevelChanged(BatteryLevel batteryLevel) {
}
