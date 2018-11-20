#include "Engine/Camera.h"
#include "Engine/EngineManager.h"

Camera::Camera(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane)
	:	m_iCameraType(PERSPECTIVE),
		m_iViewX(x),
		m_iViewY(y),
		m_iViewW(w),
		m_iViewH(h),
		m_fAspectRatio((float)(w)/h),
		m_fFieldOfView(iFieldOfView),
		m_fNearPlane(fNearPlane),
		m_fFarPlane(fFarPlane),

		m_iDirty(CAMERA_DIRTY_ALL),

		m_CameraPosition(),
		m_CameraAngle(),
		
		m_MatrixView(),
		m_MatrixProjection(),
		m_MatrixViewProjection(),

		m_pNode(NULL)
{
	
}

Camera::Camera(int x, int y, int w, int h, float fNearPlane, float fFarPlane)
	:	m_iCameraType(ORTHOGRAPHIC),
		m_iViewX(x),
		m_iViewY(y),
		m_iViewW(w),
		m_iViewH(h),
		m_fAspectRatio((float)(w)/h),
		m_fNearPlane(fNearPlane),
		m_fFarPlane(fFarPlane),
		
		m_iDirty(CAMERA_DIRTY_ALL),

		m_CameraPosition(),
		m_CameraAngle(),

		m_MatrixView(),
		m_MatrixProjection(),
		m_MatrixViewProjection(),

		m_pNode(NULL)
{
	m_iViewX = x;
	m_iViewY = y;
	m_iViewW = w;
	m_iViewH = h;
	m_fAspectRatio = (float)(w)/h;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;
}

Camera::~Camera() {

}

Camera* Camera::createPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane) {
	Camera* cam = new Camera(x, y, w, h, iFieldOfView, fNearPlane, fFarPlane);
	cam->setPerspective(x, y, w, h, iFieldOfView, fNearPlane, fFarPlane);

	return cam;
}

Camera* Camera::createOrthographic(int x, int y, int w, int h, float fNearPlane, float fFarPlane) {
	Camera* cam = new Camera(x, y, w, h, fNearPlane, fFarPlane);
	cam->setOrthographic(x, y, w, h, fNearPlane, fFarPlane);
	
	return cam;
}

void Camera::setCameraValues(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane) {
	///////////////////// Set Camera Values
	m_iViewX = x;
	m_iViewY = y;
	m_iViewW = w;
	m_iViewH = h;
	m_fAspectRatio = (float)(w)/h;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;
	//////////////////////////////////////
}

void Camera::forceType(Camera::Type camType) {

	m_iCameraType = NONE;
	setType(camType);
}

void Camera::setType(Camera::Type camType) {
	
	if(m_iCameraType != camType) {
		setDirty(CAMERA_DIRTY_PROJ);
		m_iCameraType = camType;
	}

	if(m_iDirty & CAMERA_DIRTY_PROJ) {
		if(m_iCameraType == PERSPECTIVE) {
			setPerspective(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fFieldOfView, m_fNearPlane, m_fFarPlane);
		}
		else {
			setOrthographic(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fNearPlane, m_fFarPlane);
		}

		m_iDirty &= ~CAMERA_DIRTY_PROJ;
	}
}

Matrix4& Camera::getProjectionMatrix() {
	if(m_iDirty & CAMERA_DIRTY_PROJ) {
		
		if(m_iCameraType == PERSPECTIVE) {
			setPerspective(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fFieldOfView, m_fNearPlane, m_fFarPlane);
		}
		else {
			setOrthographic(m_iViewX, m_iViewY, m_iViewW, m_iViewH, m_fNearPlane, m_fFarPlane);
		}

		m_iDirty &= ~CAMERA_DIRTY_PROJ;
	}

	return m_MatrixProjection;
}

Matrix4& Camera::getViewProjectionMatrix() {
	if (m_iDirty & CAMERA_DIRTY_VIEW_PROJ)
	{
		m_MatrixViewProjection = getProjectionMatrix() * getViewMatrix();
		m_iDirty &= ~CAMERA_DIRTY_VIEW_PROJ;
	}

	return m_MatrixViewProjection;
}

Matrix4& Camera::getViewMatrix() {
	if (m_iDirty & CAMERA_DIRTY_VIEW) {
		if (m_pNode) {
			// The view matrix is the inverse of our transform matrix.
			m_MatrixView = m_pNode->getWorldMatrix();//.invert(&m_MatrixView);
		}
		else {
			m_MatrixView.setIdentity();
		}

		m_iDirty &= ~CAMERA_DIRTY_VIEW;
	}

	return m_MatrixView;
}

