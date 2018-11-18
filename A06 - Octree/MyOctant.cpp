#include "MyOctant.h"
using namespace Simplex;

void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel, uint a_nIdealObjCount)
{

	if (!(ContainsMoreThan(a_nIdealObjCount) && m_uLevel < a_nMaxLevel)) {
		return;
	}
	else {
		Subdivide();

		for (uint i = 0; i < 8; i++)
		{
			m_pChild[i]->ConstructTree(a_nMaxLevel, a_nIdealObjCount);
		}
	}

	if (m_uLevel == 0) {
		ConstructList();
	}

}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	for (int i = 0; i < m_lChild.size(); i++)
	{
		uint entityListSize = m_lChild[i]->m_EntityList.size();
		if (entityListSize != 0) {
			for (uint j = 0; j < entityListSize; j++)
			{

				m_pEntityMngr->GetEntity(m_lChild[i]->m_EntityList[j])->AddDimension(m_lChild[i]->m_uID);
			}
		}
	}
}

void Simplex::MyOctant::RemoveAllIDs(void)
{
	uint entityListSize = m_EntityList.size();
	if (entityListSize != 0) {
		for (uint i = 0; i < entityListSize; i++)
		{

			m_pEntityMngr->GetEntity(m_EntityList[i])->RemoveDimension(m_uID);
		}
	}
}

Simplex::uint Simplex::MyOctant::GetOctantCount(void)
{
	return 0;
}

std::vector<MyOctant*> Simplex::MyOctant::LeafList()
{
	return m_lChild;
}

std::vector<uint> Simplex::MyOctant::EntityList()
{
	return m_EntityList;
}

void Simplex::MyOctant::Release(void)
{
	//m_uOctantCount--;

	if (m_pChild[0] != nullptr) {
		for (uint i = 0; i < 8; i++)
		{
			SafeDelete(m_pChild[i])
		}
	}
}

void Simplex::MyOctant::Init(void)
{
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	//m_uOctantCount++;
}

void Simplex::MyOctant::ConstructList(void)
{
	m_lChild.clear();

	std::deque<MyOctant*> octantStack;
	octantStack.push_back(this);

	while (!octantStack.empty()) {
		MyOctant* temp = octantStack.front();
		octantStack.pop_front();

		if (temp->m_pChild[0] == nullptr) {
			m_lChild.push_back(temp);
		}
		else {
			for (int i = 0; i < 8; i++)
			{
				octantStack.push_back(temp->m_pChild[i]);
			}
		}
	}
}

Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	Init();

	m_pRoot = this;

	//set the boundaries
	uint entityCount = m_pEntityMngr->GetEntityCount();
	MyEntity** entityList = m_pEntityMngr->GetEntityList();

	if (entityCount == 0) {
		return;
	}

	//find maxes
	//set default
	m_v3Max = entityList[0]->GetRigidBody()->GetMaxGlobal();
	m_v3Min = entityList[0]->GetRigidBody()->GetMinGlobal();

	m_EntityList.push_back(0);
	//find max and mins
	for (uint i = 1; i < entityCount; i++)
	{
		//add entity to list
		m_EntityList.push_back(i);

		vector3 tempMax = entityList[i]->GetRigidBody()->GetMaxGlobal();
		vector3 tempMin = entityList[i]->GetRigidBody()->GetMinGlobal();
		

		if (m_v3Max.x < tempMax.x)
			m_v3Max.x = tempMax.x;
		if (m_v3Min.x > tempMin.x)
			m_v3Min.x = tempMin.x;

		if (m_v3Max.y < tempMax.y)
			m_v3Max.y = tempMax.y;
		if (m_v3Min.y > tempMin.y)
			m_v3Min.y = tempMin.y;

		if (m_v3Max.z < tempMax.z)
			m_v3Max.z = tempMax.z;
		if (m_v3Min.z > tempMin.z)
			m_v3Min.z = tempMin.z;
	}

	//make square
	m_v3Max = vector3(glm::max(m_v3Max.x, m_v3Max.y, m_v3Max.z));
	m_v3Min = vector3(glm::min(m_v3Min.x, m_v3Min.y, m_v3Min.z));

	//set center
	m_v3Center = (m_v3Max + m_v3Min) / 2.0f;
	
	//minimum values
	if (a_nIdealEntityCount < 1) {
		a_nIdealEntityCount = 1;
	}
	if (a_nMaxLevel < 1) {
		a_nMaxLevel = 1;
	}

	//then subdivide for the number of times we need?
	//m_uMaxLevel = a_nMaxLevel;
	//m_uIdealEntityCount = a_nIdealEntityCount;
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();

	m_v3Center = a_v3Center;

	m_v3Max = a_v3Center + vector3(a_fSize / 2.0f);
	m_v3Min = a_v3Center - vector3(a_fSize / 2.0f);
}

Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

float Simplex::MyOctant::GetSize(void)
{
	return m_v3Max.x - m_v3Min.x;
}

glm::vec3 Simplex::MyOctant::GetCenterGlobal(void)
{

	return m_v3Center;
}

