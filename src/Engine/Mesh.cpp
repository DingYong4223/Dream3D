#include "Engine/Mesh.h"

Mesh::Mesh(const VertexFormat& vertexFormat) 
	:	m_VertexFormat(vertexFormat),
		m_iVertexCount(0),
		m_hVBO(0),
		m_PrimitiveType(TRIANGLES)
{

}

Mesh::~Mesh() {

	if(m_hVBO) {
		GL_ASSERT( glDeleteBuffers(1, &m_hVBO) );
		m_hVBO = 0;
	}
}

Mesh* Mesh::createMesh(const VertexFormat& vertexFormat, unsigned int vertexCount, bool bDynamic) {
	GLuint vbo;
	GL_ASSERT( glGenBuffers(1, &vbo) );
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
	GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize()*vertexCount, NULL, bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW ));
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, 0) );

	Mesh* mesh = new Mesh(vertexFormat);
	mesh->m_iVertexCount = vertexCount;
	mesh->m_hVBO = vbo;
	mesh->m_bDynamic = bDynamic;

	return mesh;
}

const VertexFormat& Mesh::getVertexFormat() const {
	return m_VertexFormat;
}

unsigned int Mesh::getVertexCount() const {
	return m_iVertexCount;
}

unsigned int Mesh::getVertexSize() const {
	return m_VertexFormat.getVertexSize();
}

VBOHandle Mesh::getVertexBuffer() const {
	return m_hVBO;
}

bool Mesh::isDynamic() const {
	return m_bDynamic;
}

Mesh::PrimitiveType Mesh::getPrimitiveType() const {
	return m_PrimitiveType;
}

void Mesh::setPrimitiveType(Mesh::PrimitiveType type) {
	m_PrimitiveType = type;
}

void Mesh::setVertexData(const float* vertexData, unsigned int vertexStart, unsigned int vertexCount) {
	GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, m_hVBO) );
	if(vertexStart == 0 && vertexCount == 0) {
		GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, m_VertexFormat.getVertexSize() * m_iVertexCount, vertexData, m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
	}
	else {
		if(vertexCount == 0) {
			vertexCount = m_iVertexCount - vertexStart;
		}

		GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, vertexStart * m_VertexFormat.getVertexSize(), vertexCount * m_VertexFormat.getVertexSize(), vertexData) );
	}
}
