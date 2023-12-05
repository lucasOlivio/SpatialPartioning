#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct sVertex
{
	float x, y, z, w;
	float r, g, b, a;
	float nx, ny, nz, nw;
	float u, v;
};

struct sTriangleMesh
{
	sVertex* pVertices;

	glm::vec3 GetNormal(void)
	{
		using namespace glm;

		vec3 v1 = vec3(
			this->pVertices[0].nx,
			this->pVertices[0].ny,
			this->pVertices[0].nz
		);
		vec3 v2 = vec3(
			this->pVertices[1].nx,
			this->pVertices[1].ny,
			this->pVertices[1].nz
		);
		vec3 v3 = vec3(
			this->pVertices[2].nx,
			this->pVertices[2].ny,
			this->pVertices[2].nz
		);

		vec3 averageNormal = normalize(v1 + v2 + v3);

		return averageNormal;
	}

	~sTriangleMesh()
	{
		delete[] pVertices;
	}
};

struct sMesh
{
	std::string name;

	unsigned int VAO_ID;

	unsigned int vertexBufferID;
	unsigned int vertexBuffer_Start_Index;
	unsigned int numberOfVertices;

	unsigned int indexBufferID;
	unsigned int indexBuffer_Start_Index;
	unsigned int numberOfIndices;
	unsigned int numberOfTriangles;

	// The index buffer (CPU side)
	unsigned int* pIndices;
	sVertex* pVertices;
	sTriangleMesh* pTriangles;

	// Mesh info
	float maxX, maxY, maxZ;
	float minX, minY, minZ;

	glm::vec3 GetExtent()
	{
		return glm::vec3(
			maxX - minX,
			maxY - minY,
			maxZ - minZ
		);
	}

	~sMesh()
	{
		delete[] pIndices;
		delete[] pTriangles;
		delete[] pVertices;
	}
};