///////////////////////////////////////////////////////////////////////////////
// configure projection and viewport of sub window
///////////////////////////////////////////////////////////////////////////////
void Camera::setPerspective(int x, int y, int w, int h, float iFieldOfView, float fNearPlane, float fFarPlane) {

	///////////////////////// Set Viewport
	int xViewport = x;
	int yViewport = EngineManager::getInstance()->getHeight() - (y + h);
	int wViewport = w;
	int hViewport = h;
	glEnable(GL_SCISSOR_TEST);
	glViewport(xViewport, yViewport, wViewport, hViewport);
	glScissor(xViewport, yViewport, wViewport, hViewport);
	////////////////////////////////////////////

	///////////////// Set Perspective Viewing Frustum
	setFrustum(iFieldOfView, (float)(w)/h, fNearPlane, fFarPlane); // FOV, AspectRatio, NearClip, FarClip
	////////////////////////////////////////////

	// copy projection matrix to OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_MatrixProjection.getTranspose());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
void Camera::setFrustum(float fovY, float aspectRatio, float front, float back) {
	float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane

	// params: left, right, bottom, top, near, far
	setPerspectiveFrustum(-width, width, -height, height, front, back);
}

///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void Camera::setPerspectiveFrustum(float l, float r, float b, float t, float n, float f) {
	m_MatrixProjection.setIdentity();
	m_MatrixProjection[0]  =  2 * n / (r - l);
	m_MatrixProjection[2]  =  (r + l) / (r - l);
	m_MatrixProjection[5]  =  2 * n / (t - b);
	m_MatrixProjection[6]  =  (t + b) / (t - b);
	m_MatrixProjection[10] = -(f + n) / (f - n);
	m_MatrixProjection[11] = -(2 * f * n) / (f - n);
	m_MatrixProjection[14] = -1;
	m_MatrixProjection[15] =  0;
}

void Camera::setOrthographic(int x, int y, int w, int h, float fNearPlane, float fFarPlane) {

	///////////////////////// Set Viewport
	int xViewport = x;
	int yViewport = EngineManager::getInstance()->getHeight() - (y + h);
	int wViewport = w;
	int hViewport = h;
	glEnable(GL_SCISSOR_TEST);
	glViewport(xViewport, yViewport, wViewport, hViewport);
	glScissor(xViewport, yViewport, wViewport, hViewport);
	////////////////////////////////////////////

	//////////////// Set Orthogonal Viewing Frustrum
	int left = x;
	int right = (x + w);
	int bottom = (y + h);
	int top = y;
	setOrthogonalFrustum((float)left, (float)right, (float)bottom, (float)top, 0.0f, 1.0f);
	////////////////////////////////////////////
	
	glMatrixMode(GL_PROJECTION);						// Setup and orthogonal, pixel-to-pixel projection matrix
	glLoadMatrixf(m_MatrixProjection.getTranspose());	// copy projection matrix to OpenGL

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set a orthographic frustum with 6 params similar to glOrtho()
// (left, right, bottom, top, near, far)
// Note: this is for row-major notation. OpenGL needs transpose it
///////////////////////////////////////////////////////////////////////////////
void Camera::setOrthogonalFrustum(float l, float r, float b, float t, float n, float f) {
	m_MatrixProjection.setIdentity();
	m_MatrixProjection[0]  =  2 / (r - l);
	m_MatrixProjection[3]  =  -(r + l) / (r - l);
	m_MatrixProjection[5]  =  2 / (t - b);
	m_MatrixProjection[7]  =  -(t + b) / (t - b);
	m_MatrixProjection[10] = -2 / (f - n);
	m_MatrixProjection[11] = -(f + n) / (f - n);
}

void Camera::setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ) {
	float forward[4];
	float up[4];
	float left[4];
	float position[4];
	float invLength;

	// determine forward vector (direction reversed because it is camera)
	forward[0] = -(posX - targetX);    // x
	forward[1] = -(posY - targetY);    // y
	forward[2] = -(posZ - targetZ);    // z
	forward[3] = 0.0f;              // w

	// normalize it without w-component
	invLength = 1.0f / sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
	forward[0] *= invLength;
	forward[1] *= invLength;
	forward[2] *= invLength;

	// assume up direction is straight up
	up[0] = 0.0f;   // x
	up[1] = 1.0f;   // y
	up[2] = 0.0f;   // z
	up[3] = 0.0f;   // w

	// compute left vector with cross product
	left[0] = up[1]*forward[2] - up[2]*forward[1];  // x
	left[1] = up[2]*forward[0] - up[0]*forward[2];  // y
	left[2] = up[0]*forward[1] - up[1]*forward[0];  // z
	left[3] = 1.0f;                                 // w

	// re-compute orthogonal up vector
	up[0] = forward[1]*left[2] - forward[2]*left[1];    // x
	up[1] = forward[2]*left[0] - forward[0]*left[2];    // y
	up[2] = forward[0]*left[1] - forward[1]*left[0];    // z
	up[3] = 0.0f;                                       // w

	// camera position
	position[0] = -posX;
	position[1] = -posY;
	position[2] = -posZ;
	position[3] = 1.0f;

	// copy axis vectors to matrix
	m_pNode->setIdentity();
	m_pNode->setAxisX(Vector3(left[0], left[1], left[2]));
	m_pNode->setAxisY(Vector3(up[0], up[1], up[2]));
	m_pNode->setAxisZ(Vector3(forward[0], forward[1], forward[2]));
	m_pNode->setPosition(Vector3(position[0], position[1], position[2]));
	setDirty(CAMERA_DIRTY_VIEW);
}

