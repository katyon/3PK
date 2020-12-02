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
		const char* vsName, D3D11_INPUT_ELEMENT_DESC* inputElementDescs, int numElement,
		const char* psName) : GeometricPrimitive()
	{
		_init(device, vsName, inputElementDescs, numElement, psName);
		_fbxInit(device, fbx_filename);
	}
	~StaticMesh() {}
};


//#pragma once
//
//#include <d3d11.h>
//#include <wrl.h>
//#include <directxmath.h>
//#include <vector>
//
//#include "geometric_primitive.h"
//
//class static_mesh
//{
//public:
//	struct vertex
//	{
//		DirectX::XMFLOAT3 position;
//		DirectX::XMFLOAT3 normal;
//		DirectX::XMFLOAT2 texcoord;
//	};
//	struct cbuffer
//	{
//		DirectX::XMFLOAT4X4 world_view_projection;
//		DirectX::XMFLOAT4X4 world;
//		DirectX::XMFLOAT4 material_color;
//		DirectX::XMFLOAT4 light_direction;
//	};
//
//	struct subset
//	{
//		std::wstring usemtl;
//		u_int index_start = 0;
//		u_int index_count = 0;
//	};
//	std::vector<subset> subsets;
//
//	struct material
//	{
//		std::wstring newmtl;
//		DirectX::XMFLOAT3 Ka = { 0.2f, 0.2f, 0.2f };
//		DirectX::XMFLOAT3 Kd = { 0.8f, 0.8f, 0.8f };
//		DirectX::XMFLOAT3 Ks = { 1.0f, 1.0f, 1.0f };
//		u_int illum = 1;
//		std::wstring map_Kd;
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
//	};
//	std::vector<material> materials;
//
//private:
//	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
//
//	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
//	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
//	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
//
//	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
//	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
//
//	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
//
//public:
//	static_mesh(ID3D11Device *device, const wchar_t *obj_filename, bool flipping_v_coordinates);
//	virtual ~static_mesh();
//
//	void render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world_inverse_transpose, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe = false);
//
//private:
//	void create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
//
//};
//
