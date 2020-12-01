#pragma once

// UNIT.16
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

// UNIT.18
#include <vector>

class skinned_mesh
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		// UNIT.17
		DirectX::XMFLOAT2 texcoord;
	};
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world_inverse_transpose;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};

	// UNIT.17
	struct material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f }; // w channel is used as shininess by only specular.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	};
	// UNIT.18
	//material diffuse;

	// UNIT.18
	struct subset
	{
		u_int index_start = 0;	// start number of index buffer
		u_int index_count = 0;	// number of vertices (indices)
		material diffuse;
	};
	// UNIT.19
	//std::vector<subset> subsets;

	// UNIT.19
	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<subset> subsets;
		
		DirectX::XMFLOAT4X4 global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		
		void create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
	};
	std::vector<mesh> meshes;

private:
	// UNIT.19
	//Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;

	// UNIT.17
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

public:
	skinned_mesh(ID3D11Device *device, const char *fbx_filename);
	virtual ~skinned_mesh() {}

	void render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world_inverse_transpose, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe = false);

protected:
	// UNIT.19
	//void create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices);
};