void Camera::handleMouse(float deltaTimeMs) {
	EngineManager* pEngineManager = EngineManager::getInstance();
	//MouseManager* pMouseManager = pEngineManager->getMouseManager();
	int screenCenterX = pEngineManager->getWidth()>>1;
	int screenCenterY = pEngineManager->getHeight()>>1;

	POINT mousePos;
	GetCursorPos(&mousePos);
	//mousePos.x = pMouseManager->getMousePosX();
	//mousePos.y = pMouseManager->getMousePosY();

	//Mouse not moved, hence return;
	if(mousePos.x == screenCenterX && mousePos.y == screenCenterY)
		return;

	SetCursorPos(screenCenterX, screenCenterY);

	//Move, Rotate as proportinal to the displacement
	float deltaX = (mousePos.x - screenCenterX)/5.0f;
	float deltaY = (mousePos.y - screenCenterY)/5.0f;

	bool yesRotateX = false;
	bool yesRotateY = (deltaX != 0);
	if(deltaY < 0) {
		if(m_pNode->getRotateX() < 80) {
			yesRotateX = true;
		}
	}
	else 
	if(deltaY > 0) {
		if(m_pNode->getRotateX() > -80) {
			yesRotateX = true;
		}
	}

	if(yesRotateX) {
		m_pNode->rotateX(-deltaY);
		setDirty(CAMERA_DIRTY_VIEW);
	}

	if(yesRotateY) {
		m_pNode->rotateY(-deltaX);
		setDirty(CAMERA_DIRTY_VIEW);
	}
}

void Camera::handleKeyboard(float deltaTimeMs) {
	if(EngineManager::isKeyPressed('Q') || EngineManager::isKeyPressed('q') || EngineManager::isKeyPressed(VK_LEFT)) {
		m_pNode->rotateY(ROTATE_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}
	else
	if(EngineManager::isKeyPressed('E') || EngineManager::isKeyPressed('e') || EngineManager::isKeyPressed(VK_RIGHT)) {
		m_pNode->rotateY(-ROTATE_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}

	if(EngineManager::isKeyPressed('W') || EngineManager::isKeyPressed('w')) {
		m_pNode->translateForward(-WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}
	else
	if(EngineManager::isKeyPressed('S') || EngineManager::isKeyPressed('s')) {
		m_pNode->translateForward(WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}

	if(EngineManager::isKeyPressed('A') || EngineManager::isKeyPressed('a')) {
		m_pNode->translateLeft(-WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}
	else
	if(EngineManager::isKeyPressed('D') || EngineManager::isKeyPressed('d')) {
		m_pNode->translateLeft(WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}

	if(EngineManager::isKeyPressed(VK_PRIOR)) {
		m_pNode->translateUp(WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}
	else
	if(EngineManager::isKeyPressed(VK_NEXT)) {
		m_pNode->translateUp(-WALK_SPEED * deltaTimeMs);
		setDirty(CAMERA_DIRTY_VIEW);
	}

	if(EngineManager::isKeyPressed(VK_UP)) {
		if(m_pNode->getRotateX() < 80) {
			m_pNode->rotateX(ROTATE_SPEED * deltaTimeMs);
			setDirty(CAMERA_DIRTY_VIEW);
		}
	}
	else
	if(EngineManager::isKeyPressed(VK_DOWN)) {
		if(m_pNode->getRotateX() > -80) {
			m_pNode->rotateX(-ROTATE_SPEED * deltaTimeMs);
			setDirty(CAMERA_DIRTY_VIEW);
		}
	}
}

void Camera::update(float deltaTimeMs) {
	handleKeyboard(deltaTimeMs);
	handleMouse(deltaTimeMs);
}

Node* Camera::getNode() const {
	return m_pNode;
}

void Camera::setNode(Node* node) {
	if(node != m_pNode) {
		m_pNode = node;
	}
}

void Camera::setDirty(int iDirty) {
	m_iDirty |= iDirty;
}