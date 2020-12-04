#include	"geometric_primitive.h"

#include	<vector>
#include	"resource_manager.h"


bool	LoadCSOFile(BYTE** data, long* size, const char* filename);

void GeometricPrimitive::_init(ID3D11Device* device,
	const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
	const char* psName) 
{
	HRESULT hr;

	//	頂点シェーダーオブジェクト & 入力レイアウトの生成
	ResourceManager::LoadVertexShaders(device, vsName, inputElementDescs, numElement, &vertex_shader, &input_layout);

	//	ピクセルシェーダーオブジェクトの生成
	ResourceManager::LoadPixelShaders(device, psName, &pixel_shader);

	//	ラスタライザーステート(ワイヤーフレーム)の作成
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &wireframe_rasterizer_state);
	if( FAILED(hr) )	assert(0&&"ワイヤーフレームラスタライザー作成失敗");

	//	ラスタライザーステート(ソリッド)の作成
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &solid_rasterizer_state);
	if( FAILED(hr) )	assert(0&&"ソリッドラスタライザー作成失敗");



	//	深度ステンシルStateの作成
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory( &dssDesc, sizeof(dssDesc) );
	dssDesc.DepthEnable = TRUE;						//	深度テスト有効
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable = FALSE;					//	ステンシルは無効
	hr = device->CreateDepthStencilState( &dssDesc, &depth_stencil_state );
	if( FAILED( hr ) )		assert(0&&"深度ステンシルState作成失敗");

}



bool	GeometricPrimitive::CreateBuffers(ID3D11Device* device, Vertex* vertices, int numV, unsigned int* indices, int numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA subresource_data;

	//	Vertex Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));
	ZeroMemory(&subresource_data, sizeof(subresource_data));

	buffer_desc.ByteWidth = numV * sizeof(Vertex);
	buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer);
	if( FAILED( hr ) )		return false;


	//	Index Buffer
	if (indices)
	{
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		ZeroMemory(&subresource_data, sizeof(subresource_data));

		buffer_desc.ByteWidth = numI * sizeof(unsigned int);
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;

		numIndices = numI;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &index_buffer);
		if (FAILED(hr))		return false;
	}

	//	Constant Buffer
	ZeroMemory(&buffer_desc, sizeof(buffer_desc));

	buffer_desc.ByteWidth = sizeof(Cbuffer);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&buffer_desc, nullptr, &constant_buffer);
	if( FAILED( hr ) )		return false;

	isMakeBuffer = true;
	return true;
}


GeometricPrimitive::~GeometricPrimitive()
{
	if (constant_buffer)			constant_buffer->Release();
	if (index_buffer)				index_buffer->Release();
	if (vertex_buffer)				vertex_buffer->Release();

	if (depth_stencil_state)		depth_stencil_state->Release();
	if (solid_rasterizer_state)		solid_rasterizer_state->Release();
	if (wireframe_rasterizer_state)	wireframe_rasterizer_state->Release();

	ResourceManager::ReleasePixelShaders(pixel_shader);
	ResourceManager::ReleaseVertexShaders(vertex_shader, input_layout);
}


void GeometricPrimitive::render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color,
	bool bSolid)
{
	//	念の為のエラーチェック
	assert(isMakeBuffer);

	//	定数バッファの作成
	Cbuffer cb;
	cb.wvp = wvp;
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//	インデックスバッファのバインド
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	入力レイアウトのバインド
	context->IASetInputLayout( input_layout );

	//	ラスタライザーの設定
	if(bSolid)	context->RSSetState(solid_rasterizer_state);
	else		context->RSSetState(wireframe_rasterizer_state);

	//	シェーダー(2種)の設定
	context->VSSetShader(vertex_shader, nullptr, 0);
	context->PSSetShader(pixel_shader, nullptr, 0);

	//	深度テストの設定
	context->OMSetDepthStencilState( depth_stencil_state, 0 );

	//	プリミティブの描画(index付き)
	context->DrawIndexed(numIndices, 0, 0);
}


