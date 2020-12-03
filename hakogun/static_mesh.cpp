#include	"static_mesh.h"
#include	<fbxsdk.h>

using namespace fbxsdk;

#include	<vector>
#include	<functional>

#include	"resource_manager.h"

void StaticMesh::_fbxInit(ID3D11Device* device, const char* fbx_filename)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool import_status = false;
	import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
	if (!import_status)
	{
		assert(importer->GetStatus().GetErrorString());
		return;
	}

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	import_status = importer->Import(scene);
	if (!import_status)
	{
		assert(importer->GetStatus().GetErrorString());
		return;
	}

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetched_meshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
		if (node) {
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType()) {
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	FbxMesh* fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.

	// Fetch mesh data
	std::vector<Vertex> vertices;	// Vertex buffer
	std::vector<u_int> indices;		// Index buffer
	u_int vertex_count = 0;

	const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
	const int number_of_polygons = fbx_mesh->GetPolygonCount();
	for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++) {
		for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
			Vertex vertex;
			const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
			vertex.pos.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
			vertex.pos.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
			vertex.pos.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

			FbxVector4 normal;
			fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
			vertex.normal.x = static_cast<float>(normal[0]);
			vertex.normal.y = static_cast<float>(normal[1]);
			vertex.normal.z = static_cast<float>(normal[2]);

			vertices.push_back(vertex);
			indices.push_back(vertex_count);
			vertex_count += 1;
		}
	}
	manager->Destroy();

	CreateBuffers(device, vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());
}


