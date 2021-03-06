/*!
	@file
	@author		Albert Semenov
	@date		08/2008
	@module
*/

#ifndef __STATISTIC_INFO_H__
#define __STATISTIC_INFO_H__

#include <MyGUI.h>

namespace diagnostic
{

	class StatisticInfo
	{

	public:
		typedef std::pair<std::string, std::string> PairString;
		typedef std::vector<PairString> VectorPairString;

		StatisticInfo() :
			mInfo(nullptr)
		{
			const std::string layer = "Statistic";
			if ( ! MyGUI::LayerManager::getInstance().isExist(layer)) return;

			const std::string skin = "StatisticRect";
			if ( ! MyGUI::SkinManager::getInstance().isExist(skin)) return;

			mInfo = MyGUI::Gui::getInstance().createWidget<MyGUI::StaticText>(skin, MyGUI::IntCoord(), MyGUI::Align::Default, layer);
		}

		~StatisticInfo()
		{
			if (mInfo)
			{
				MyGUI::Gui::getInstance().destroyChildWidget(mInfo);
				mInfo = nullptr;
			}
		}

		template <typename T>
		void change(const std::string& _key, const T& _value)
		{
			for (VectorPairString::iterator iter = mParams.begin(); iter != mParams.end(); ++iter)
			{
				if (iter->first == _key)
				{
					iter->second = MyGUI::utility::toString<T>(_value);
					return;
				}
			}
			mParams.push_back(std::make_pair(_key, MyGUI::utility::toString<T>(_value)));
		}

		void remove(const std::string& _key)
		{
			for (VectorPairString::iterator iter = mParams.begin(); iter != mParams.end(); ++iter)
			{
				if (iter->first == _key)
				{
					mParams.erase(iter);
					return;
				}
			}
		}

		void update()
		{
			if (mInfo)
			{
				std::ostringstream stream;
				for (VectorPairString::iterator iter = mParams.begin(); iter != mParams.end(); ++iter)
				{
					if (iter != mParams.begin()) stream << "\n";
					stream << iter->first << " : " << iter->second;
				}

				mInfo->setCaption(stream.str());

				MyGUI::ISubWidgetText* text = mInfo->getSubWidgetText();
				if (text)
				{
					const MyGUI::IntSize& size = text->getTextSize() + mInfo->getSize() - text->getSize();
					const MyGUI::IntSize& size_view = MyGUI::RenderManager::getInstance().getViewSize();
					mInfo->setCoord(size_view.width - size.width - 20, size_view.height - size.height - 20, size.width, size.height);
				}
			}
		}

		void clear()
		{
			mParams.clear();
		}

		void clear(const std::string& _key)
		{
			for (VectorPairString::iterator iter = mParams.begin(); iter != mParams.end(); ++iter)
			{
				if (iter->first == _key)
				{
					mParams.erase(iter);
					return;
				}
			}
		}

		void setVisible(bool _value)
		{
			if (mInfo)
				mInfo->setVisible(_value);
		}

		bool getVisible()
		{
			if (mInfo)
				return mInfo->getVisible();
			return false;
		}

	private:
		MyGUI::StaticText* mInfo;
		VectorPairString mParams;

	};

} // namespace diagnostic

#endif // __STATISTIC_INFO_H__
