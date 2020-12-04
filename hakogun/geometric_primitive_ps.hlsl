#include "geometric_primitive.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	if (pin.color.w <= 0)	discard;
	return pin.color;
}