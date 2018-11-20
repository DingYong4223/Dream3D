#ifndef VERTEXATTRIBUTEBINDING_H
#define VERTEXATTRIBUTEBINDING_H

#include "Base.h"
#include "Engine/VertexFormat.h"
#include "Engine/Mesh.h"

class VertexAttributeBinding {
	
	public:
		static VertexAttributeBinding*	create(Mesh* mesh/*, Effect* effect*/);
		static VertexAttributeBinding*	create(const VertexFormat& vertexFormat, void* vertexPointer/*, Effect* effect*/);

		void							bind();
		void							unbind();

		virtual ~VertexAttributeBinding();
	private:
		class VertexAttribute {
			public:
				bool			m_bEnabled;
				int				m_iSize;
				GLenum			m_Type;
				GLboolean		m_bNormalized;
				unsigned int	m_iStride;
				void*			m_pPointer;
		};

		VertexAttributeBinding();
		static VertexAttributeBinding*	create(Mesh* mesh, const VertexFormat& vertexFormat, void* vertexPointer/*, Effect* effect*/);
		void setVertexAttributeBinding(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer);

		VAOHandle			m_hVAO;
		VertexAttribute*	m_pAttributes;
		Mesh*				m_pMesh;
};

#endif