GeometricRect::GeometricRect(ID3D11Device* device) : GeometricPrimitive(device)
{
	Vertex vertices[4] = {};
	unsigned int indices[3*2] = {};


	vertices[0].pos = DirectX::XMFLOAT3(-0.5f, .0f, +0.5f);
	vertices[1].pos = DirectX::XMFLOAT3(+0.5f, .0f, +0.5f);
	vertices[2].pos = DirectX::XMFLOAT3(-0.5f, .0f, -0.5f);
	vertices[3].pos = DirectX::XMFLOAT3(+0.5f, .0f, -0.5f);
	vertices[0].normal =	vertices[1].normal =
	vertices[2].normal =
	vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	CreateBuffers( device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices) );
}


GeometricBoard::GeometricBoard(ID3D11Device* device) : GeometricPrimitive(device)
{
	Vertex vertices[4] = {};
	unsigned int indices[3*2] = {};


	vertices[0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, .0f);
	vertices[1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, .0f);
	vertices[2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, .0f);
	vertices[3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, .0f);
	vertices[0].normal =	vertices[1].normal =
	vertices[2].normal =
	vertices[3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 1;	indices[4] = 3;	indices[5] = 2;

	CreateBuffers( device, vertices, ARRAYSIZE(vertices), indices, ARRAYSIZE(indices) );
}


GeometricCube::GeometricCube(ID3D11Device* device) : GeometricPrimitive(device)
{
	Vertex vertices[4*6] = {};
	unsigned int indices[3*2*6] = {};

	int numV = 0, numI = 0;

	//	上面
	vertices[numV+0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV+0].normal = 	vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+1;	indices[numI+2] = numV+2;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+3;	indices[numI+5] = numV+2;
	numV += 4;	numI += 6;

	//	下面
	vertices[numV+0].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV+0].normal = 	vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+2;	indices[numI+2] = numV+1;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+2;	indices[numI+5] = numV+3;
	numV += 4;	numI += 6;

	//	右面
	vertices[numV+0].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV+0].normal = 		vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+1;	indices[numI+2] = numV+2;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+3;	indices[numI+5] = numV+2;
	numV += 4;	numI += 6;

	//	左面
	vertices[numV+0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV+0].normal = 	vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+2;	indices[numI+2] = numV+1;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+2;	indices[numI+5] = numV+3;
	numV += 4;	numI += 6;

	//	後面
	vertices[numV+0].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[numV+0].normal = 	vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+1;	indices[numI+2] = numV+2;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+3;	indices[numI+5] = numV+2;
	numV += 4;	numI += 6;

	//	前面
	vertices[numV+0].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[numV+1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[numV+2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[numV+3].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[numV+0].normal = 		vertices[numV+1].normal = 
	vertices[numV+2].normal = 
	vertices[numV+3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
	indices[numI+0] = numV+0;	indices[numI+1] = numV+2;	indices[numI+2] = numV+1;
	indices[numI+3] = numV+1;	indices[numI+4] = numV+2;	indices[numI+5] = numV+3;
	numV += 4;	numI += 6;

	CreateBuffers( device, vertices, numV, indices, numI );
}

GeometricSphere::GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks)	:
	GeometricPrimitive(device)
{
	std::vector<Vertex> vertices;
	std::vector<u_int> indices;


	float r = 0.5f;		//	半径 0.5f = 直径 1.0f

	//
	// Compute the vertices stating at the top pole and moving down the stacks.
	//

	// Poles: note that there will be texture coordinate distortion as there is
	// not a unique point on the texture map to assign to the pole when mapping
	// a rectangular texture onto a sphere.
	Vertex top_vertex;
	top_vertex.pos = DirectX::XMFLOAT3(0.0f, +r, 0.0f);
	top_vertex.normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

	Vertex bottom_vertex;
	bottom_vertex.pos = DirectX::XMFLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices.push_back(top_vertex);

	float phi_step = DirectX::XM_PI / stacks;
	float theta_step = 2.0f*DirectX::XM_PI / slices;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (u_int i = 1; i <= stacks - 1; ++i)
	{
		float phi = i*phi_step;
		float rs_phi = r * sinf(phi), rc_phi = r * cosf(phi);

		// Vertices of ring.
		for (u_int j = 0; j <= slices; ++j)
		{
			float theta = j*theta_step;

			Vertex v;

			// spherical to cartesian
			v.pos.x = rs_phi*cosf(theta);
			v.pos.y = rc_phi;
			v.pos.z = rs_phi*sinf(theta);

			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom_vertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//
	for (UINT i = 1; i <= slices; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	u_int base_index = 1;
	u_int ring_vertex_count = slices + 1;
	for (u_int i = 0; i < stacks - 2; ++i)
	{
		u_int i_rvc = i*ring_vertex_count;
		u_int i1_rvc = (i + 1)*ring_vertex_count;

		for (u_int j = 0; j < slices; ++j)
		{
			indices.push_back(base_index + i_rvc  + j);
			indices.push_back(base_index + i_rvc  + j + 1);
			indices.push_back(base_index + i1_rvc + j);

			indices.push_back(base_index + i1_rvc + j);
			indices.push_back(base_index + i_rvc  + j + 1);
			indices.push_back(base_index + i1_rvc + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	u_int south_pole_index = (u_int)vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	base_index = south_pole_index - ring_vertex_count;

	for (u_int i = 0; i < slices; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
	CreateBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

GeometricPrimitive::Vertex GeometricSphere2::_makeVertex(const DirectX::XMFLOAT3& p)
{
	Vertex v;
	float l = sqrtf(p.x*p.x + p.y*p.y + p.z*p.z);
	v.pos = p;
	v.normal.x = p.x / l;
	v.normal.y = p.y / l;
	v.normal.z = p.z / l;
	return v;
}
GeometricSphere2::GeometricSphere2(ID3D11Device* device, u_int div)	:
	GeometricPrimitive(device)
{
	//
	//	参考 : http://rudora7.blog81.fc2.com/blog-entry-390.html
	//

#ifdef D3DD_ASSERT
	if (device == nullptr)	return;
	if (div < 2)			return;
#else
	if (device == nullptr)	assert(0 && "D3D11Device ERROR!");
	if (div < 2)			assert(0 && "Sphere div ERROR!");
#endif // !D3DD_ASSERT


	std::vector<Vertex> sphere;
	int TriangleNum = 0;

	FLOAT Radius	= 0.5f;

	DirectX::XMFLOAT3 temp;
	std::vector<std::vector<DirectX::XMFLOAT3>> iList;
	std::vector<DirectX::XMFLOAT3> tTest;

	float *radians = new float[div];
	for( u_int i=0; i<=div-1; i++ )
		radians[i]	= DirectX::XMConvertToRadians( 90.0f * (div-i-1) / (div-1) );


	int tNum = div;
	int tP = ((tNum - 2)<<2) + 4;
	TriangleNum += ((((tP - 1)<<2) + tP)<<1);
	TriangleNum += ((tNum - 2)<<1) * (tP<<1);
	TriangleNum += (((tNum - 1)<<1) - 1)<<2;


	//一番上の頂点の定義
	temp.x = .0f;
	temp.y = Radius;
	temp.z = .0f;
	tTest.push_back(temp);
	iList.push_back(tTest);
	tTest.clear();

	//半球を定義する頂点を算出
	for(int i = 1, count = tNum; i < count; i++)
	{
		float tRadius = cosf(radians[i]) * Radius;	//円の半径
		float tHeight = sinf(radians[i]) * Radius;	//円のＹ座標
		for(int j = 0, count2 = tNum; j < count2; j++)
		{
			temp.x = tRadius * cosf(radians[j]);
			temp.y = tHeight;
			temp.z = tRadius * sinf(radians[j]);

			tTest.push_back(temp);
		}
		//1象限の頂点をｚ座標反転	（k = 0）
		//4象限の頂点をｘ座標反転	（k = 1）
		//3象限の頂点をｚ座標反転	（k = 2）
		for(int k = 0; k < 3; k++){
			for(int L = 0, Now = tTest.size(), count3 = tNum - 1; L < count3; L++)
			{
				temp = tTest[Now - 2 - L];
				if(k % 2 == 0){
					temp.z = -tTest[Now - 2 - L].z;
				}else{
					temp.x = -tTest[Now - 2 - L].x;
				}
				if(!(k == 2 && L == count3 - 1))
					tTest.push_back(temp);
			}
		}
		iList.push_back(tTest);
		tTest.clear();
	}
	delete[]	radians;

	//半球を定義する頂点を全てＹ座標反転。（境界球全体の頂点が出る）
	for(int i = 1, count = iList.size(); i < count; i++)
	{
		int revi = count - 1 - i;
		for(int j = 0, count2 = iList[revi].size(); j < count2; j++)
		{
			temp = iList[revi][j];
			temp.y = -temp.y;
			tTest.push_back(temp);
		}
		iList.push_back(tTest);
		tTest.clear();
	}


	//真上の頂点１つと、1段下の頂点をつなぐ
	for(int i = 0, count = iList[1].size(); i < count; i++)
	{
		sphere.push_back( _makeVertex(iList[0][0]) );
		sphere.push_back( _makeVertex(iList[1][(i + ( i      & 1)) % count]) );
		sphere.push_back( _makeVertex(iList[1][(i + ((i + 1) & 1)) % count]) );
		sphere.push_back( _makeVertex(iList[1][(i + ((i + 1) & 1)) % count]) );
		if(i != count - 1){
			sphere.push_back( _makeVertex(iList[0][0]));
		}
	}

	//真上と真下以外の頂点をつなぐ
	for(int i = 1, count = iList.size(); i < count - 2; i++)
	{
		sphere.push_back( _makeVertex(iList[i][0]) );
		for(int j = 0, count2 = iList[i].size(); j <= count2; j++)
		{
			sphere.push_back( _makeVertex(iList[i    ][j % count2]) );
			sphere.push_back( _makeVertex(iList[i + 1][j % count2]) );
		}
		sphere.push_back( _makeVertex(iList[i + 1][0]) );
	}

	sphere.push_back( _makeVertex(iList[iList.size() - 1][0]) );

	//真下の頂点１つと、1段上の頂点をつなぐ
	for(int i = 0, count = iList[iList.size() - 2].size(), Num = iList.size(); i < count; i++)
	{
		sphere.push_back( _makeVertex(iList[Num - 1][0]) );
		sphere.push_back( _makeVertex(iList[Num - 2][(i + ((i + 1) & 1)) % count]) );
		sphere.push_back( _makeVertex(iList[Num - 2][(i + ( i      & 1)) % count]) );
		sphere.push_back( _makeVertex(iList[Num - 2][(i + ( i      & 1)) % count]) );
		if(i != count - 1){
			sphere.push_back( _makeVertex(iList[Num - 1][0]) );
		}
	}

	CreateBuffers(device, sphere.data(), sphere.size(), nullptr, 0);
	numVertices = sphere.size();
}

void GeometricSphere2::render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color,
	bool bSolid)
{
	//	念の為のエラーチェック
	assert(isMakeBuffer);

	//	定数バッファの作成
	Cbuffer cb;
	cb.wvp = wvp;
	cb.world = world;
	cb.light_direction = light_direction;
	cb.material_color = material_color;
	context->UpdateSubresource(constant_buffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constant_buffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	////	インデックスバッファのバインド
	//context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//	プリミティブモードの設定(STRIPで行こう！)
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//	入力レイアウトのバインド
	context->IASetInputLayout( input_layout );

	//	ラスタライザーの設定
	if(bSolid)	context->RSSetState(solid_rasterizer_state);
	else		context->RSSetState(wireframe_rasterizer_state);

	//	シェーダー(2種)の設定
	context->VSSetShader(vertex_shader, nullptr, 0);
	context->PSSetShader(pixel_shader, nullptr, 0);

	//	深度テストの設定
	context->OMSetDepthStencilState( depth_stencil_state, 0 );

	//	プリミティブの描画(index付き)
	context->Draw(numVertices, 0);
}
