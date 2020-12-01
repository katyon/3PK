#pragma once

// UNIT.12
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>

class static_mesh
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		// UNIT.13
		DirectX::XMFLOAT2 texcoord;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

	// UNIT.14
	struct subset
	{
		std::wstring usemtl;
		u_int index_start = 0;
		u_int index_count = 0;
	};
	std::vector<subset> subsets;

	// UNIT.14
	struct material
	{
		std::wstring newmtl;
		DirectX::XMFLOAT3 Ka = { 0.2f, 0.2f, 0.2f };
		DirectX::XMFLOAT3 Kd = { 0.8f, 0.8f, 0.8f };
		DirectX::XMFLOAT3 Ks = { 1.0f, 1.0f, 1.0f };
		u_int illum = 1;
		std::wstring map_Kd;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};
	std::vector<material> materials;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	//UNIT.13
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

public:
	static_mesh(ID3D11Device *device, const wchar_t *obj_filename, bool flipping_v_coordinates/*UNIT.13*/);
	virtual ~static_mesh();

	void render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world_inverse_transpose, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe = false);

private:
	void create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);

};

