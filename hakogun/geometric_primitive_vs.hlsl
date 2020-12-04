#include "geometric_primitive.hlsli"


VS_OUT main(float4 pos : POSITION, float4 normal : NORMAL)
{
	VS_OUT vout;

	vout.position = mul(pos, wvp);

	normal.w = 0;
	float4 N = normalize(mul(normal, world));
	float4 L = normalize(-light_direction);

	vout.color = material_color * max(0, dot(L, N));
	vout.color.a = material_color.a;

	return vout;
}