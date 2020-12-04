//#include "shader.h"
//#include "texture.h"
//#include "misc.h"
//#include "skinned_mesh.h"
//
//#include "FBXSDK/include/fbxsdk.h"
////#include <fbxsdk.h>
//using namespace fbxsdk;
//
//#include <vector>
//#include <functional>
//
//void fbxamatrix_to_xmfloat4x4(const FbxAMatrix &fbxamatrix, DirectX::XMFLOAT4X4 &xmfloat4x4)
//{
//	for (int row = 0; row < 4; row++)
//	{
//		for (int column = 0; column < 4; column++)
//		{
//			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
//		}
//	}
//}
//
//skinned_mesh::skinned_mesh(ID3D11Device *device, const char *fbx_filename)
//{
//	FbxManager* manager = FbxManager::Create();
//
//	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
//
//	FbxImporter* importer = FbxImporter::Create(manager, "");
//
//	bool import_status = false;
//	import_status = importer->Initialize(fbx_filename, -1, manager->GetIOSettings());
//	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());
//
//	FbxScene* scene = FbxScene::Create(manager, "");
//
//	import_status = importer->Import(scene);
//	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());
//
//	fbxsdk::FbxGeometryConverter geometry_converter(manager);
//	geometry_converter.Triangulate(scene, true);
//
//	std::vector<FbxNode*> fetched_meshes;
//	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
//	{
//		if (node)
//		{
//			FbxNodeAttribute *fbx_node_attribute = node->GetNodeAttribute();
//			if (fbx_node_attribute)
//			{
//				switch (fbx_node_attribute->GetAttributeType())
//				{
//				case FbxNodeAttribute::eMesh:
//					fetched_meshes.push_back(node);
//					break;
//				}
//			}
//			for (int i = 0; i < node->GetChildCount(); i++)
//			{
//				traverse(node->GetChild(i));
//			}
//		}
//	};
//	traverse(scene->GetRootNode());
//
//	meshes.resize(fetched_meshes.size());
//
//	for (size_t i = 0; i < fetched_meshes.size(); i++)
//	{
//		FbxMesh *fbx_mesh = fetched_meshes.at(i)->GetMesh();
//		mesh &mesh = meshes.at(i);
//
//		FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
//		fbxamatrix_to_xmfloat4x4(global_transform, mesh.global_transform);
//		
//		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
//
//		mesh.subsets.resize(number_of_materials > 0 ? number_of_materials : 1);
//		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
//		{
//			subset &subset = mesh.subsets.at(index_of_material);
//
//			const FbxSurfaceMaterial *surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);
//
//			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
//			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
//			if (property.IsValid() && factor.IsValid())
//			{
//				FbxDouble3 color = property.Get<FbxDouble3>();
//				double f = factor.Get<FbxDouble>();
//				subset.diffuse.color.x = static_cast<float>(color[0] * f);
//				subset.diffuse.color.y = static_cast<float>(color[1] * f);
//				subset.diffuse.color.z = static_cast<float>(color[2] * f);
//				subset.diffuse.color.w = 1.0f;
//			}
//			if (property.IsValid())
//			{
//				const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
//				if (number_of_textures)
//				{
//					const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
//					if (file_texture)
//					{
//						const char *filename = file_texture->GetRelativeFileName();
//
//						wchar_t fbx_unicode[256];
//						MultiByteToWideChar(CP_ACP, 0, fbx_filename, strlen(fbx_filename) + 1, fbx_unicode, 1024);
//						wchar_t texture_unicode[256];
//						MultiByteToWideChar(CP_ACP, 0, file_texture->GetFileName(), strlen(file_texture->GetFileName()) + 1, texture_unicode, 1024);
//						combine_resource_path(texture_unicode, fbx_unicode, texture_unicode);
//
//						D3D11_TEXTURE2D_DESC texture2d_desc;
//						load_texture_from_file(device, texture_unicode, subset.diffuse.shader_resource_view.GetAddressOf(), &texture2d_desc);
//					}
//				}
//			}
//		}
//		for (subset &subset : mesh.subsets)
//		{
//			if (!subset.diffuse.shader_resource_view)
//			{
//				make_dummy_texture(device, subset.diffuse.shader_resource_view.GetAddressOf());
//			}
//		}
//
//		if (number_of_materials > 0)
//		{
//			const int number_of_polygons = fbx_mesh->GetPolygonCount();
//			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
//			{
//				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
//				mesh.subsets.at(material_index).index_count += 3;
//
//			}
//
//			int offset = 0;
//			for (subset &subset : mesh.subsets)
//			{
//				subset.index_start = offset;
//				offset += subset.index_count;
//				subset.index_count = 0;
//			}
//		}
//
//		std::vector<vertex> vertices;
//		std::vector<u_int> indices;
//		u_int vertex_count = 0;
//
//		FbxStringList uv_names;
//		fbx_mesh->GetUVSetNames(uv_names);
//
//		const FbxVector4 *array_of_control_points = fbx_mesh->GetControlPoints();
//		const int number_of_polygons = fbx_mesh->GetPolygonCount();
//		indices.resize(number_of_polygons * 3);
//
//		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
//		{
//			int index_of_material = 0;
//			if (number_of_materials > 0)
//			{
//				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
//			}
//
//			subset &subset = mesh.subsets.at(index_of_material);
//			const int index_offset = subset.index_start + subset.index_count;
//
//			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
//			{
//				vertex vertex;
//				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
//				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
//				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
//				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
//
//				if (fbx_mesh->GetElementNormalCount() > 0)
//				{
//					FbxVector4 normal;
//					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
//					vertex.normal.x = static_cast<float>(normal[0]);
//					vertex.normal.y = static_cast<float>(normal[1]);
//					vertex.normal.z = static_cast<float>(normal[2]);
//				}
//
//				if (fbx_mesh->GetElementUVCount() > 0)
//				{
//					FbxVector2 uv;
//					bool unmapped_uv;
//					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
//					vertex.texcoord.x = static_cast<float>(uv[0]);
//					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
//				}
//
//				vertices.push_back(vertex);
//
//				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
//				vertex_count += 1;
//			}
//			subset.index_count += 3;
//		}
//		mesh.create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
//	}
//	manager->Destroy();
//
//	HRESULT hr = S_OK;
//
//	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//	create_vs_from_cso(device, "skinned_mesh_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
//	create_ps_from_cso(device, "skinned_mesh_ps.cso", pixel_shader.GetAddressOf());
//
//	// create rasterizer state : solid mode
//	{
//		D3D11_RASTERIZER_DESC rasterizer_desc = {};
//		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
//		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
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
//		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
//		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
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
//	// create sampler state
//	D3D11_SAMPLER_DESC sampler_desc = {};
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
//void skinned_mesh::mesh::create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
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
//void skinned_mesh::render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world_inverse_transpose, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe)
//{
//	for (mesh &mesh : meshes)
//	{
//		u_int stride = sizeof(vertex);
//		u_int offset = 0;
//		//immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
//		//immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
//		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		immediate_context->IASetInputLayout(input_layout.Get());
//
//		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
//		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
//
//		immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
//		if (wireframe)
//		{
//			immediate_context->RSSetState(rasterizer_states[1].Get());
//		}
//		else
//		{
//			immediate_context->RSSetState(rasterizer_states[0].Get());
//		}
//
//		cbuffer data;
//		DirectX::XMStoreFloat4x4(&data.world_view_projection, DirectX::XMLoadFloat4x4(&mesh.global_transform)* DirectX::XMLoadFloat4x4(&world_view_projection));
//		DirectX::XMStoreFloat4x4(&data.world_inverse_transpose, DirectX::XMLoadFloat4x4(&mesh.global_transform) * DirectX::XMLoadFloat4x4(&world_inverse_transpose));
//
//		data.light_direction = light_direction;
//
//		for (subset &subset : mesh.subsets)
//		{
//			data.material_color.x = subset.diffuse.color.x * material_color.x;
//			data.material_color.y = subset.diffuse.color.y * material_color.y;
//			data.material_color.z = subset.diffuse.color.z * material_color.z;
//			data.material_color.w = material_color.w;
//			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
//			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
//
//			immediate_context->PSSetShaderResources(0, 1, subset.diffuse.shader_resource_view.GetAddressOf());
//			immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
//			immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
//		}
//	}
//}