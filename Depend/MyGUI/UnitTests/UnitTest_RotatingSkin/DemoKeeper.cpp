/*!
	@file
	@author		George Evmenov
	@date		02/2010
*/
#include "precompiled.h"
#include "DemoKeeper.h"
#include "Base/Main.h"

namespace demo
{

	MyGUI::StaticImage* image;
	MyGUI::RotatingSkin* rotato;

	void DemoKeeper::setupResources()
	{
		base::BaseManager::setupResources();
		addResourceLocation(getRootMedia() + "/Common/Wallpapers");
	}

	void DemoKeeper::createScene()
	{
		const MyGUI::VectorWidgetPtr& root = MyGUI::LayoutManager::getInstance().loadLayout("BackHelp.layout");
		root.at(0)->findWidget("Text")->castType<MyGUI::StaticText>()->setCaption("Move mouse to rotate skin. Right mouse click to set new center.");

		MyGUI::Window* window = getGUI()->createWidget<MyGUI::Window>("WindowCSX", MyGUI::IntCoord(100, 100, 400, 400), MyGUI::Align::Default, "Main");

		image = window->createWidget<MyGUI::StaticImage>("RotatingSkin", MyGUI::IntCoord(150, 150, 100, 150), MyGUI::Align::Default/*, "Main"*/);
		image->setImageTexture("wallpaper0.png");

		MyGUI::ISubWidget* main = image->getSubWidgetMain();
		rotato = main->castType<MyGUI::RotatingSkin>();
	}

	void DemoKeeper::injectMousePress(int _absx, int _absy, MyGUI::MouseButton _id)
	{
		if (_id == MyGUI::MouseButton::Right)
			rotato->setCenter(MyGUI::IntPoint(_absx, _absy) - image->getAbsolutePosition());

		base::BaseManager::injectMousePress(_absx, _absy, _id);
	}

	void DemoKeeper::injectMouseMove(int _absx, int _absy, int _absz)
	{
		rotato->setAngle(atan2((float)_absx - rotato->getCenter(false).left, -(float)_absy + rotato->getCenter(false).top));

		base::BaseManager::injectMouseMove(_absx, _absy, _absz);
	}

} // namespace demo

MYGUI_APP(demo::DemoKeeper)
