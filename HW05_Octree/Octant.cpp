#include "Octant.h"
using namespace BTX;
//  Octant
uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	/*
	* This constructor is meant to be used ONLY on the root node, there is already a working constructor
	* that will take a size and a center to create a new space
	*/
	Init();//Init the default values
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	// Calculate the minimum and maximum of all the rigid objects
	vector3 v3MinBounds(FLT_MAX);
	vector3 v3MaxBounds(-FLT_MAX);

	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); ++i)
	{
		RigidBody* pRigidBody = m_pEntityMngr->GetRigidBody(i);

		if (pRigidBody)
		{
			vector3 v3RBMin = pRigidBody->GetMinGlobal();
			vector3 v3RBMax = pRigidBody->GetMaxGlobal();

			v3MinBounds = glm::min(v3MinBounds, v3RBMin);
			v3MaxBounds = glm::max(v3MaxBounds, v3RBMax);
		}
	}

	// Create a rigid body that holds all the objects
	RigidBody pRigidBody = RigidBody({ v3MinBounds, v3MaxBounds });

	// Set up the values of the octant based on the calculated rigid body
	m_fSize = pRigidBody.GetHalfWidth().x * 2.0f;
	m_v3Center = pRigidBody.GetCenterLocal();
	m_v3Min = m_v3Center - pRigidBody.GetHalfWidth();
	m_v3Max = m_v3Center + pRigidBody.GetHalfWidth();

	//When we add an octant we increment the count
	m_uOctantCount++;
	// Construct the children
	ConstructTree(m_uMaxLevel);
}

bool Octant::IsColliding(uint a_uRBIndex)
{
	// Get the rigid body with the specified index
	RigidBody* pRigidBody = m_pEntityMngr->GetRigidBody(a_uRBIndex);

	// Checks for a rigid body
	if (pRigidBody)
	{
		// Get the minimum and maximum points of the rigid body's bounding box in global space
		vector3 v3RBMin = pRigidBody->GetMinGlobal();
		vector3 v3RBMax = pRigidBody->GetMaxGlobal();

		// Check for collision in all three dimensions
		bool xCollide = m_v3Min.x <= v3RBMax.x && m_v3Max.x >= v3RBMin.x;
		bool yCollide = m_v3Min.y <= v3RBMax.y && m_v3Max.y >= v3RBMin.y;
		bool zCollide = m_v3Min.z <= v3RBMax.z && m_v3Max.z >= v3RBMin.z;

		// If there is a collision in all dimensions, the octant, and the rigid body are colliding
		return xCollide && yCollide && zCollide;
	}

	// If the rigid body is not valid, it's not colliding
	return false;
}

void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	// Check if the index is within range
	if (m_uID == a_nIndex)
	{
		m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(m_fSize)), a_v3Color);

		return;
	}

	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_nIndex);
	}
}

void Octant::Display(vector3 a_v3Color)
{
	//A recursive method
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->Display(a_v3Color);
	}
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
}

void Octant::Subdivide(void)
{
	//If this node has reach the maximum, it will be returned
	if (m_uLevel >= m_uMaxLevel)
		return;

	//If this node has been already subdivided, it will be returned
	if (m_uChildren != 0)
		return;

	//Subdivides the space and sets up the 8 children
	m_uChildren = 8;

	float fSize = m_fSize / 4.0f;
	float fSizeD = fSize * 2.0f;
	vector3 v3Center;

	v3Center = m_v3Center;
	v3Center.x -= fSize;
	v3Center.y -= fSize;
	v3Center.z -= fSize;
	m_pChild[0] = new Octant(v3Center, fSizeD);

	//Octant 1 Right bottom back
	v3Center.x += fSizeD;
	m_pChild[1] = new Octant(v3Center, fSizeD);

	//Octant 2 right bottom front
	v3Center.z += fSizeD;
	m_pChild[2] = new Octant(v3Center, fSizeD);

	//Octant 3 left bottom front
	v3Center.x -= fSizeD;
	m_pChild[3] = new Octant(v3Center, fSizeD);

	//Octant 4 left top front
	v3Center.y += fSizeD;
	m_pChild[4] = new Octant(v3Center, fSizeD);

	//Octant 5 left top back
	v3Center.z -= fSizeD;
	m_pChild[5] = new Octant(v3Center, fSizeD);

	//Octant 6 right top back
	v3Center.x += fSizeD;
	m_pChild[6] = new Octant(v3Center, fSizeD);

	//Octant 7 right top front
	v3Center.z += fSizeD;
	m_pChild[7] = new Octant(v3Center, fSizeD);

	for (uint nIndex = 0; nIndex < 8; nIndex++)
	{
		m_pChild[nIndex]->m_pRoot = m_pRoot;
		m_pChild[nIndex]->m_pParent = this;
		m_pChild[nIndex]->m_uLevel = m_uLevel + 1;
		if (m_pChild[nIndex]->ContainsAtLeast(m_uIdealEntityCount))
		{
			m_pChild[nIndex]->Subdivide();
		}
	}
}

