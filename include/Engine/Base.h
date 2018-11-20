#ifndef BASE_H
#define BASE_H

//#define USE_VAO

//C/C++
#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include <gl\glew.h>
//#include <gl\gl.h>						// Header File For The OpenGL32 Library
//#include <gl\glu.h>						// Header File For The GLu32 Library

// Common
#ifndef NULL
#define NULL     0
#endif

typedef GLuint VertexAttribute;				// Vertex Attribute
typedef GLuint VBOHandle;					// Vertex Buffer Object Handle
typedef GLuint VAOHandle;					// Vertex Array Object Handle
typedef GLuint IBOHandle;					// Indexed Buffer Object Handle

#define gl_Vertex 			0
#define gl_Normal 			1
#define gl_TexCoord 		2
#define gl_Color 			3
#define gl_SecondaryColor 	4
#define gl_FogCoord 		5
#define gl_MultiTexCoord0 	8
#define gl_MultiTexCoord1 	9
#define gl_MultiTexCoord2 	10
#define gl_MultiTexCoord3 	11
#define gl_MultiTexCoord4 	12
#define gl_MultiTexCoord5 	13
#define gl_MultiTexCoord6 	14
#define gl_MultiTexCoord7 	15

/** Global variable to hold GL errors
 * @script{ignore} */
static GLenum __gl_error_code;

// Assert macros.
#ifdef _DEBUG
#define GP_ASSERT(expression) assert(expression)
#else
#define GP_ASSERT(expression)
#endif


#ifdef NDEBUG
#define GL_ASSERT( gl_code ) gl_code
#else
#define GL_ASSERT( gl_code ) do	{ gl_code; __gl_error_code = glGetError(); GP_ASSERT(__gl_error_code == GL_NO_ERROR); } while(0)
#endif

// Object deletion macro
#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

// Array deletion macro
#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = NULL; }

// Ref cleanup macro
#define SAFE_RELEASE(x)	if(x) { (x)->release(); x = NULL; }

typedef GLuint TextureHandle;

#endif