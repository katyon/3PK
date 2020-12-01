// UNIT.12
#include "static_mesh.h"
#include "shader.h"
#include "misc.h"

#include <fstream>
#include <vector>

// UNIT.13
#include "texture.h"

static_mesh::static_mesh(ID3D11Device *device, const wchar_t *obj_filename, bool flipping_v_coordinates/*UNIT.13*/)
{
	std::vector<vertex> vertices;
	std::vector<u_int> indices;
	u_int current_index = 0;

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> texcoords;
	std::vector<DirectX::XMFLOAT3> normals;

	// UNIT.13
	std::vector<std::wstring> mtl_filenames;

	// parse OBJ file
	{
		std::wifstream fin(obj_filename);
		_ASSERT_EXPR(fin, L"'OBJ file not fuund.");
		wchar_t command[256];
		while (fin)
		{
			fin >> command;
			if (0 == wcscmp(command, L"v"))
			{
				// v x y z w
				// 
				// Specifies a geometric vertex and its x y z coordinates.Rational
				// curves and surfaces require a fourth homogeneous coordinate, also
				// called the weight.
				// 
				// x y z are the x, y, and z coordinates for the vertex.These are
				// floating point numbers that define the position of the vertex in
				// three dimensions.
				// 
				// w is the weight required for rational curves and surfaces.It is
				// not required for non - rational curves and surfaces.If you do not
				// specify a value for w, the default is 1.0.
				float x, y, z;
				fin >> x >> y >> z;
				positions.push_back(DirectX::XMFLOAT3(x, y, z));
				fin.ignore(1024, L'\n');
			}
			// UNIT.13
			else if (0 == wcscmp(command, L"vt"))
			{
				// vt u v w
				// 
				// Specifies a texture vertex and its coordinates.A 1D texture
				// requires only u texture coordinates, a 2D texture requires both u
				// and v texture coordinates, and a 3D texture requires all three
				// coordinates.
				// 
				// u is the value for the horizontal direction of the texture.
				// 
				// v is an optional argument.
				// 
				// v is the value for the vertical direction of the texture.The
				// default is 0.
				// 
				// w is an optional argument.
				// 
				// w is a value for the depth of the texture.The default is 0.
				float u, v;
				fin >> u >> v;
				//texcoords.push_back(DirectX::XMFLOAT2(u, flipping_v_coordinates ? 1.0f - v : v));
				texcoords.push_back(DirectX::XMFLOAT2(u, flipping_v_coordinates ? 1.0f - v : v));
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"vn"))
			{
				// vn i j k
				// 
				// Specifies a normal vector with components i, j, and k.
				// 
				// Vertex normals affect the smooth - shading and rendering of geometry.0
				// For polygons, vertex normals are used in place of the actual facet
				// normals.For surfaces, vertex normals are interpolated over the
				// entire surface and replace the actual analytic surface normal.
				// 
				// When vertex normals are present, they supersede smoothing groups.
				// 
				// i j k are the i, j, and k coordinates for the vertex normal.They
				// are floating point numbers.
				FLOAT i, j, k;
				fin >> i >> j >> k;
				normals.push_back(DirectX::XMFLOAT3(i, j, k));
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"f"))
			{
				//f  v1 / vt1 / vn1   v2 / vt2 / vn2   v3 / vt3 / vn3 . . .
				//
				// optionally include the texture vertex and vertex normal reference
				// numbers.
				// 
				// The reference numbers for the vertices, texture vertices, and
				// vertex normals must be separated by slashes(/ ).There is no space
				// between the number and the slash.
				// 
				// v is the reference number for a vertex in the face element.A
				// minimum of three vertices are required.
				// 
				// vt is an optional argument.
				// 
				// vt is the reference number for a texture vertex in the face
				// element.It always follows the first slash.
				// 
				// vn is an optional argument.
				// 
				// vn is the reference number for a vertex normal in the face element.
				// It must always follow the second slash.
				// 
				// Face elements use surface normals to indicate their orientation.If
				// vertices are ordered counterclockwise around the face, both the
				// face and the normal will point toward the viewer.If the vertex
				// ordering is clockwise, both will point away from the viewer.If
				// vertex normals are assigned, they should point in the general
				// direction of the surface normal, otherwise unpredictable results
				// may occur.
				//
				// If a face has a texture map assigned to it and no texture vertices
				// are assigned in the f statement, the texture map is ignored when
				// the element is rendered.
				for (u_int i = 0; i < 3; i++)
				{
					vertex vertex;
					u_int v, vt, vn;

					fin >> v;
					vertex.position = positions[v - 1];
					if (L'/' == fin.peek())
					{
						fin.ignore();
						if (L'/' != fin.peek())
						{
							fin >> vt;
							// UNIT.13
							vertex.texcoord = texcoords[vt - 1];
						}
						if (L'/' == fin.peek())
						{
							fin.ignore();
							fin >> vn;
							vertex.normal = normals[vn - 1];
						}
					}
					vertices.push_back(vertex);
					indices.push_back(current_index++);
				}
				fin.ignore(1024, '\n');
			}
			// UNIT.13
			else if (0 == wcscmp(command, L"mtllib"))
			{
				// mtllib filename1 filename2 . . .
				// Specifies the material library file for the material definitions
				// set with the usemtl statement.You can specify multiple filenames
				// with mtllib.If multiple filenames are specified, the first file
				// listed is searched first for the material definition, the second
				// file is searched next, and so on.
				wchar_t mtllib[256];
				fin >> mtllib;
				mtl_filenames.push_back(mtllib);
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"usemtl"))
			{
				wchar_t usemtl[MAX_PATH] = { 0 };
				fin >> usemtl;

				subset current_subset = {};
				current_subset.usemtl = usemtl;
				current_subset.index_start = indices.size();
				subsets.push_back(current_subset);
			}
			else
			{
				fin.ignore(1024, '\n');
			}
		}
		fin.close();
	}

	// UNIT.14
	std::vector<subset>::reverse_iterator iterator = subsets.rbegin();
	iterator->index_count = indices.size() - iterator->index_start;
	for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
	{
		iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
	}

	create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
	// UNIT.13
	//std::wstring texture_filename;
	// parse MTL file
	{
		wchar_t mtl_filename[256];
		/*
		e.g.
		obj_filename <= L"data/bison.obj"
		resource_filename <= L"/user/textures/bison.png"
		combined_resource_path => L"/data/bison.png"
		*/
		combine_resource_path(mtl_filename, obj_filename, mtl_filenames[0].c_str());

		std::wifstream fin(mtl_filename);
		_ASSERT_EXPR(fin, L"'MTL file not fuund.");

		wchar_t command[256] = { 0 };
		while (fin)
		{
			fin >> command;
			if (0 == wcscmp(command, L"#"))
			{
				// Comment
				fin.ignore(1024, L'\n');
			}
			else if (0 == wcscmp(command, L"map_Kd"))
			{
				// map_Kd - options args filename
				//
				// Specifies that a color texture file or color procedural texture file is
				// linked to the diffuse reflectivity of the material.During rendering,
				// the map_Kd value is multiplied by the Kd value.
				//
				// "filename" is the name of a color texture file(.mpc), a color
				// procedural texture file(.cxc), or an image file.
				fin.ignore();
				wchar_t map_Kd[256];
				fin >> map_Kd;
				combine_resource_path(map_Kd, obj_filename, map_Kd);
				//texture_filename = map_Kd;
				materials.rbegin()->map_Kd = map_Kd;
				fin.ignore(1024, L'\n');
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"newmtl"))
			{
				// The folowing syntax describes the material name statement.
				//
				//	newmtl name
				//
				// Specifies the start of a material description and assigns a name to the
				// material.An.mtl file must have one newmtl statement at the start of
				// each material description.
				// "name" is the name of the material.Names may be any length but
				// cannot include blanks.Underscores may be used in material names.material material;
				fin.ignore();
				wchar_t newmtl[256];
				material material;
				fin >> newmtl;
				material.newmtl = newmtl;
				materials.push_back(material);
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"Ka"))
			{
				// Ka r g b
				//
				// The Ka statement specifies the ambient reflectivity using RGB values.
				// "r g b" are the values for the red, green, and blue components of the
				// color.The g and b arguments are optional.If only r is specified,
				// then g, and b are assumed to be equal to r.The r g b values are
				// normally in the range of 0.0 to 1.0.Values outside this range increase
				// or decrease the relectivity accordingly.
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->Ka = DirectX::XMFLOAT3(r, g, b);
				fin.ignore(1024, L'\n');
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"Kd"))
			{
				// Kd r g b
				//
				// The Kd statement specifies the diffuse reflectivity using RGB values.
				// "r g b" are the values for the red, green, and blue components of the
				// atmosphere.The g and b arguments are optional.If only r is
				// specified, then g, and b are assumed to be equal to r.The r g b values
				// are normally in the range of 0.0 to 1.0.Values outside this range
				// increase or decrease the relectivity accordingly.
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->Kd = DirectX::XMFLOAT3(r, g, b);
				fin.ignore(1024, L'\n');
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"Ks"))
			{
				// Ks r g b
				//
				// The Ks statement specifies the specular reflectivity using RGB values.
				// "r g b" are the values for the red, green, and blue components of the
				// atmosphere.The g and b arguments are optional.If only r is
				// specified, then g, and b are assumed to be equal to r.The r g b values
				// are normally in the range of 0.0 to 1.0.Values outside this range
				// increase or decrease the relectivity accordingly.
				float r, g, b;
				fin >> r >> g >> b;
				materials.rbegin()->Ks = DirectX::XMFLOAT3(r, g, b);
				fin.ignore(1024, L'\n');
			}
			// UNIT.14
			else if (0 == wcscmp(command, L"illum"))
			{
				// illum illum_#
				//
				// The "illum" statement specifies the illumination model to use in the
				// material.Illumination models are mathematical equations that represent
				// various material lighting and shading effects.
				//
				// "illum_#"can be a number from 0 to 10.  The illumination models are
				// summarized below; 
				// 0		Color on and Ambient off
				// 1		Color on and Ambient on
				// 2		Highlight on
				// 3		Reflection on and Ray trace on
				// 4		Transparency: Glass on
				//  		Reflection : Ray trace on
				// 5		Reflection : Fresnel on and Ray trace on
				// 6		Transparency : Refraction on
				//  		Reflection : Fresnel off and Ray trace on
				// 7 		Transparency : Refraction on
				//  		Reflection : Fresnel on and Ray trace on
				// 8		Reflection on and Ray trace off
				// 9		Transparency : Glass on
				//  		Reflection : Ray trace off
				// 10		Casts shadows onto invisible 
				u_int illum;
				fin >> illum;
				materials.rbegin()->illum = illum;
				fin.ignore(1024, L'\n');
			}
			else
			{
				// Unimplemented or unrecognized command
				fin.ignore(1024, L'\n');
			}
		}
	}

	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// UNIT.13
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.GetAddressOf());

	// create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create rasterizer state : wireframe mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// UNIT.13
	D3D11_TEXTURE2D_DESC texture2d_desc;
	// UNIT.14
	//hr = loat_texture_from_file(device, texture_filename.c_str(), &shader_resource_view, &texture2d_desc);
	for (auto &material : materials)
	{
		load_texture_from_file(device, material.map_Kd.c_str(), &material.shader_resource_view, &texture2d_desc);
	}

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

