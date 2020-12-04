#include	"stage.h"

void Stage::Initialize(const char* filename)
{
	obj.Initialize();
	obj.Load(filename);

	exist = true;
}

void Stage::Release()
{
	obj.Release();
}

void Stage::Update()
{

}

void Stage::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
	if (exist)
	{
		obj.pos = this->pos;
		obj.angle.y = this->angle;
		obj.Render(view, projection, light_dir);
	}
}