/*!
	@file
	@author		Albert Semenov
	@date		11/2007
*/
/*
	This file is part of MyGUI.

	MyGUI is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	MyGUI is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with MyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "MyGUI_Precompiled.h"
#include "MyGUI_Gui.h"
#include "MyGUI_Widget.h"
#include "MyGUI_InputManager.h"
#include "MyGUI_SkinManager.h"
#include "MyGUI_SubWidgetManager.h"
#include "MyGUI_WidgetManager.h"
#include "MyGUI_ResourceSkin.h"
#include "MyGUI_WidgetDefines.h"
#include "MyGUI_LayerItem.h"
#include "MyGUI_LayerManager.h"
#include "MyGUI_RenderItem.h"
#include "MyGUI_ISubWidget.h"
#include "MyGUI_ISubWidgetText.h"
#include "MyGUI_StaticText.h"
#include "MyGUI_FactoryManager.h"
#include "MyGUI_CoordConverter.h"
#include "MyGUI_RenderManager.h"
#include "MyGUI_ToolTipManager.h"
#include "MyGUI_LayoutManager.h"

namespace MyGUI
{

	Widget::Widget() :
		mWidgetClient(nullptr),
		mEnabled(true),
		mInheritsEnabled(true),
		mInheritsVisible(true),
		mAlpha(ALPHA_MAX),
		mRealAlpha(ALPHA_MAX),
		mInheritsAlpha(true),
		mParent(nullptr),
		mWidgetStyle(WidgetStyle::Child),
		mContainer(nullptr),
		mAlign(Align::Default),
		mVisible(true)
	{
	}

	Widget::~Widget()
	{
	}

	void Widget::_initialise(WidgetStyle _style, const IntCoord& _coord, const std::string& _skinName, Widget* _parent, ICroppedRectangle* _croppedParent, const std::string& _name)
	{
		ResourceSkin* skinInfo = nullptr;
		ResourceLayout* templateInfo = nullptr;

		if (LayoutManager::getInstance().isExist(_skinName))
			templateInfo = LayoutManager::getInstance().getByName(_skinName);
		else
			skinInfo = SkinManager::getInstance().getByName(_skinName);

		mCoord = _coord;

		mAlign = Align::Default;
		mWidgetStyle = _style;
		mName = _name;

		mCroppedParent = _croppedParent;
		mParent = _parent;


#if MYGUI_DEBUG_MODE == 1
		// проверяем соответсвие входных данных
		if (mWidgetStyle == WidgetStyle::Child)
		{
			MYGUI_ASSERT(mCroppedParent, "must be cropped");
			MYGUI_ASSERT(mParent, "must be parent");
		}
		else if (mWidgetStyle == WidgetStyle::Overlapped)
		{
			MYGUI_ASSERT((mParent == nullptr) == (mCroppedParent == nullptr), "error cropped");
		}
		else if (mWidgetStyle == WidgetStyle::Popup)
		{
			MYGUI_ASSERT(!mCroppedParent, "cropped must be nullptr");
			MYGUI_ASSERT(mParent, "must be parent");
		}
#endif

		// корректируем абсолютные координаты
		mAbsolutePosition = _coord.point();

		if (nullptr != mCroppedParent)
			mAbsolutePosition += mCroppedParent->getAbsolutePosition();

		const WidgetInfo* root = initialiseWidgetSkinBase(skinInfo, templateInfo);

		// дочернее окно обыкновенное
		if (mWidgetStyle == WidgetStyle::Child)
		{
			if (mParent)
				mParent->addChildItem(this);
		}
		// дочернее нуно перекрывающееся
		else if (mWidgetStyle == WidgetStyle::Overlapped)
		{
			// дочернее перекрывающееся
			if (mParent)
				mParent->addChildNode(this);
		}

		// витр метод для наследников
		initialiseOverride();

		if (skinInfo != nullptr)
			setSkinProperty(skinInfo);

		if (root != nullptr)
		{
			for (VectorStringPairs::const_iterator iter = root->properties.begin(); iter != root->properties.end(); ++iter)
			{
				setProperty(iter->first, iter->second);
			}
		}
	}

	void Widget::_shutdown()
	{
		// витр метод для наследников
		shutdownOverride();

		shutdownWidgetSkinBase();

		_destroyAllChildWidget();

		// дочернее окно обыкновенное
		if (mWidgetStyle == WidgetStyle::Child)
		{
			if (mParent)
				mParent->removeChildItem(this);
		}
		// дочернее нуно перекрывающееся
		else if (mWidgetStyle == WidgetStyle::Overlapped)
		{
			// дочернее перекрывающееся
			if (mParent)
				mParent->removeChildNode(this);
		}

		mParent = nullptr;
		mCroppedParent = nullptr;
	}

	void Widget::changeWidgetSkin(const std::string& _skinName)
	{
		ResourceSkin* skinInfo = nullptr;
		ResourceLayout* templateInfo = nullptr;

		if (LayoutManager::getInstance().isExist(_skinName))
			templateInfo = LayoutManager::getInstance().getByName(_skinName);
		else
			skinInfo = SkinManager::getInstance().getByName(_skinName);

		shutdownOverride();

		saveLayerItem();

		shutdownWidgetSkinBase();
		const WidgetInfo* root = initialiseWidgetSkinBase(skinInfo, templateInfo);

		restoreLayerItem();

		initialiseOverride();

		if (skinInfo != nullptr)
			setSkinProperty(skinInfo);

		if (root != nullptr)
		{
			for (VectorStringPairs::const_iterator iter = root->properties.begin(); iter != root->properties.end(); ++iter)
			{
				setProperty(iter->first, iter->second);
			}
		}
	}

	const WidgetInfo* Widget::initialiseWidgetSkinBase(ResourceSkin* _skinInfo, ResourceLayout* _templateInfo)
	{
		const WidgetInfo* root  = nullptr;
		bool skinOnly = false;

		if (_skinInfo == nullptr)
		{
			skinOnly = true;
			std::string skinName;

			const VectorWidgetInfo& data = _templateInfo->getLayoutData();
			for (VectorWidgetInfo::const_iterator item = data.begin(); item != data.end(); ++item)
			{
				if ((*item).name == "Root")
				{
					skinName = (*item).skin;
					root = &(*item);
					break;
				}
			}

			_skinInfo = SkinManager::getInstance().getByName(skinName);
		}

		//SAVE
		const IntSize& _size = mCoord.size();

		//FIXME - явный вызов
		Widget::setSize(_skinInfo->getSize());

		_createSkinItem(_skinInfo);

		// выставляем альфу, корректировка по отцу автоматически
		_updateAlpha();
		_updateEnabled();
		_updateVisible();

		if (!skinOnly)
		{
			const MapString& properties = _skinInfo->getProperties();
			for (MapString::const_iterator item = properties.begin(); item != properties.end(); ++item)
			{
				if (BackwardCompatibility::isIgnoreProperty((*item).first))
					setUserString((*item).first, (*item).second);
			}

			// создаем детей скина
			const VectorChildSkinInfo& child = _skinInfo->getChild();
			for (VectorChildSkinInfo::const_iterator iter = child.begin(); iter != child.end(); ++iter)
			{
				Widget* widget = baseCreateWidget(iter->style, iter->type, iter->skin, iter->coord, iter->align, iter->layer, iter->name, true);
				// заполняем UserString пропертями
				for (MapString::const_iterator prop = iter->params.begin(); prop != iter->params.end(); ++prop)
					widget->setUserString(prop->first, prop->second);
			}
		}

		if (root != nullptr)
		{
			//FIXME - явный вызов
			Widget::setSize(root->intCoord.size());

			for (MapString::const_iterator iter = root->userStrings.begin(); iter != root->userStrings.end(); ++iter)
			{
				setUserString(iter->first, iter->second);
			}

			for (VectorWidgetInfo::const_iterator iter = root->childWidgetsInfo.begin(); iter != root->childWidgetsInfo.end(); ++iter)
			{
				_templateInfo->createWidget(*iter, "", this, true);
			}
		}

		//FIXME - явный вызов
		Widget::setSize(_size);

		return root;
	}

	void Widget::shutdownWidgetSkinBase()
	{
		setMaskPick("");

		_deleteSkinItem();

		// удаляем виджеты чтобы ли в скине
		for (VectorWidgetPtr::iterator iter = mWidgetChildSkin.begin(); iter != mWidgetChildSkin.end(); ++iter)
		{
			// Добавляем себя чтобы удалилось
			mWidgetChild.push_back(*iter);
			_destroyChildWidget(*iter);
		}
		mWidgetChildSkin.clear();

		mWidgetClient = nullptr;
	}

	Widget* Widget::baseCreateWidget(WidgetStyle _style, const std::string& _type, const std::string& _skin, const IntCoord& _coord, Align _align, const std::string& _layer, const std::string& _name, bool _template)
	{
		Widget* widget = nullptr;

		if (_template)
		{
			widget = WidgetManager::getInstance().createWidget(_style, _type, _skin, _coord, this, _style == WidgetStyle::Popup ? nullptr : this, _name);
			mWidgetChildSkin.push_back(widget);
		}
		else
		{
			if (mWidgetClient != nullptr)
			{
				widget = mWidgetClient->baseCreateWidget(_style, _type, _skin, _coord, _align, _layer, _name, _template);
				onWidgetCreated(widget);
				return widget;
			}
			else
			{
				widget = WidgetManager::getInstance().createWidget(_style, _type, _skin, _coord, this, _style == WidgetStyle::Popup ? nullptr : this, _name);
				mWidgetChild.push_back(widget);
			}
		}

		widget->setAlign(_align);

		// присоединяем виджет с уровню
		if (!_layer.empty() && widget->isRootWidget())
			LayerManager::getInstance().attachToLayerNode(_layer, widget);

		onWidgetCreated(widget);

		return widget;
	}

	Widget* Widget::createWidgetRealT(const std::string& _type, const std::string& _skin, const FloatCoord& _coord, Align _align, const std::string& _name)
	{
		return createWidgetT(_type, _skin, CoordConverter::convertFromRelative(_coord, getSize()), _align, _name);
	}

	void Widget::_updateView()
	{
		bool margin = mCroppedParent ? _checkMargin() : false;

		// вьюпорт стал битым
		if (margin)
		{
			// проверка на полный выход за границу
			if (_checkOutside())
			{
				// запоминаем текущее состояние
				mIsMargin = margin;

				// скрываем
				_setSubSkinVisible(false);

				// вся иерархия должна быть проверенна
				for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
					(*widget)->_updateView();
				for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
					(*widget)->_updateView();

				return;
			}

		}
		// мы не обрезаны и были нормальные
		else if (!mIsMargin)
		{
			_updateSkinItemView();
			return;
		}

		// запоминаем текущее состояние
		mIsMargin = margin;

		// если скин был скрыт, то покажем
		_setSubSkinVisible(true);

		// обновляем наших детей, а они уже решат обновлять ли своих детей
		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateView();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateView();

		_updateSkinItemView();
	}

	bool Widget::setState(const std::string& _state)
	{
		return _setSkinItemState(_state);
	}

	void Widget::_destroyChildWidget(Widget* _widget)
	{
		MYGUI_ASSERT(nullptr != _widget, "invalid widget pointer");

		VectorWidgetPtr::iterator iter = std::find(mWidgetChild.begin(), mWidgetChild.end(), _widget);
		if (iter != mWidgetChild.end())
		{
			// сохраняем указатель
			MyGUI::Widget* widget = *iter;

			// удаляем из списка
			*iter = mWidgetChild.back();
			mWidgetChild.pop_back();

			// отписываем от всех
			WidgetManager::getInstance().unlinkFromUnlinkers(_widget);

			// непосредственное удаление
			WidgetManager::getInstance()._deleteWidget(widget);
		}
		else
		{
			MYGUI_EXCEPT("Widget '" << _widget->getName() << "' not found");
		}
	}

	// удаляет всех детей
	void Widget::_destroyAllChildWidget()
	{
		WidgetManager& manager = WidgetManager::getInstance();
		while (!mWidgetChild.empty())
		{
			// сразу себя отписывем, иначе вложенной удаление убивает все
			Widget* widget = mWidgetChild.back();
			mWidgetChild.pop_back();

			// отписываем от всех
			manager.unlinkFromUnlinkers(widget);

			// и сами удалим, так как его больше в списке нет
			WidgetManager::getInstance()._deleteWidget(widget);
		}
	}

	IntCoord Widget::getClientCoord()
	{
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			return mWidgetClient->getCoord();
		return IntCoord(0, 0, mCoord.width, mCoord.height);
	}

	void Widget::setAlpha(float _alpha)
	{
		if (mAlpha == _alpha)
			return;
		mAlpha = _alpha;

		_updateAlpha();
	}

	void Widget::_updateAlpha()
	{
		if (nullptr != mParent)
			mRealAlpha = mAlpha * (mInheritsAlpha ? mParent->_getRealAlpha() : ALPHA_MAX);
		else
			mRealAlpha = mAlpha;

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateAlpha();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateAlpha();

		_setSkinItemAlpha(mRealAlpha);
	}

	void Widget::setInheritsAlpha(bool _inherits)
	{
		mInheritsAlpha = _inherits;
		_updateAlpha();
	}

	ILayerItem* Widget::getLayerItemByPoint(int _left, int _top) const
	{
		// проверяем попадание
		if (!mEnabled
			|| !mVisible
			|| (!getNeedMouseFocus() && !getInheritsPick())
			|| !_checkPoint(_left, _top)
			// если есть маска, проверяем еще и по маске
			|| isMaskPickInside(IntPoint(_left - mCoord.left, _top - mCoord.top), mCoord)
			)
			return nullptr;

		// спрашиваем у детишек
		for (VectorWidgetPtr::const_reverse_iterator widget = mWidgetChild.rbegin(); widget != mWidgetChild.rend(); ++widget)
		{
			// общаемся только с послушными детьми
			if ((*widget)->mWidgetStyle == WidgetStyle::Popup)
				continue;

			ILayerItem* item = (*widget)->getLayerItemByPoint(_left - mCoord.left, _top - mCoord.top);
			if (item != nullptr)
				return item;
		}
		// спрашиваем у детишек скна
		for (VectorWidgetPtr::const_reverse_iterator widget = mWidgetChildSkin.rbegin(); widget != mWidgetChildSkin.rend(); ++widget)
		{
			ILayerItem* item = (*widget)->getLayerItemByPoint(_left - mCoord.left, _top - mCoord.top);
			if (item != nullptr)
				return item;
		}

		// непослушные дети
		return getInheritsPick() ? nullptr : (ILayerItem*)(this);
	}

	void Widget::_updateAbsolutePoint()
	{
		// мы рут, нам не надо
		if (!mCroppedParent)
			return;

		mAbsolutePosition = mCroppedParent->getAbsolutePosition() + mCoord.point();

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateAbsolutePoint();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateAbsolutePoint();

		_correctSkinItemView();
	}

	void Widget::_forcePeek(Widget* _widget)
	{
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			mWidgetClient->_forcePeek(_widget);

		VectorWidgetPtr::iterator item = std::remove(mWidgetChild.begin(), mWidgetChild.end(), _widget);
		if (item != mWidgetChild.end())
		{
			mWidgetChild.erase(item);
			mWidgetChild.insert(mWidgetChild.begin(), _widget);
		}
	}

	Widget* Widget::findWidget(const std::string& _name)
	{
		if (_name == mName)
			return this;
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			return mWidgetClient->findWidget(_name);

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
		{
			Widget* find = (*widget)->findWidget(_name);
			if (nullptr != find)
				return find;
		}
		return nullptr;
	}

	void Widget::setRealPosition(const FloatPoint& _point)
	{
		setPosition(CoordConverter::convertFromRelative(_point, mCroppedParent == nullptr ? RenderManager::getInstance().getViewSize() : mCroppedParent->getSize()));
	}

	void Widget::setRealSize(const FloatSize& _size)
	{
		setSize(CoordConverter::convertFromRelative(_size, mCroppedParent == nullptr ? RenderManager::getInstance().getViewSize() : mCroppedParent->getSize()));
	}

	void Widget::setRealCoord(const FloatCoord& _coord)
	{
		setCoord(CoordConverter::convertFromRelative(_coord, mCroppedParent == nullptr ? RenderManager::getInstance().getViewSize() : mCroppedParent->getSize()));
	}

	void Widget::_setAlign(const IntSize& _oldsize)
	{
		const IntSize& size = getParentSize();

		bool need_move = false;
		bool need_size = false;
		IntCoord coord = mCoord;

		// первоначальное выравнивание
		if (mAlign.isHStretch())
		{
			// растягиваем
			coord.width = mCoord.width + (size.width - _oldsize.width);
			need_size = true;
		}
		else if (mAlign.isRight())
		{
			// двигаем по правому краю
			coord.left = mCoord.left + (size.width - _oldsize.width);
			need_move = true;
		}
		else if (mAlign.isHCenter())
		{
			// выравнивание по горизонтали без растяжения
			coord.left = (size.width - mCoord.width) / 2;
			need_move = true;
		}

		if (mAlign.isVStretch())
		{
			// растягиваем
			coord.height = mCoord.height + (size.height - _oldsize.height);
			need_size = true;
		}
		else if (mAlign.isBottom())
		{
			// двигаем по нижнему краю
			coord.top = mCoord.top + (size.height - _oldsize.height);
			need_move = true;
		}
		else if (mAlign.isVCenter())
		{
			// выравнивание по вертикали без растяжения
			coord.top = (size.height - mCoord.height) / 2;
			need_move = true;
		}

		if (need_move)
		{
			if (need_size)
				setCoord(coord);
			else
				setPosition(coord.point());
		}
		else if (need_size)
		{
			setSize(coord.size());
		}
		else
		{
			_updateView(); // только если не вызвано передвижение и сайз
		}
	}

	void Widget::setPosition(const IntPoint& _point)
	{
		// обновляем абсолютные координаты
		mAbsolutePosition += _point - mCoord.point();

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateAbsolutePoint();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateAbsolutePoint();

		mCoord = _point;

		_updateView();
	}

	void Widget::setSize(const IntSize& _size)
	{
		// устанавливаем новую координату а старую пускаем в расчеты
		IntSize old = mCoord.size();
		mCoord = _size;

		bool visible = true;

		// обновляем выравнивание
		bool margin = mCroppedParent ? _checkMargin() : false;

		if (margin)
		{
			// проверка на полный выход за границу
			if (_checkOutside())
			{
				// скрываем
				visible = false;
			}
		}

		_setSubSkinVisible(visible);

		// передаем старую координату , до вызова, текущая координата отца должна быть новой
		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_setAlign(old);
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_setAlign(old);

		_setSkinItemAlign(old);

		// запоминаем текущее состояние
		mIsMargin = margin;
	}

	void Widget::setCoord(const IntCoord& _coord)
	{
		// обновляем абсолютные координаты
		mAbsolutePosition += _coord.point() - mCoord.point();

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateAbsolutePoint();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateAbsolutePoint();

		// устанавливаем новую координату а старую пускаем в расчеты
		IntCoord old = mCoord;
		mCoord = _coord;

		bool visible = true;

		// обновляем выравнивание
		bool margin = mCroppedParent ? _checkMargin() : false;

		if (margin)
		{
			// проверка на полный выход за границу
			if (_checkOutside())
			{
				// скрываем
				visible = false;
			}
		}

		_setSubSkinVisible(visible);

		// передаем старую координату , до вызова, текущая координата отца должна быть новой
		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_setAlign(old.size());
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_setAlign(old.size());

		_setSkinItemAlign(old.size());

		// запоминаем текущее состояние
		mIsMargin = margin;
	}

	void Widget::setAlign(Align _value)
	{
		mAlign = _value;
	}

	void Widget::detachFromWidget(const std::string& _layer)
	{
		std::string oldlayer = getLayer() != nullptr ? getLayer()->getName() : "";

		Widget* parent = getParent();
		if (parent)
		{
			// отдетачиваемся от лееров
			if ( ! isRootWidget() )
			{
				detachFromLayerItemNode(true);

				if (mWidgetStyle == WidgetStyle::Child)
				{
					mParent->removeChildItem(this);
				}
				else if (mWidgetStyle == WidgetStyle::Overlapped)
				{
					mParent->removeChildNode(this);
				}

				mWidgetStyle = WidgetStyle::Overlapped;

				mCroppedParent = nullptr;

				// обновляем координаты
				mAbsolutePosition = mCoord.point();

				for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
					(*widget)->_updateAbsolutePoint();
				for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
					(*widget)->_updateAbsolutePoint();

				// сбрасываем обрезку
				mMargin.clear();

				_updateView();
			}

			// нам нужен самый рутовый парент
			while (parent->getParent())
				parent = parent->getParent();

			//mIWidgetCreator = parent->mIWidgetCreator;
			//mIWidgetCreator->_linkChildWidget(this);
			Gui::getInstance()._linkChildWidget(this);
			mParent->_unlinkChildWidget(this);
			mParent = nullptr;
		}

		if (!_layer.empty())
		{
			LayerManager::getInstance().attachToLayerNode(_layer, this);
		}
		else if (!oldlayer.empty())
		{
			LayerManager::getInstance().attachToLayerNode(oldlayer, this);
		}

		_updateAlpha();
	}

	void Widget::attachToWidget(Widget* _parent, WidgetStyle _style, const std::string& _layer)
	{
		MYGUI_ASSERT(_parent, "parent must be valid");
		MYGUI_ASSERT(_parent != this, "cyclic attach (attaching to self)");

		// attach to client if widget have it
		if (_parent->getClientWidget())
			_parent = _parent->getClientWidget();

		// проверяем на цикличность атача
		Widget* parent = _parent;
		while (parent->getParent())
		{
			MYGUI_ASSERT(parent != this, "cyclic attach");
			parent = parent->getParent();
		}

		// отдетачиваемся от всего
		detachFromWidget();

		mWidgetStyle = _style;

		if (_style == WidgetStyle::Popup)
		{
			//mIWidgetCreator->_unlinkChildWidget(this);
			//mIWidgetCreator = _parent;
			if (mParent == nullptr)
				Gui::getInstance()._unlinkChildWidget(this);
			else
				mParent->_unlinkChildWidget(this);

			mParent = _parent;
			mParent->_linkChildWidget(this);

			mCroppedParent = nullptr;

			if (!_layer.empty())
			{
				LayerManager::getInstance().attachToLayerNode(_layer, this);
			}
		}
		else if (_style == WidgetStyle::Child)
		{
			LayerManager::getInstance().detachFromLayer(this);

			//mIWidgetCreator->_unlinkChildWidget(this);
			//mIWidgetCreator = _parent;
			if (mParent == nullptr)
				Gui::getInstance()._unlinkChildWidget(this);
			else
				mParent->_unlinkChildWidget(this);

			mParent = _parent;
			mParent->_linkChildWidget(this);

			mCroppedParent = _parent;
			mAbsolutePosition = _parent->getAbsolutePosition() + mCoord.point();

			for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
				(*widget)->_updateAbsolutePoint();
			for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
				(*widget)->_updateAbsolutePoint();

			mParent->addChildItem(this);

			_updateView();
		}
		else if (_style == WidgetStyle::Overlapped)
		{
			LayerManager::getInstance().detachFromLayer(this);

			//mIWidgetCreator->_unlinkChildWidget(this);
			//mIWidgetCreator = _parent;
			if (mParent == nullptr)
				Gui::getInstance()._unlinkChildWidget(this);
			else
				mParent->_unlinkChildWidget(this);

			mParent = _parent;
			mParent->_linkChildWidget(this);

			mCroppedParent = _parent;
			mAbsolutePosition = _parent->getAbsolutePosition() + mCoord.point();

			for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
				(*widget)->_updateAbsolutePoint();
			for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
				(*widget)->_updateAbsolutePoint();

			mParent->addChildNode(this);

			_updateView();
		}

		_updateAlpha();
	}

	void Widget::setWidgetStyle(WidgetStyle _style, const std::string& _layer)
	{
		if (_style == mWidgetStyle)
			return;
		if (nullptr == getParent())
			return;

		Widget* parent = mParent;

		detachFromWidget();
		attachToWidget(parent, _style, _layer);
		// ищем леер к которому мы присоедененны
	}

	Widget* Widget::createWidgetT(const std::string& _type, const std::string& _skin, const IntCoord& _coord, Align _align, const std::string& _name)
	{
		return baseCreateWidget(WidgetStyle::Child, _type, _skin, _coord, _align, "", _name, false);
	}

	Widget* Widget::createWidgetT(const std::string& _type, const std::string& _skin, int _left, int _top, int _width, int _height, Align _align, const std::string& _name)
	{
		return createWidgetT(_type, _skin, IntCoord(_left, _top, _width, _height), _align, _name);
	}

	Widget* Widget::createWidgetRealT(const std::string& _type, const std::string& _skin, float _left, float _top, float _width, float _height, Align _align, const std::string& _name)
	{
		return createWidgetRealT(_type, _skin, FloatCoord(_left, _top, _width, _height), _align, _name);
	}

	Widget* Widget::createWidgetT(WidgetStyle _style, const std::string& _type, const std::string& _skin, const IntCoord& _coord, Align _align, const std::string& _layer, const std::string& _name)
	{
		return baseCreateWidget(_style, _type, _skin, _coord, _align, _layer, _name, false);
	}

	EnumeratorWidgetPtr Widget::getEnumerator() const
	{
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			return mWidgetClient->getEnumerator();
		return Enumerator<VectorWidgetPtr>(mWidgetChild.begin(), mWidgetChild.end());
	}

	size_t Widget::getChildCount()
	{
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			return mWidgetClient->getChildCount();
		return mWidgetChild.size();
	}

	Widget* Widget::getChildAt(size_t _index)
	{
		MYGUI_ASSERT(mWidgetClient != this, "mWidgetClient can not be this widget");
		if (mWidgetClient != nullptr)
			return mWidgetClient->getChildAt(_index);
		MYGUI_ASSERT_RANGE(_index, mWidgetChild.size(), "Widget::getChildAt");
		return mWidgetChild[_index];
	}

	void Widget::baseUpdateEnable()
	{
		if (mEnabled)
			setState("normal");
		else
			setState("disabled");
	}

	void Widget::setVisible(bool _value)
	{
		if (mVisible == _value)
			return;
		mVisible = _value;

		_updateVisible();

		/*if (mInheritsVisible)
		{
			_setSkinItemVisible(_value);

			for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
				(*widget)->_setInheritsVisible(_value);
			for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
				(*widget)->_setInheritsVisible(_value);
		}*/
	}

	void Widget::_updateVisible()
	{
		mInheritsVisible = mParent == nullptr || (mParent->getVisible() && mParent->_isInheritsVisible());
		bool value = mVisible && mInheritsVisible;

		_setSkinItemVisible(value);

		for (VectorWidgetPtr::iterator widget = mWidgetChild.begin(); widget != mWidgetChild.end(); ++widget)
			(*widget)->_updateVisible();
		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->_updateVisible();
	}

	void Widget::setEnabled(bool _value)
	{
		if (mEnabled == _value)
			return;
		mEnabled = _value;

		_updateEnabled();
	}

	void Widget::_updateEnabled()
	{
		mInheritsEnabled = mParent == nullptr || (mParent->getEnabled() && mParent->_isInheritsEnable());
		bool value = mEnabled && mInheritsEnabled;

		for (VectorWidgetPtr::iterator iter = mWidgetChild.begin(); iter != mWidgetChild.end(); ++iter)
			(*iter)->_updateEnabled();
		for (VectorWidgetPtr::iterator iter = mWidgetChildSkin.begin(); iter != mWidgetChildSkin.end(); ++iter)
			(*iter)->_updateEnabled();

		baseUpdateEnable();

		if (!value)
			InputManager::getInstance().unlinkWidget(this);
	}

	void Widget::setColour(const Colour& _value)
	{
		_setSkinItemColour(_value);

		for (VectorWidgetPtr::iterator widget = mWidgetChildSkin.begin(); widget != mWidgetChildSkin.end(); ++widget)
			(*widget)->setColour(_value);
	}

	IntSize Widget::getParentSize() const
	{
		if (mCroppedParent)
			return static_cast<Widget*>(mCroppedParent)->getSize();
		if (getLayer())
			return getLayer()->getSize();

		return RenderManager::getInstance().getViewSize();
	}

	void Widget::_resetContainer(bool _updateOnly)
	{
		if (getNeedToolTip())
			ToolTipManager::getInstance()._unlinkWidget(this);
	}

	bool Widget::_checkPoint(int _left, int _top) const
	{
		return ! ((_getViewLeft() > _left) || (_getViewTop() > _top) || (_getViewRight() < _left) || (_getViewBottom() < _top));
	}

	void Widget::_linkChildWidget(Widget* _widget)
	{
		VectorWidgetPtr::iterator iter = std::find(mWidgetChild.begin(), mWidgetChild.end(), _widget);
		MYGUI_ASSERT(iter == mWidgetChild.end(), "widget already exist");
		mWidgetChild.push_back(_widget);
	}

	void Widget::_unlinkChildWidget(Widget* _widget)
	{
		VectorWidgetPtr::iterator iter = std::remove(mWidgetChild.begin(), mWidgetChild.end(), _widget);
		MYGUI_ASSERT(iter != mWidgetChild.end(), "widget not found");
		mWidgetChild.erase(iter);
	}

	void Widget::shutdownOverride()
	{
	}

	void Widget::initialiseOverride()
	{
	}

	void Widget::setSkinProperty(ResourceSkin* _info)
	{
		const MapString& properties = _info->getProperties();
		for (MapString::const_iterator item = properties.begin(); item != properties.end(); ++item)
			setProperty((*item).first, (*item).second);
	}

	void Widget::setProperty(const std::string& _key, const std::string& _value)
	{
		std::string key = _key;
		std::string value = _value;

		if (BackwardCompatibility::checkProperty(this, key, value))
		{
			size_t index = key.find("_");
			if (index != std::string::npos)
			{
				MYGUI_LOG(Warning, "Widget property '" << key << "' have type prefix" << " [" << LayoutManager::getInstance().getCurrentLayout() << "]");
				key = key.substr(index + 1);
			}

			setPropertyOverride(key, value);
		}
	}

	VectorWidgetPtr Widget::getSkinWidgetsByName(const std::string& _name)
	{
		VectorWidgetPtr result;

		for (VectorWidgetPtr::iterator iter = mWidgetChildSkin.begin(); iter != mWidgetChildSkin.end(); ++iter)
		{
			Widget* find = (*iter)->findWidget(_name);
			if (nullptr != find)
				result.push_back(find);
		}

		return result;
	}

	void Widget::destroySkinWidget(Widget* _widget)
	{
		mWidgetChild.push_back(_widget);
		WidgetManager::getInstance().destroyWidget(_widget);
	}

	void Widget::onWidgetCreated(Widget* _widget)
	{
	}

	void Widget::setWidgetClient(Widget* _widget)
	{
		mWidgetClient = _widget;
	}

	Widget* Widget::_createSkinWidget(WidgetStyle _style, const std::string& _type, const std::string& _skin, const IntCoord& _coord, Align _align, const std::string& _layer, const std::string& _name)
	{
		return baseCreateWidget(_style, _type, _skin, _coord, _align, _layer, _name, true);
	}

	void Widget::setPropertyOverride(const std::string& _key, const std::string& _value)
	{
		/// @wproperty{Widget, Widget_Position, IntPoint} Sets position
		if (_key == "Position") setPosition(utility::parseValue<IntPoint>(_value));
		else if (_key == "Size") setSize(utility::parseValue<IntSize>(_value));
		else if (_key == "Coord") setCoord(utility::parseValue<IntCoord>(_value));
		else if (_key == "Visible") setVisible(utility::parseValue<bool>(_value));
		else if (_key == "Alpha") setAlpha(utility::parseValue<float>(_value));
		else if (_key == "Colour") setColour(utility::parseValue<Colour>(_value));
		else if (_key == "InheritsAlpha") setInheritsAlpha(utility::parseValue<bool>(_value));
		else if (_key == "InheritsPick") setInheritsPick(utility::parseValue<bool>(_value));
		else if (_key == "MaskPick") setMaskPick(_value);
		else if (_key == "State") setState(_value);
		else if (_key == "NeedKey") setNeedKeyFocus(utility::parseValue<bool>(_value));
		else if (_key == "NeedMouse") setNeedMouseFocus(utility::parseValue<bool>(_value));
		else if (_key == "Enabled") setEnabled(utility::parseValue<bool>(_value));
		else if (_key == "NeedToolTip") setNeedToolTip(utility::parseValue<bool>(_value));
		else if (_key == "Pointer") setPointer(_value);
		else
		{
			MYGUI_LOG(Warning, "Widget property '" << _key << "' not found" << " [" << LayoutManager::getInstance().getCurrentLayout() << "]");
			return;
		}

		eventChangeProperty(this, _key, _value);
	}

} // namespace MyGUI