bool Octant::ContainsAtLeast(uint a_nEntities)
{
	uint nCount = 0;
	uint nObjectCount = m_pEntityMngr->GetEntityCount();

    // Iterate through the entities and count them
	for (uint n = 0; n < nObjectCount; n++)
	{
		if (IsColliding(n))
		{
			nCount++;
		}
		if (nCount > a_nEntities)
		{
			return true;
		}
	}
	
	// Will return false if no entities are contained
	return false;
}

void Octant::AssignIDtoEntity(void)
{
	// Traverses the tree and tells the entity manager what octant (space) each object is at
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->AssignIDtoEntity();
	}
	if (m_uChildren += 0)
	{
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (uint nIndex = 0; nIndex < nEntities; nIndex++)
		{
			if (IsColliding(nIndex))
			{
				m_EntityList.push_back(nIndex);
				m_pEntityMngr->AddDimension(nIndex, m_uID);
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
// You can assume the following is fine and does not need changes, you may add onto it but the code is fine as is
// in the proposed solution.
void Octant::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}
void Octant::Swap(Octant& other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
void Octant::Release(void)
{
	//this is a special kind of release, it will only happen for the root
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}
void Octant::ConstructTree(uint a_nMaxLevel)
{
	//If this method is tried to be applied to something else
	//other than the root, don't.
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel; //Make sure you mark which is the maximum level you are willing to reach
	m_uOctantCount = 1;// We will always start with one octant

	//If this was initialized before make sure its empty
	m_EntityList.clear();//Make sure the list of entities inside this octant is empty
	KillBranches();
	m_lChild.clear();

	//If we have more entities than those that we ideally want in this octant we subdivide it
	if (ContainsAtLeast(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();//Add octant ID to Entities
	ConstructList();//construct the list of objects
}
//The big 3
Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	//Will create the octant object based on the center and size but will not construct children
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}
Octant::Octant(Octant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}
Octant& Octant::operator=(Octant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}
	return *this;
}
Octant::~Octant() { Release(); };
//Accessors
float Octant::GetSize(void) { return m_fSize; }
vector3 Octant::GetCenterGlobal(void) { return m_v3Center; }
vector3 Octant::GetMinGlobal(void) { return m_v3Min; }
vector3 Octant::GetMaxGlobal(void) { return m_v3Max; }
uint Octant::GetOctantCount(void) { return m_uOctantCount; }
bool Octant::IsLeaf(void) { return m_uChildren == 0; }
Octant* Octant::GetParent(void) { return m_pParent; }
//--- other methods
Octant * Octant::GetChild(uint a_nChild)
{
	//if its asking for more than the 8th children return nullptr, as we don't have one
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}
void Octant::KillBranches(void)
{
	/*This method has recursive behavior that is somewhat hard to explain line by line but
	it will traverse the whole tree until it reaches a node with no children and
	once it returns from it to its parent it will kill all of its children, recursively until
	it reaches back the node that called it.*/
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}
void Octant::DisplayLeaves(vector3 a_v3Color)
{
	/*
	* Recursive method
	* it will traverse the tree until it find leaves and once found will draw them
	*/
	uint nLeafs = m_lChild.size(); //get how many children this will have (either 0 or 8)
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeaves(a_v3Color);
	}
	//Draw the cube
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
}
void Octant::ClearEntityList(void)
{
	/*
	* Recursive method
	* It will traverse the tree until it find leaves and once found will clear its content
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}
void Octant::ConstructList(void)
{
	/*
	* Recursive method
	* It will traverse the tree adding children
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}
	//if we find a non-empty child add it to the tree
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
