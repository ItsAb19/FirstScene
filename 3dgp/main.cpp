#include <iostream>
#include <GL/glew.h>
#include <3dgl/3dgl.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D models
C3dglModel camera;
C3dglModel table;
C3dglModel lamp;
C3dglModel lamp2;

const vec3 LAMP1_BASE_POS = vec3(13.0f, 4.45f, 9.0f);  // on top of table
const vec3 LAMP2_BASE_POS = vec3(17.0f, 4.45f, 9.0f);  // on top of table
const float LAMP_BULB_Y_OFFSET = 1.2f;

// The View Matrix
mat4 matrixView;

// Camera & navigation
float maxspeed = 4.f;	// camera max speed
float accel = 4.f;		// camera acceleration
vec3 _acc(0), _vel(0);	// camera acceleration and velocity vectors
float _fov = 60.f;		// field of view (zoom)

bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// setup lighting
	glEnable(GL_LIGHTING);									// --- DEPRECATED
	//ambient lighting
	GLfloat globalAmbient[] = { 0.25f, 0.25f, 0.25f, 1.0f }; // tweak: 0.1 = dim, 0.4 = brighter
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	// Warm lamp colour
	GLfloat lampDiffuse[] = { 1.0f, 0.95f, 0.8f, 1.0f };
	GLfloat lampSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lampAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // keep 0 (global ambient handles ambient)

	// LIGHT1 properties
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lampDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpecular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lampAmbient);

	// LIGHT2 properties
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lampDiffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lampSpecular);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lampAmbient);

	// Attenuation (realistic falloff)
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.15f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05f);

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.15f);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.05f);

	// Specular highlights need material settings
	GLfloat matSpec[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

	// Material properties
	GLfloat shininess = 32.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	//----------------------------------------------------------------------------
	glEnable(GL_LIGHT0);									// --- DEPRECATED

	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	if (!table.load("models\\Desk.3ds")) return false;
	if (!lamp.load("models\\Lamp.3ds")) return false;
	if (!lamp2.load("models\\Lamp.3ds")) return false;


	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1), radians(12.f), vec3(1, 0, 0));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 10.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift to speed up your movement" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void renderScene(mat4& matrixView, float time, float deltaTime)
{
	mat4 m;

	// setup materials - grey
	GLfloat rgbaGrey[] = { 0.6f, 0.6f, 0.6f, 1.0f };		// --- DEPRECATED
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgbaGrey);	// --- DEPRECATED
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgbaGrey);	// --- DEPRECATED

	// camera
	m = matrixView;
	m = translate(m, vec3(-3.0f, 0, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.04f, 0.04f, 0.04f));
	camera.render(m);

	//table
	m = matrixView;
	m = translate(m, vec3(15.0f, 0, 9.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	// ---------- TABLE MATERIAL (WOOD) ----------
	GLfloat woodCol[] = { 0.55f, 0.35f, 0.18f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, woodCol);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, woodCol);
	table.render(m);

	// setup materials - blue
	GLfloat rgbaBlue[] = { 0.2f, 0.2f, 0.8f, 1.0f };		// --- DEPRECATED
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, rgbaBlue);	// --- DEPRECATED
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, rgbaBlue);	// --- DEPRECATED

	// teapot
	m = matrixView;
	m = translate(m, vec3(15.0f, 4.45f, 0.0f));
	m = rotate(m, radians(120.f), vec3(0.0f, 1.0f, 0.0f));
	// the GLUT objects require the Model View Matrix setup
	glMatrixMode(GL_MODELVIEW);								// --- DEPRECATED
	glLoadIdentity();										// --- DEPRECATED
	glMultMatrixf((GLfloat*)&m);							// --- DEPRECATED
	glutSolidTeapot(2.0);

	//lamp1
	m = matrixView;
	m = translate(m, vec3(4.0f, -7, -2.0f));
	m = rotate(m, radians(180.f), vec3(0.70710678f, -0.70710678f, 0.0f)); // axis = (1,-1,0)
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	lamp.render(m);

	//lamp2
	m = matrixView;
	m = translate(m, vec3(20.0f, -7, 4.0f));
	m = rotate(m, radians(180.f), vec3(0.70710678f, -0.70710678f, 0.0f)); // axis = (1,-1,0)
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	lamp.render(m);
}
void setDirectionalDiffuseLight()
{
	// Directional light: (x,y,z,0). Think "light is coming FROM this direction toward the scene".
	// Example: coming from above-right-front -> shining down onto the table.
	GLfloat dir[] = { -0.3f,  1.0f,  0.4f,  0.0f };  // w=0 => directional
	GLfloat diffuse[] = { 1.0f,  1.0f,  1.0f,  1.0f };  // main brightness
	GLfloat ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };  // keep low (ambient is separate)
	GLfloat spec[] = { 0.2f,  0.2f,  0.2f,  1.0f };  // optional

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	// Make it "world directional": load the current view, then set the light direction
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf((GLfloat*)&matrixView);

	glLightfv(GL_LIGHT0, GL_POSITION, dir);
}
void onRender()
{
	// these variables control time & animation
	static float prev = 0;
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;	// time since start in seconds
	float deltaTime = time - prev;						// time since last frame
	prev = time;										// framerate is 1/deltaTime

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	_vel = clamp(_vel + _acc * deltaTime, -vec3(maxspeed), vec3(maxspeed));
	float pitch = getPitch(matrixView);
	matrixView = rotate(translate(rotate(mat4(1),
		pitch, vec3(1, 0, 0)),	// switch the pitch off
		_vel * deltaTime),		// animate camera motion (controlled by WASD keys)
		-pitch, vec3(1, 0, 0))	// switch the pitch on
		* matrixView;

	setDirectionalDiffuseLight();
	//LIGHTING
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf((GLfloat*)&matrixView);

	// Point lights at the lamp "bulbs" (above each lamp base)
	vec3 bulb1 = LAMP1_BASE_POS + vec3(0.0f, LAMP_BULB_Y_OFFSET, 0.0f);
	GLfloat lamp1Pos[] = { bulb1.x, bulb1.y, bulb1.z, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lamp1Pos);

	vec3 bulb2 = LAMP2_BASE_POS + vec3(0.0f, LAMP_BULB_Y_OFFSET, 0.0f);
	GLfloat lamp2Pos[] = { bulb2.x, bulb2.y, bulb2.z, 1.0f };
	glLightfv(GL_LIGHT2, GL_POSITION, lamp2Pos);

	// render the scene objects
	renderScene(matrixView, time, deltaTime);

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(_fov), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	glMatrixMode(GL_PROJECTION);							// --- DEPRECATED
	glLoadIdentity();										// --- DEPRECATED
	glMultMatrixf((GLfloat*)&matrixProjection);				// --- DEPRECATED
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': _acc.z = accel; break;
	case 's': _acc.z = -accel; break;
	case 'a': _acc.x = accel; break;
	case 'd': _acc.x = -accel; break;
	case 'e': _acc.y = accel; break;
	case 'q': _acc.y = -accel; break;
	}
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': _acc.z = _vel.z = 0; break;
	case 'a':
	case 'd': _acc.x = _vel.x = 0; break;
	case 'q':
	case 'e': _acc.y = _vel.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	maxspeed = glutGetModifiers() & GLUT_ACTIVE_SHIFT ? 20.f : 4.f;
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
void onMouse(int button, int state, int x, int y)
{
	glutSetCursor(state == GLUT_DOWN ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
	if (button == 1)
	{
		_fov = 60.0f;
		onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

// handle mouse move
void onMotion(int x, int y)
{
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

	// find delta (change to) pan & pitch
	float deltaYaw = 0.005f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
	float deltaPitch = 0.005f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

	if (abs(deltaYaw) > 0.3f || abs(deltaPitch) > 0.3f)
		return;	// avoid warping side-effects

	// View = Pitch * DeltaPitch * DeltaYaw * Pitch^-1 * View;
	constexpr float maxPitch = radians(80.f);
	float pitch = getPitch(matrixView);
	float newPitch = glm::clamp(pitch + deltaPitch, -maxPitch, maxPitch);
	matrixView = rotate(rotate(rotate(mat4(1.f),
		newPitch, vec3(1.f, 0.f, 0.f)),
		deltaYaw, vec3(0.f, 1.f, 0.f)), 
		-pitch, vec3(1.f, 0.f, 0.f)) 
		* matrixView;
}

void onMouseWheel(int button, int dir, int x, int y)
{
	_fov = glm::clamp(_fov - dir * 5.f, 5.0f, 175.f);
	onReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("3DGL Scene: First Example");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		C3dglLogger::log("GLEW Error {}", (const char*)glewGetErrorString(err));
		return 0;
	}
	C3dglLogger::log("Using GLEW {}", (const char*)glewGetString(GLEW_VERSION));

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutMouseWheelFunc(onMouseWheel);

	C3dglLogger::log("Vendor: {}", (const char *)glGetString(GL_VENDOR));
	C3dglLogger::log("Renderer: {}", (const char *)glGetString(GL_RENDERER));
	C3dglLogger::log("Version: {}", (const char*)glGetString(GL_VERSION));
	C3dglLogger::log("");

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		C3dglLogger::log("Application failed to initialise\r\n");
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}

