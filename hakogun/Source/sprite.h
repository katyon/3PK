#pragma once

#include	<d3d11.h>
#include	<DirectXMath.h>

class Sprite
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};


	ID3D11VertexShader*			vertex_shader;
	ID3D11PixelShader*			pixel_shader;
	ID3D11InputLayout*			input_layout;
	ID3D11Buffer*				vertex_buffer;
	ID3D11RasterizerState*		rasterizer_state;

	ID3D11ShaderResourceView*	shader_resource_view;
	D3D11_TEXTURE2D_DESC*		texture2d_desc;
	ID3D11SamplerState*			sampler_state;

	ID3D11DepthStencilState*	depth_stencil_state;

	void _init(ID3D11Device* device, const wchar_t* texName,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName);

public:
	Sprite(ID3D11Device* device, const wchar_t* texName) :
		vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr),
		vertex_buffer(nullptr), rasterizer_state(nullptr),
		shader_resource_view(nullptr), sampler_state(nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC	layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                                                   D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::XMFLOAT3),                           D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, sizeof(DirectX::XMFLOAT3)+sizeof(DirectX::XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		const char* vsName = "./Data/Shader/sprite_vs.cso";
		const char* psName = "./Data/Shader/sprite_ps.cso";

		_init(device, texName, vsName, layout, ARRAYSIZE(layout), psName);
	}
	Sprite(ID3D11Device* device, const wchar_t* texName,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName) :
		vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr),
		vertex_buffer(nullptr), rasterizer_state(nullptr),
		shader_resource_view(nullptr), sampler_state(nullptr)
	{
		_init(device, texName, vsName, inputElementDescs, numElement, psName);
	}

	virtual ~Sprite();

	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(1,1,1,1)
	);
	void Render(ID3D11DeviceContext* context,
		float dx, float dy, float dw, float dh,
		float sx, float sy, float sw, float sh,
		float angle,
		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(1, 1, 1, 1)
	);
};

class SpriteBatch
{
protected:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;
		DirectX::XMFLOAT4 texcoord_transform;
		DirectX::XMFLOAT4 color;
	};

	const u_int MAX_INSTANCE;

	ID3D11VertexShader*			vertex_shader;
	ID3D11PixelShader*			pixel_shader;
	ID3D11InputLayout*			input_layout;
	ID3D11Buffer*				vertex_buffer;
	ID3D11RasterizerState*		rasterizer_state;

	ID3D11ShaderResourceView*	shader_resource_view;
	D3D11_TEXTURE2D_DESC*		texture2d_desc;
	ID3D11SamplerState*			sampler_state;

	ID3D11DepthStencilState*	depth_stencil_state;

	ID3D11Buffer*				instance_buffer;
	Instance*					instances;
	u_int						reserveNum;

	void _init(ID3D11Device* device, const wchar_t* texName,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName, u_int maxInstance);

public:
	SpriteBatch(u_int maxInstance = 256) :
		MAX_INSTANCE(maxInstance),
		vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr),
		vertex_buffer(nullptr), rasterizer_state(nullptr),
		shader_resource_view(nullptr), sampler_state(nullptr),
		instance_buffer(nullptr), instances(nullptr), reserveNum(0)
	{};
	SpriteBatch(ID3D11Device* device, const wchar_t* texName, u_int maxInstance = 256) :
		MAX_INSTANCE(maxInstance),
		vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr),
		vertex_buffer(nullptr), rasterizer_state(nullptr),
		shader_resource_view(nullptr), sampler_state(nullptr),
		instance_buffer(nullptr), instances(nullptr), reserveNum(0)
	{
		D3D11_INPUT_ELEMENT_DESC	layout[] =
		{
			{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		const char* vsName = "./Data/Shader/sprite_batch_vs.cso";
		const char* psName = "./Data/Shader/sprite_ps.cso";

		_init(device, texName, vsName, layout, ARRAYSIZE(layout), psName, maxInstance);
	}
	SpriteBatch(ID3D11Device* device, const wchar_t* texName,
		const char* vsName, D3D11_INPUT_ELEMENT_DESC *inputElementDescs, int numElement,
		const char* psName, u_int maxInstance = 256) :
		MAX_INSTANCE(maxInstance),
		vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr),
		vertex_buffer(nullptr), rasterizer_state(nullptr),
		shader_resource_view(nullptr), sampler_state(nullptr),
		instance_buffer(nullptr), instances(nullptr), reserveNum(0)
	{
		_init(device, texName, vsName, inputElementDescs, numElement, psName, maxInstance);
	}

	virtual ~SpriteBatch();

	bool	Reserve(float dx, float dy, float dw, float dh,
			float sx, float sy, float sw, float sh,
			float angle = .0f,
			DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f));
	bool	Reserve(float dx, float dy, float dw, float dh,
			float sx, float sy, float sw, float sh,
			float angle,
			float r, float g, float b, float a = 1.0f)
	{
		return	Reserve(dx, dy, dw, dh, sx, sy, sw, sh, angle, DirectX::XMFLOAT4(r, g, b, a));
	}

	//	ó\ñÒÇ≥ÇÍÇΩãÈå`åQÇï`âÊ + ó\ñÒÉäÉXÉgÇÃÉäÉZÉbÉg
	void Render(ID3D11DeviceContext* context);

};



class Text : public SpriteBatch
{
private:
	static const u_int MAX_INSTANCES = 2048;		//	ï\é¶Ç≈Ç´ÇÈï∂éöêî ÅÇ ï∂èÕêî
	u_int	IMAGE_FONT_SIZE_W;
	u_int	IMAGE_FONT_SIZE_H;
	u_int	NUM_OF_1_LINE;

public:
	Text():SpriteBatch(MAX_INSTANCES),IMAGE_FONT_SIZE_W(0),NUM_OF_1_LINE(0){};
	Text(ID3D11Device* device, const wchar_t* texName, u_int imageFontSizeW, u_int imageFontSizeH);
	virtual ~Text(){	};

	bool	Initialize(ID3D11Device* device, const wchar_t* texName, u_int imageFontSizeW, u_int imageFontSizeH=0)
	{
		D3D11_INPUT_ELEMENT_DESC	layout[] =
		{
			{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
			{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};
		const char* vsName = "./Data/Shader/sprite_batch_vs.cso";
		const char* psName = "./Data/Shader/sprite_ps.cso";

		_init(device, texName, vsName, layout, ARRAYSIZE(layout), psName, MAX_INSTANCES);

		IMAGE_FONT_SIZE_W = imageFontSizeW;
		IMAGE_FONT_SIZE_H = imageFontSizeH;
		if (IMAGE_FONT_SIZE_H <= 0)	IMAGE_FONT_SIZE_H = IMAGE_FONT_SIZE_W;
		NUM_OF_1_LINE = texture2d_desc->Width / imageFontSizeW;

		return	true;
	}

	bool	Set(const char* str, float x, float y, float dispSizeW = 0, float dispSizeH = 0);

};
