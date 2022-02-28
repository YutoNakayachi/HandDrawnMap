#pragma once

#include"SharedStruct.h"
#include"BoxCollider.h"

class Collision
{
private:
	float XminBoxCol(boxCollider col); 
	float YminBoxCol(boxCollider col); 
	float ZminBoxCol(boxCollider col);

	float XmaxBoxCol(boxCollider col);
	float YmaxBoxCol(boxCollider col);
	float ZmaxBoxCol(boxCollider col);

	bool PosCollisionEnter(DirectX::XMFLOAT3 P, boxCollider target); 
	bool BoxCollisionEnter(boxCollider col, boxCollider target); 

public:
	Collision(); 

	void InitDict(); 

	bool OnCollisionEnter(boxCollider col1, boxCollider col2); 

	void NextCollision(boxCollider col1, boxCollider col2); 

	DirectX::XMFLOAT3 _weight; 

	std::vector<bool> _moveDict; 
};