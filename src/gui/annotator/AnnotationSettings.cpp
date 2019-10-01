/*
 * Copyright (C) 2019 Damir Porobic <damir.porobic@gmx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "AnnotationSettings.h"

namespace kImageAnnotator {

AnnotationSettings::AnnotationSettings(Config *config)
{
	mConfig = config;

	initGui();
	loadToolTypeFromConfig();
}

AnnotationSettings::~AnnotationSettings()
{
	delete mToolPicker;
	delete mColorPicker;
	delete mWidthPicker;
	delete mTextColorPicker;
	delete mFontSizePicker;
	delete mFillTypePicker;
	delete mFirstNumberPicker;
	delete mBlurRadiusPicker;
}

void AnnotationSettings::loadFromItem(AbstractAnnotationItem *item)
{
	auto properties = item->properties();
	mColorPicker->setColor(properties->color());
	mTextColorPicker->setColor(properties->textColor());
	mWidthPicker->setNumber(properties->width());
	mFillTypePicker->setFillType(properties->fillType());
	auto textProperties = dynamic_cast<const AnnotationTextProperties *>(properties);
	if(textProperties != nullptr) {
		mFontSizePicker->setNumber(textProperties->font().pointSize());
	}
	auto blurProperties = dynamic_cast<const AnnotationBlurProperties *>(properties);
	if(blurProperties != nullptr) {
		mBlurRadiusPicker->setNumber(blurProperties->radius());
	}
	mWidgetConfigurator.setCurrentTool(item->toolType());
}

void AnnotationSettings::activateSelectTool()
{
	mToolPicker->setTool(ToolTypes::Select);
	mWidgetConfigurator.setCurrentTool(ToolTypes::Select);
}

ToolTypes AnnotationSettings::toolType() const
{
	return mToolPicker->tool();
}

void AnnotationSettings::initGui()
{
	mMainLayout = new QVBoxLayout();
	mToolPicker = new ToolPicker();
	mColorPicker = new ColorPicker(IconLoader::load(QStringLiteral("color.svg")), tr("Color"));
	mWidthPicker = new NumberPicker(IconLoader::load(QStringLiteral("width.svg")), tr("Width"));
	mTextColorPicker = new ColorPicker(IconLoader::load(QStringLiteral("textColor.svg")), tr("Text Color"));
	mFontSizePicker = new NumberPicker(IconLoader::load(QStringLiteral("fontSize.svg")), tr("Font Size"));
	mFontSizePicker->setRange(10, 40);
	mFillTypePicker = new FillTypePicker(IconLoader::load(QStringLiteral("fillType.svg")), tr("Border And Fill Visibility"));
	mFirstNumberPicker = new NumberPicker(IconLoader::load(QStringLiteral("number.svg")), tr("Starting Number"));
	mFirstNumberPicker->setRange(1, 100);
	mBlurRadiusPicker = new NumberPicker(IconLoader::load(QStringLiteral("blur.svg")), tr("Blur Radius"));
	mBlurRadiusPicker->setRange(1, 20);

	mMainLayout->addWidget(mToolPicker);
	mMainLayout->addSpacing(20);
	mMainLayout->addWidget(mColorPicker);
	mMainLayout->addWidget(mWidthPicker);
	mMainLayout->addWidget(mTextColorPicker);
	mMainLayout->addWidget(mFontSizePicker);
	mMainLayout->addWidget(mFillTypePicker);
	mMainLayout->addWidget(mFirstNumberPicker);
	mMainLayout->addWidget(mBlurRadiusPicker);
	mMainLayout->setAlignment(Qt::AlignTop | Qt::AlignCenter);

	mWidgetConfigurator.setColorWidget(mColorPicker);
	mWidgetConfigurator.setTextColorWidget(mTextColorPicker);
	mWidgetConfigurator.setWidthWidget(mWidthPicker);
	mWidgetConfigurator.setFillTypeWidget(mFillTypePicker);
	mWidgetConfigurator.setFontSizeWidget(mFontSizePicker);
	mWidgetConfigurator.setFirstNumberWidget(mFirstNumberPicker);
	mWidgetConfigurator.setBlurRadiusWidget(mBlurRadiusPicker);

	setLayout(mMainLayout);

	setFocusPolicy(Qt::ClickFocus);

	connect(mToolPicker, &ToolPicker::toolSelected, this, &AnnotationSettings::saveToolType);
	connect(mColorPicker, &ColorPicker::colorSelected, this, &AnnotationSettings::saveToolColor);
	connect(mWidthPicker, &NumberPicker::numberSelected, this, &AnnotationSettings::saveToolWidth);
	connect(mTextColorPicker, &ColorPicker::colorSelected, this, &AnnotationSettings::saveToolTextColor);
	connect(mFontSizePicker, &NumberPicker::numberSelected, this, &AnnotationSettings::saveToolFontSize);
	connect(mFillTypePicker, &FillTypePicker::fillSelected, this, &AnnotationSettings::saveToolFillType);
	connect(mFirstNumberPicker, &NumberPicker::numberSelected, this, &AnnotationSettings::saveFirstBadgeNumber);
	connect(mBlurRadiusPicker, &NumberPicker::numberSelected, this, &AnnotationSettings::saveBlurRadius);
}

void AnnotationSettings::loadToolTypeFromConfig()
{
	mToolPicker->setTool(mConfig->selectedTool());
}

void AnnotationSettings::loadFromConfig(ToolTypes tool)
{
	mColorPicker->setColor(mConfig->toolColor(tool));
	mTextColorPicker->setColor(mConfig->toolTextColor(tool));
	mWidthPicker->setNumber(mConfig->toolWidth(tool));
	mFillTypePicker->setFillType(mConfig->toolFillType(tool));
	mFontSizePicker->setNumber(mConfig->toolFontSize(tool));
	mBlurRadiusPicker->setNumber(mConfig->blurRadius());
	mWidgetConfigurator.setCurrentTool(tool);
}

void AnnotationSettings::saveToolType(ToolTypes toolType)
{
	mConfig->setSelectedTool(toolType);
	loadFromConfig(toolType);
	emit toolChanged(toolType);
}

void AnnotationSettings::saveToolColor(const QColor &color)
{
	mConfig->setToolColor(color, mToolPicker->tool());
}

void AnnotationSettings::saveToolTextColor(const QColor &color)
{
	mConfig->setToolTextColor(color, mToolPicker->tool());
}

void AnnotationSettings::saveToolWidth(int size)
{
	mConfig->setToolWidth(size, mToolPicker->tool());
}

void AnnotationSettings::saveToolFillType(FillTypes fill)
{
	mConfig->setToolFillType(fill, mToolPicker->tool());
}

void AnnotationSettings::saveToolFontSize(int size)
{
	mConfig->setToolFontSize(size, mToolPicker->tool());
}

void AnnotationSettings::saveFirstBadgeNumber(int number)
{
	emit firstBadgeNumberChanged(number);
}

void AnnotationSettings::saveBlurRadius(int radius)
{
	mConfig->setBlurRadius(radius);
}

QColor AnnotationSettings::toolColor() const
{
	return mColorPicker->color();
}

QColor AnnotationSettings::textColor() const
{
	return mTextColorPicker->color();
}

int AnnotationSettings::toolWidth() const
{
	return mWidthPicker->number();
}

FillTypes AnnotationSettings::fillType() const
{
	return mFillTypePicker->fillType();
}

int AnnotationSettings::fontSize() const
{
	return mFontSizePicker->number();
}

int AnnotationSettings::blurRadius() const
{
	return mBlurRadiusPicker->number();
}

} // namespace kImageAnnotator
