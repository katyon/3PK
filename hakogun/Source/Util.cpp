#include	"framework.h"

ID3D11Device*			getDevice()	{
	return	framework::getInstance().getDevice();
};
ID3D11DeviceContext*	getContext(){
	return	framework::getInstance().getContext();
};
bool	SetText(const char* str, float x, float y, float dispSizeW = 0, float dispSizeH = 0)
{
	return	framework::getInstance().SetText(str, x, y, dispSizeW, dispSizeH);
}