glm::vec3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

glm::vec3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	vector3 rbMax = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMaxGlobal();
	vector3 rbMin = m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMinGlobal();

	//to right
	if (rbMin.x > m_v3Max.x)
		return false;

	//to left
	if (rbMax.x < m_v3Min.x)
		return false;

	//above
	if (rbMin.y > m_v3Max.y)
		return false;

	//below
	if (rbMax.y < m_v3Min.y) 
		return false;
	
	//front
	if (rbMin.z > m_v3Max.z)
		return false;

	//behind
	if (rbMax.z < m_v3Min.z)
		return false;

	//must be colliding then
	return true;
}

void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (a_nIndex < 0 || a_nIndex >= m_lChild.size()) {
		return;
	}

	matrix4 boxMat = IDENTITY_M4;

	boxMat = glm::translate(boxMat, m_lChild[a_nIndex]->GetCenterGlobal());
	boxMat = glm::scale(boxMat, vector3(m_lChild[a_nIndex]->GetSize()));

	m_pMeshMngr->AddWireCubeToRenderList(boxMat, a_v3Color);
}

void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	matrix4 boxMat = IDENTITY_M4;

	boxMat = glm::translate(boxMat, m_v3Center);
	boxMat = glm::scale(boxMat, vector3(m_v3Max.x - m_v3Min.x));

	m_pMeshMngr->AddWireCubeToRenderList(boxMat, a_v3Color);

	if (m_pChild[0] != nullptr) {
		for (uint i = 0; i < 8; i++)
		{
			m_pChild[i]->Display(a_v3Color);
		}
	}
}

void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	for each (MyOctant* var in m_lChild)
	{
		matrix4 boxMat = IDENTITY_M4;

		boxMat = glm::translate(boxMat, var->GetCenterGlobal());
		boxMat = glm::scale(boxMat, vector3(var->GetSize()));

		m_pMeshMngr->AddWireCubeToRenderList(boxMat, a_v3Color);
	}
}

void Simplex::MyOctant::ClearEntityList(void)
{
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//make sure the children thing is empty
	//replace with kill all branches
	/*if (m_pChild[0] != nullptr) {
		delete[] m_pChild;
	}*/

	//make 8 new children
	m_pChild[0] = new MyOctant(vector3(m_v3Center.x + GetSize() / 4.0, m_v3Center.y + GetSize() / 4.0, m_v3Center.z + GetSize() / 4.0), GetSize() / 2);
	m_pChild[1] = new MyOctant(vector3(m_v3Center.x - GetSize() / 4.0, m_v3Center.y + GetSize() / 4.0, m_v3Center.z + GetSize() / 4.0), GetSize() / 2);
	m_pChild[2] = new MyOctant(vector3(m_v3Center.x + GetSize() / 4.0, m_v3Center.y - GetSize() / 4.0, m_v3Center.z + GetSize() / 4.0), GetSize() / 2);
	m_pChild[3] = new MyOctant(vector3(m_v3Center.x - GetSize() / 4.0, m_v3Center.y - GetSize() / 4.0, m_v3Center.z + GetSize() / 4.0), GetSize() / 2);
	m_pChild[4] = new MyOctant(vector3(m_v3Center.x + GetSize() / 4.0, m_v3Center.y + GetSize() / 4.0, m_v3Center.z - GetSize() / 4.0), GetSize() / 2);
	m_pChild[5] = new MyOctant(vector3(m_v3Center.x - GetSize() / 4.0, m_v3Center.y + GetSize() / 4.0, m_v3Center.z - GetSize() / 4.0), GetSize() / 2);
	m_pChild[6] = new MyOctant(vector3(m_v3Center.x + GetSize() / 4.0, m_v3Center.y - GetSize() / 4.0, m_v3Center.z - GetSize() / 4.0), GetSize() / 2);
	m_pChild[7] = new MyOctant(vector3(m_v3Center.x - GetSize() / 4.0, m_v3Center.y - GetSize() / 4.0, m_v3Center.z - GetSize() / 4.0), GetSize() / 2);


	int idFactor = glm::pow(10, m_uLevel + 1);

	//test all the entities with all the children
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < m_EntityList.size(); j++)
		{
			if (m_pChild[i]->IsColliding(m_EntityList[j]))
				m_pChild[i]->m_EntityList.push_back(m_EntityList[j]);
		}

		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		m_pChild[i]->m_uID = m_uID + idFactor * (i + 1);
	}

}

Simplex::MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (m_pChild[0] == nullptr || a_nChild < 0 || a_nChild >= 8)
		return nullptr;

	return m_pChild[a_nChild];
}

Simplex::MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

bool Simplex::MyOctant::IsLeaf(void)
{
	return m_EntityList.size() != 0;
}

bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	return m_EntityList.size() > a_nEntities;
}

void Simplex::MyOctant::KillBranches(void)
{
	if (m_pChild[0] == nullptr) {
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		/*m_pChild[i]->KillBranches();*/
		SafeDelete(m_pChild[i]);
	}
}


