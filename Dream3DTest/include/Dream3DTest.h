#ifndef DREAM3DTEST_H
#define DREAM3DTEST_H

#include "Engine/EngineManager.h"
#include "Engine/Scene.h"
#include "Engine/Camera.h"
#include "Engine/Node.h"

class Dream3DTest : EngineManager
{
	public:
		Dream3DTest();
		virtual ~Dream3DTest();
		
		virtual void initialize();
		virtual void update(float elapsedTime);
		virtual void render(float elapsedTime);

		void render3D(float deltaTimeMs);
		void render2D(float deltaTimeMs);

		virtual void			keyPressedEx(unsigned int iVirtualKeycode, unsigned short ch);
		virtual void			keyReleasedEx(unsigned int iVirtualKeycode, unsigned short ch);

		virtual void			onMouseDownEx(int mCode, int x, int y);
		virtual void			onMouseMoveEx(int mCode, int x, int y);
		virtual void			onMouseUpEx(int mCode, int x, int y);
		virtual void			onMouseWheelEx(WPARAM wParam, LPARAM lParam);

	private:
		Scene*		m_pScene;
};

#endif


