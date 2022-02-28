/*
 当たり判定を行うためのクラス
*/
#include"Collision.h"
#include<vector>

Collision::Collision()
{
	_weight = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); 

	for (int i = 0; i < 4; ++i)
	{
		_moveDict.push_back(true); 
	}
	
}

float Collision::XminBoxCol(boxCollider col)
{
	float minx = col.P1.x; 
	minx = min(minx, col.P2.x); 
	minx = min(minx, col.P3.x); 
	minx = min(minx, col.P4.x); 

	minx = min(minx, col.P5.x); 
	minx - min(minx, col.P6.x); 
	minx = min(minx, col.P7.x); 
	minx = min(minx, col.P8.x); 

	return minx; 
}

float Collision::YminBoxCol(boxCollider col)
{
	float miny = col.P1.y; 
	miny = min(miny, col.P2.y);
	miny = min(miny, col.P3.y);
	miny = min(miny, col.P4.y);

	miny = min(miny, col.P5.y);
	miny = min(miny, col.P6.y);
	miny = min(miny, col.P7.y);
	miny = min(miny, col.P8.y);

	return miny; 
}

float Collision::ZminBoxCol(boxCollider col)
{
	float minz = col.P1.z; 
	minz = min(minz, col.P2.z);
	minz = min(minz, col.P3.z);
	minz = min(minz, col.P4.z);

	minz = min(minz, col.P5.z);
	minz = min(minz, col.P6.z);
	minz = min(minz, col.P7.z);
	minz = min(minz, col.P8.z);

	return minz;
}

float Collision::XmaxBoxCol(boxCollider col)
{
	float maxx = col.P1.x;
	maxx = max(maxx, col.P2.x);
	maxx = max(maxx, col.P3.x);
	maxx = max(maxx, col.P4.x);

	maxx = max(maxx, col.P5.x);
	maxx - max(maxx, col.P6.x);
	maxx = max(maxx, col.P7.x);
	maxx = max(maxx, col.P8.x);

	return maxx;
}

float Collision::YmaxBoxCol(boxCollider col)
{
	float maxy = col.P1.y;
	maxy = max(maxy, col.P2.y);
	maxy = max(maxy, col.P3.y);
	maxy = max(maxy, col.P4.y);

	maxy = max(maxy, col.P5.y);
	maxy - max(maxy, col.P6.y);
	maxy = max(maxy, col.P7.y);
	maxy = max(maxy, col.P8.y);

	return maxy;
}

float Collision::ZmaxBoxCol(boxCollider col)
{
	float maxz = col.P1.z; 
	maxz = max(maxz, col.P2.z); 
	maxz = max(maxz, col.P3.z);
	maxz = max(maxz, col.P4.z);

	maxz = max(maxz, col.P5.z);
	maxz = max(maxz, col.P6.z);
	maxz = max(maxz, col.P7.z);
	maxz = max(maxz, col.P8.z);

	return maxz; 
}

bool Collision::PosCollisionEnter(DirectX::XMFLOAT3 P, boxCollider target)
{
	float xmin = XminBoxCol(target);
	float xmax = XmaxBoxCol(target);
	float ymin = YminBoxCol(target);
	float ymax = YmaxBoxCol(target);
	float zmin = ZminBoxCol(target);
	float zmax = ZmaxBoxCol(target);

	if (xmin <= P.x && P.x <= xmax)
	{
		if (ymin <= P.y && P.y <= ymax)
		{
			if (zmin <= P.z && P.z <= zmax)
			{
				return true;
			}
		}
	}
	return false; 
}

bool Collision::BoxCollisionEnter(boxCollider col, boxCollider target)
{
	if (PosCollisionEnter(col.P1, target)) return true; 
	if (PosCollisionEnter(col.P2, target)) return true;
	if (PosCollisionEnter(col.P3, target)) return true;
	if (PosCollisionEnter(col.P4, target)) return true;
	
	if (PosCollisionEnter(col.P5, target)) return true;
	if (PosCollisionEnter(col.P6, target)) return true;
	if (PosCollisionEnter(col.P7, target)) return true;
	if (PosCollisionEnter(col.P8, target)) return true;
			
	return false; 
}


boxCollider boxColNext(boxCollider col, int dict)
{
	float threshold = 10.0f;
	if (dict == 0)
	{
		col.P1.x += threshold; 
		col.P2.x += threshold; 
		col.P3.x += threshold;
		col.P4.x += threshold;

		col.P5.x += threshold;
		col.P6.x += threshold;
		col.P7.x += threshold;
		col.P8.x += threshold;
	}
	else if (dict == 1)
	{
		col.P1.x -= threshold;
		col.P2.x -= threshold;
		col.P3.x -= threshold;
		col.P4.x -= threshold;

		col.P5.x -= threshold;
		col.P6.x -= threshold;
		col.P7.x -= threshold;
		col.P8.x -= threshold;
	}
	else if (dict == 2)
	{
		col.P1.z += threshold;
		col.P2.z += threshold;
		col.P3.z += threshold;
		col.P4.z += threshold;

		col.P5.z += threshold;
		col.P6.z += threshold;
		col.P7.z += threshold;
		col.P8.z += threshold;
	}
	else if (dict == 3)
	{
		col.P1.z -= threshold;
		col.P2.z -= threshold;
		col.P3.z -= threshold;
		col.P4.z -= threshold;

		col.P5.z -= threshold;
		col.P6.z -= threshold;
		col.P7.z -= threshold;
		col.P8.z -= threshold;
	}
	return col;
}

/*----オブジェクトが衝突したか否かを返す関数------*/
bool Collision::OnCollisionEnter(boxCollider col1, boxCollider col2)
{

	if (BoxCollisionEnter(col1, col2)) return true; 
	if (BoxCollisionEnter(col2, col1)) return true; 
	
	return false; 
}

void Collision::InitDict()
{
	for (int i = 0; i < 4; ++i)
	{
		_moveDict[i] = true; 
	}
}

void Collision::NextCollision(boxCollider col1, boxCollider col2)
{
	// 0 -> +x 
	// 1 -> -x 
	// 2 -> +z
	// 3 -> -z 
	for (int i = 0; i < 4; ++i)
	{
		if (OnCollisionEnter(boxColNext(col1, i), col2))
		{
			_moveDict[i] = false;
		}
	}
}