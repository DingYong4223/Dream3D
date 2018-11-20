#ifndef CAMERA_H
#define CAMERA_H

#include "Base.h"
#include "Common/Vectors.h"
#include "Common/Matrices.h"

class Camera {
	public:
		enum Type {
			PERSPECTIVE = 1,
			ORTHOGRAPHIC = 2
		};

		Camera(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane);
		Camera(int x, int y, int w, int h, float fNearPlane, float fFarPlane);
		~Camera();

		void		setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
		void		update(float deltaTimeMs);
		void		translate(Matrix4 &mat, Vector3 camPos);

		static Camera*	createPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane);
		void			setPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane);
		void			setFrustum(float fovY, float aspectRatio, float front, float back);
		void			setPerspectiveFrustum(float l, float r, float b, float t, float n, float f);	

		static Camera*	createOrthographic(int x, int y, int w, int h, float fNearPlane, float fFarPlane);
		void			setOrthographic(int x, int y, int w, int h, float fNearPlane, float fFarPlane);
		void			setOrthogonalFrustum(float l, float r, float b, float t, float n, float f);

		Matrix4&		getProjectionMatrix();
		Matrix4&		getViewProjectionMatrix();
		Matrix4&		getViewMatrix();

		void			handleKeyboard(float deltaTimeMs);
		void			handleMouse();
		void			moveForward(float distance);
		void			strafe(float distance);
		void			moveUpward(float distance);
		void			updateViewMatrix();

	private:
		Camera::Type	m_iCameraType;
		unsigned int	m_iViewX;
		unsigned int	m_iViewY;
		unsigned int	m_iViewW;
		unsigned int	m_iViewH;
		float			m_fFieldOfView;
		float			m_fAspectRatio;
		float			m_fNearPlane;
		float			m_fFarPlane;

		int				m_iDirty;
		Vector3			m_CameraPosition;
		Vector3			m_CameraAngle;

		Matrix4			m_MatrixView;
		Matrix4			m_MatrixProjection;
		Matrix4			m_MatrixViewProjection;
};

#endif