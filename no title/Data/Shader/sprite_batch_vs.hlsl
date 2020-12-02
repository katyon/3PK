#include	"sprite.hlsli"

VS_OUT main( 
	float4                position				: POSITION,
	float2                texcoord				: TEXCOORD,
	column_major float4x4 ndc_transform			: NDC_TRANSFORM,
	float4                texcoord_transform	: TEXCOORD_TRANSFORM,
	float4                color					: COLOR
)
{
	VS_OUT vout;

	vout.pos		= mul(position, ndc_transform);
	vout.color		= color;
	vout.texcoord	= texcoord * texcoord_transform.zw + texcoord_transform.xy;

	return vout;
}