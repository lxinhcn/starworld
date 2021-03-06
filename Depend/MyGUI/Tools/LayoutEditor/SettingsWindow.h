/*!
	@file
	@author		Georgiy Evmenov
	@date		09/2008
*/
#ifndef __SETTINGS_WINDOW_H__
#define __SETTINGS_WINDOW_H__

#include "BaseLayout/BaseLayout.h"
#include "Dialog.h"
#include "OpenSaveFileDialog.h"

namespace tools
{
	class SettingsWindow :
		public Dialog
	{
	public:
		SettingsWindow();
		virtual ~SettingsWindow();

		void loadSettings();
		void saveSettings();

	protected:
		virtual void onDoModal();
		virtual void onEndModal();

	private:
		int getGridStep();
		void setGridStep();

		bool getShowName();
		void setShowName(bool _pressed);
		bool getShowType();
		void setShowType(bool _pressed);
		bool getShowSkin();
		void setShowSkin(bool _pressed);

		void notifyNewGridStep(MyGUI::Widget* _sender, MyGUI::Widget* _new = 0);
		void notifyNewGridStepAccept(MyGUI::Edit* _sender); // calls previous method
		void notifyOkSettings(MyGUI::Widget* _sender);
		void notifyCancel(MyGUI::Widget* _sender);
		void notifyToggleCheck(MyGUI::Widget* _sender);
		void notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _name);

		void notifyClickResourcePathAdd(MyGUI::Widget* _sender);
		void notifyClickResourcePathDelete(MyGUI::Widget* _sender);

		void notifyEndDialogOpenSaveFile(Dialog* _sender, bool _result);

	private:
		MyGUI::Edit* mGridEdit;
		MyGUI::Button* mButtonOkSettings;
		MyGUI::Button* mButtonCancel;
		MyGUI::Button* mCheckShowName;
		MyGUI::Button* mCheckShowType;
		MyGUI::Button* mCheckShowSkin;
		MyGUI::Button* mResourcePathAdd;
		MyGUI::Button* mResourcePathDelete;
		MyGUI::List* mResourcePaths;
		MyGUI::ComboBox* mLayoutVersion;

		OpenSaveFileDialog* mOpenSaveFileDialog;

		int mGridStep;
	};

} // namespace tools

#endif // __SETTINGS_WINDOW_H__
