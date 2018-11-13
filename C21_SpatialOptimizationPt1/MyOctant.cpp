#include "MyOctant.h"
using namespace Simplex;
//  MyOctant
void MyOctant::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();

	std::vector<vector3> v3MaxMin_list;
	for each (MyEntity* var in l_EntityList)
	{
		vector3 v3Max = var->GetRigidBody()->GetMaxGlobal();
		vector3 v3Min = var->GetRigidBody()->GetMinGlobal();
		v3MaxMin_list.push_back(v3Min);
		v3MaxMin_list.push_back(v3Max);
		/*if (v3Position.x < 0.0f)
		{
			if (v3Position.x < -17.0f)
				var->AddDimension(1);
			else
				var->AddDimension(2);
		}
		else if (v3Position.x > 0.0f)
		{
			if (v3Position.x > 17.0f)
				var->AddDimension(3);
			else
				var->AddDimension(4);
		}*/
	}

	m_pRigidBody = new MyRigidBody(v3MaxMin_list);
	
	//DOTHIS
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}

	CalculateCollisions();
	m_nData = 0;
}
void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}
void MyOctant::Release(void)
{
	m_lData.clear();
}
void Simplex::MyOctant::CalculateCollisions(void)
{
	std::vector<MyEntity*> l_EntityList = m_pEntityMngr->GetEntityList();

	for each (MyEntity* var in l_EntityList)
	{
		MyRigidBody* pRB = var->GetRigidBody();

		if (pRB->IsColliding(m_pRigidBody)) {
			var->AddDimension(m_uID);
		}
	}
}
void Simplex::MyOctant::Display(void)
{
	m_pRigidBody->AddToRenderList();
}
void Simplex::MyOctant::Subdivide(void)
{
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = new MyOctant;
	}
}
//The big 3
MyOctant::MyOctant(){Init();}
MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}
MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctant::~MyOctant(){Release();};
//Accessors
void MyOctant::SetData(int a_nData){ m_nData = a_nData; }
int MyOctant::GetData(void){ return m_nData; }
void MyOctant::SetDataOnVector(int a_nData){ m_lData.push_back(a_nData);}
int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}
//--- Non Standard Singleton Methods

