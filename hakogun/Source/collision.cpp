#include	"collision.h"

/*******************************************************************************
	‹…“¯m‚Ì”»’èŠÖ”
*******************************************************************************/
bool	Collision::HitSphere(const DirectX::XMFLOAT3& p1, float r1, const DirectX::XMFLOAT3& p2, float r2)
{
	//	”¼Œa‚Ì‡Z‚Ì2æ
	const float R2 = (r1 + r2) * (r1 + r2);

	//	’†S“¯m‚Ì‹——£‚Ì2æ
	DirectX::XMFLOAT3 vec;
	vec.x = p1.x - p2.x;
	vec.y = p1.y - p2.y;
	vec.z = p1.z - p2.z;
	float L2 = vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;

	//	Õ“Ë”»’è
	if (L2 < R2)	return	true;

	return	false;
}