//#include "static_mesh.h"
//#include "shader.h"
//#include "misc.h"
//
//#include <fstream>
//#include <vector>
//
//#include "texture.h"
//
//static_mesh::static_mesh(ID3D11Device *device, const wchar_t *obj_filename, bool flipping_v_coordinates/*UNIT.13*/)
//{
//	std::vector<vertex> vertices;
//	std::vector<u_int> indices;
//	u_int current_index = 0;
//
//	std::vector<DirectX::XMFLOAT3> positions;
//	std::vector<DirectX::XMFLOAT2> texcoords;
//	std::vector<DirectX::XMFLOAT3> normals;
//
//	std::vector<std::wstring> mtl_filenames;
//
//	// parse OBJ file
//	{
//		std::wifstream fin(obj_filename);
//		_ASSERT_EXPR(fin, L"'OBJ file not fuund.");
//		wchar_t command[256];
//		while (fin)
//		{
//			fin >> command;
//			if (0 == wcscmp(command, L"v"))
//			{
//				float x, y, z;
//				fin >> x >> y >> z;
//				positions.push_back(DirectX::XMFLOAT3(x, y, z));
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"vt"))
//			{
//				float u, v;
//				fin >> u >> v;
//				texcoords.push_back(DirectX::XMFLOAT2(u, flipping_v_coordinates ? 1.0f - v : v));
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"vn"))
//			{
//				FLOAT i, j, k;
//				fin >> i >> j >> k;
//				normals.push_back(DirectX::XMFLOAT3(i, j, k));
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"f"))
//			{
//				for (u_int i = 0; i < 3; i++)
//				{
//					vertex vertex;
//					u_int v, vt, vn;
//
//					fin >> v;
//					vertex.position = positions[v - 1];
//					if (L'/' == fin.peek())
//					{
//						fin.ignore();
//						if (L'/' != fin.peek())
//						{
//							fin >> vt;
//							vertex.texcoord = texcoords[vt - 1];
//						}
//						if (L'/' == fin.peek())
//						{
//							fin.ignore();
//							fin >> vn;
//							vertex.normal = normals[vn - 1];
//						}
//					}
//					vertices.push_back(vertex);
//					indices.push_back(current_index++);
//				}
//				fin.ignore(1024, '\n');
//			}
//			else if (0 == wcscmp(command, L"mtllib"))
//			{
//				wchar_t mtllib[256];
//				fin >> mtllib;
//				mtl_filenames.push_back(mtllib);
//			}
//			else if (0 == wcscmp(command, L"usemtl"))
//			{
//				wchar_t usemtl[MAX_PATH] = { 0 };
//				fin >> usemtl;
//
//				subset current_subset = {};
//				current_subset.usemtl = usemtl;
//				current_subset.index_start = indices.size();
//				subsets.push_back(current_subset);
//			}
//			else
//			{
//				fin.ignore(1024, '\n');
//			}
//		}
//		fin.close();
//	}
//
//	std::vector<subset>::reverse_iterator iterator = subsets.rbegin();
//	iterator->index_count = indices.size() - iterator->index_start;
//	for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
//	{
//		iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
//	}
//
//	create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
//	// parse MTL file
//	{
//		wchar_t mtl_filename[256];
//		combine_resource_path(mtl_filename, obj_filename, mtl_filenames[0].c_str());
//
//		std::wifstream fin(mtl_filename);
//		_ASSERT_EXPR(fin, L"'MTL file not fuund.");
//
//		wchar_t command[256] = { 0 };
//		while (fin)
//		{
//			fin >> command;
//			if (0 == wcscmp(command, L"#"))
//			{
//				// Comment
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"map_Kd"))
//			{
//				fin.ignore();
//				wchar_t map_Kd[256];
//				fin >> map_Kd;
//				combine_resource_path(map_Kd, obj_filename, map_Kd);
//				materials.rbegin()->map_Kd = map_Kd;
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"newmtl"))
//			{
//				fin.ignore();
//				wchar_t newmtl[256];
//				material material;
//				fin >> newmtl;
//				material.newmtl = newmtl;
//				materials.push_back(material);
//			}
//			else if (0 == wcscmp(command, L"Ka"))
//			{
//				float r, g, b;
//				fin >> r >> g >> b;
//				materials.rbegin()->Ka = DirectX::XMFLOAT3(r, g, b);
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"Kd"))
//			{
//				float r, g, b;
//				fin >> r >> g >> b;
//				materials.rbegin()->Kd = DirectX::XMFLOAT3(r, g, b);
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"Ks"))
//			{
//				float r, g, b;
//				fin >> r >> g >> b;
//				materials.rbegin()->Ks = DirectX::XMFLOAT3(r, g, b);
//				fin.ignore(1024, L'\n');
//			}
//			else if (0 == wcscmp(command, L"illum"))
//			{
//				u_int illum;
//				fin >> illum;
//				materials.rbegin()->illum = illum;
//				fin.ignore(1024, L'\n');
//			}
//			else
//			{
//				// Unimplemented or unrecognized command
//				fin.ignore(1024, L'\n');
//			}
//		}
//	}
//
//	HRESULT hr = S_OK;
//
//	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
//	create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.GetAddressOf());
//
//	// create rasterizer state : solid mode
//	{
//		D3D11_RASTERIZER_DESC rasterizer_desc = {};
//		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
//		rasterizer_desc.CullMode = D3D11_CULL_BACK;
//		rasterizer_desc.FrontCounterClockwise = FALSE;
//		rasterizer_desc.DepthBias = 0;
//		rasterizer_desc.DepthBiasClamp = 0;
//		rasterizer_desc.SlopeScaledDepthBias = 0;
//		rasterizer_desc.DepthClipEnable = TRUE;
//		rasterizer_desc.ScissorEnable = FALSE;
//		rasterizer_desc.MultisampleEnable = FALSE;
//		rasterizer_desc.AntialiasedLineEnable = FALSE;
//		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//	// create rasterizer state : wireframe mode
//	{
//		D3D11_RASTERIZER_DESC rasterizer_desc = {};
//		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
//		rasterizer_desc.CullMode = D3D11_CULL_BACK; 
//		rasterizer_desc.FrontCounterClockwise = FALSE;
//		rasterizer_desc.DepthBias = 0;
//		rasterizer_desc.DepthBiasClamp = 0;
//		rasterizer_desc.SlopeScaledDepthBias = 0;
//		rasterizer_desc.DepthClipEnable = TRUE;
//		rasterizer_desc.ScissorEnable = FALSE;
//		rasterizer_desc.MultisampleEnable = FALSE;
//		rasterizer_desc.AntialiasedLineEnable = FALSE;
//		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//	// create depth stencil state
//	{
//		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
//		depth_stencil_desc.DepthEnable = TRUE;
//		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
//		depth_stencil_desc.StencilEnable = FALSE;
//		depth_stencil_desc.StencilReadMask = 0xFF;
//		depth_stencil_desc.StencilWriteMask = 0xFF;
//		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//	// create constant buffer
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		buffer_desc.ByteWidth = sizeof(cbuffer);
//		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//
//	D3D11_TEXTURE2D_DESC texture2d_desc;
//	for (auto &material : materials)
//	{
//		load_texture_from_file(device, material.map_Kd.c_str(), &material.shader_resource_view, &texture2d_desc);
//	}
//
//	D3D11_SAMPLER_DESC sampler_desc;
//	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
//	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	sampler_desc.MipLODBias = 0;
//	sampler_desc.MaxAnisotropy = 16;
//	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
//	sampler_desc.MinLOD = 0;
//	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//}
//
//static_mesh::~static_mesh()
//{
//
//}
//
//void static_mesh::create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
//{
//	HRESULT hr;
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		D3D11_SUBRESOURCE_DATA subresource_data = {};
//
//		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
//		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		subresource_data.pSysMem = vertices;
//		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
//		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.
//
//		hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//	{
//		D3D11_BUFFER_DESC buffer_desc = {};
//		D3D11_SUBRESOURCE_DATA subresource_data = {};
//
//		buffer_desc.ByteWidth = sizeof(u_int)*num_indices;
//		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
//		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		buffer_desc.CPUAccessFlags = 0;
//		buffer_desc.MiscFlags = 0;
//		buffer_desc.StructureByteStride = 0;
//		subresource_data.pSysMem = indices;
//		subresource_data.SysMemPitch = 0; //Not use for index buffers.
//		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
//		hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//}
//
//void static_mesh::render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe)
//{
//	u_int stride = sizeof(vertex);
//	u_int offset = 0;
//	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
//	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	immediate_context->IASetInputLayout(input_layout.Get());
//
//	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
//	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
//
//	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
//	if (wireframe)
//	{
//		immediate_context->RSSetState(rasterizer_states[1].Get());
//	}
//	else
//	{
//		immediate_context->RSSetState(rasterizer_states[0].Get());
//	}
//
//	for (material &material : materials)
//	{
//		cbuffer data;
//		data.world_view_projection = world_view_projection;
//		data.world = world;
//		data.light_direction = light_direction;
//		data.material_color.x = material_color.x * material.Kd.x;
//		data.material_color.y = material_color.y * material.Kd.y;
//		data.material_color.z = material_color.z * material.Kd.z;
//		data.material_color.w = material_color.w;
//		immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
//		immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
//
//		immediate_context->PSSetShaderResources(0, 1, material.shader_resource_view.GetAddressOf());
//		immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
//		for (subset &subset : subsets)
//		{
//			if (material.newmtl == subset.usemtl)
//			{
//				immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
//			}
//		}
//	}
//}
