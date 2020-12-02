#pragma once

#include "geometric_primitive.h"


class StaticMesh : public GeometricPrimitive
{
protected:
	void _fbxInit(ID3D11Device* device, const char* fbx_filename);

public:
	StaticMesh(ID3D11Device* device, const char* fbx_filename) : GeometricPrimitive()
	{
		//	VertexShader & PixelShader
		D3D11_INPUT_ELEMENT_DESC elements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(elements);
		const char* vsName = "./Data/Shader/static_mesh_vs.cso";
		const char* psName = "./Data/Shader/static_mesh_ps.cso";

		_init(device, vsName, elements, numElements, psName);
		_fbxInit(device, fbx_filename);
	}
	StaticMesh(ID3D11Device* device, const char* fbx_filename,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName) : GeometricPrimitive()
	{
		_init(device, vsName, inputElementDescs, numElement, psName);
		_fbxInit(device, fbx_filename);
	}
	~StaticMesh(){}
};