static_mesh::~static_mesh()
{

}

void static_mesh::create_buffers(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int)*num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void static_mesh::render(ID3D11DeviceContext *immediate_context, const DirectX::XMFLOAT4X4 &world_view_projection, const DirectX::XMFLOAT4X4 &world, const DirectX::XMFLOAT4 &light_direction, const DirectX::XMFLOAT4 &material_color, bool wireframe)
{
	//cbuffer data;
	//data.world_view_projection = world_view_projection;
	//data.world = world;
	//data.light_direction = light_direction;
	//data.material_color = material_color;
	//immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	//immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	u_int stride = sizeof(vertex);
	u_int offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
	if (wireframe)
	{
		immediate_context->RSSetState(rasterizer_states[1].Get());
	}
	else
	{
		immediate_context->RSSetState(rasterizer_states[0].Get());
	}
	// UNIT.13
	//immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	//immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	//D3D11_BUFFER_DESC buffer_desc;
	//index_buffer->GetDesc(&buffer_desc);
	//immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);

	// UNIT.14
	for (material &material : materials)
	{
		cbuffer data;
		data.world_view_projection = world_view_projection;
		data.world = world;
		data.light_direction = light_direction;
		data.material_color.x = material_color.x * material.Kd.x;
		data.material_color.y = material_color.y * material.Kd.y;
		data.material_color.z = material_color.z * material.Kd.z;
		data.material_color.w = material_color.w;
		immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

		immediate_context->PSSetShaderResources(0, 1, material.shader_resource_view.GetAddressOf());
		immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
		for (subset &subset : subsets)
		{
			if (material.newmtl == subset.usemtl)
			{
				immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}

	}
}
