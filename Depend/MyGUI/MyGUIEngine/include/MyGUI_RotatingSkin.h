/*!
	@file
	@author		George Evmenov
	@date		05/2009
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
#ifndef __MYGUI_ROTATING_SKIN_H__
#define __MYGUI_ROTATING_SKIN_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_Types.h"
#include "MyGUI_ISubWidgetRect.h"
#include "MyGUI_RenderFormat.h"

namespace MyGUI
{

	class MYGUI_EXPORT RotatingSkin : public ISubWidgetRect
	{
		MYGUI_RTTI_DERIVED( RotatingSkin )

	public:
		RotatingSkin();
		virtual ~RotatingSkin();

		/** Set angle of rotation in radians */
		void setAngle(float _angle);
		/** Get angle of rotation in radians */
		float getAngle() const
		{
			return mAngle;
		}

		/** Set center of rotation
			@param _center Center point.
			@param _local If true - _center point calculated as point on SubWidget, else calculated as point on screen.
		*/
		void setCenter(const IntPoint& _center);
		/** Get center of rotation */
		IntPoint getCenter(bool _local = true) const;

		virtual void setAlpha(float _alpha);

		virtual void setVisible(bool _visible);

		virtual void setStateData(IStateInfo* _data);

		virtual void createDrawItem(ITexture* _texture, ILayerNode* _node);
		virtual void destroyDrawItem();

		// ����� ��� ��������� ����
		virtual void doRender();

	/*internal:*/
		virtual void _updateView();
		virtual void _correctView();

		virtual void _setAlign(const IntSize& _oldsize/*, bool _update*/);

		virtual void _setUVSet(const FloatRect& _rect);
		virtual void _setColour(const Colour& _value);
	protected:
		void _rebuildGeometry();

		size_t _cropRotatedRectangle(FloatPoint* _baseVerticiesPos);
		enum Side {Left, Right, Top, Bottom};
		void _cropRotatedRectangleSide(std::vector<FloatPoint>& _verticies, int _sideCoord, Side _side);

		// get point position relative to rectangle
		FloatPoint _getPositionInsideRect(const FloatPoint& _point, const FloatPoint& _corner0, const FloatPoint& _corner1, const FloatPoint& _corner2);
	private:
		bool mGeometryOutdated;

		float mAngle;
		IntPoint mCenterPos;

		enum {RECT_VERTICIES_COUNT = 4, GEOMETRY_VERTICIES_TOTAL_COUNT = 8};
		FloatPoint mResultVerticiesPos[GEOMETRY_VERTICIES_TOTAL_COUNT];
		FloatPoint mResultVerticiesUV[GEOMETRY_VERTICIES_TOTAL_COUNT];

		// common variables (same in SubSkin and TileRect)
		//FloatRect mRectTexture;
		bool mEmptyView;

		VertexColourType mVertexFormat;
		uint32 mCurrentColour;

		FloatRect mCurrentTexture;
		IntCoord mCurrentCoord;

		ILayerNode* mNode;
		RenderItem* mRenderItem;
	};

} // namespace MyGUI

#endif // __MYGUI_ROTATING_SKIN_H__
