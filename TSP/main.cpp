#include <iostream>
#include <vector>
#include <climits>
#include <map>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

const int m_size{ 5 };
vector<bool> unvisited; // sprawdzic czy da sie zainicjowac
vector<int> memo;
map<int, bool> visited_map;
int UP;

int matrix[][5] = {
	{  -1,10,8,9,7  },
	{  10,-1,10,5,6 },
	{  8,10,-1,8,9  },
	{  9,5,8,-1,6   },
	{  7,6,9,6,-1   }
};


void printMatrix(int m[][5])
{
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
		{
			cout << m[i][j];

			if (j == 4)
				cout << endl;
		}
}


class GTNode
{
public:

	int lb = 0;
	int actual_cost = 0;
	int level = 0;
	// first int - which city, second int - on which position
	map<int, int> visited; // sprawdziæ czy da sie zainicjowac
	GTNode* parent_ptr  = nullptr;
	GTNode* child_ptr   = nullptr;
	GTNode* sibling_ptr = nullptr;

	GTNode() 
	{
		for (int i = 0; i < m_size; ++i)
			visited[i] = -1;
	}

	~GTNode()
	{
		// make up
	}
};


void calcLB(GTNode* n)
{
	
	auto last_visited_node = std::max_element(std::begin(n->visited), std::end(n->visited),
		[](const pair<int, int> &pair_1, const pair<int, int> &pair_2)
		{
			return pair_1.second < pair_2.second;
		});

	auto penultimate_visted_node = std::find_if(std::begin(n->visited), std::end(n->visited),
		[&](const pair<int, int> &pair)
		{
			return pair.second == (last_visited_node->second - 1);
		});

	n->actual_cost = matrix[penultimate_visted_node->first][last_visited_node->first];
	n->actual_cost += n->parent_ptr->actual_cost;
	n->lb = n->actual_cost;

	int counter = 0;
	std::for_each(std::begin(n->visited), std::end(n->visited),
		[&, counter](const pair<int, int> &pair) mutable
	{
		if (pair.second == last_visited_node->second || pair.second == -1 )
			n->lb += memo[counter];

		++counter;
	});
}


void setPath(GTNode* current_node, map<int, int> &m)
{
	cout << "New Path" << endl;
	//current_node->visited = current_node->parent_ptr->visited;

	auto first_unvisited_node = std::find_if(std::begin(m), std::end(m),
		[](const pair<int, int> &pair)
		{
			// you should consider only last result of this cout 
			cout << "ancillary_map: " <<pair.first << " " << pair.second << endl;
			return pair.second == -1;
		});
	
	// add new city to path
	first_unvisited_node->second = true;
	current_node->visited[first_unvisited_node->first] = current_node->level; // pos
	for (auto it = current_node->visited.begin(); it != current_node->visited.end(); ++it)
		cout << "current_node first: " << it->first << "  second: " << it->second << endl;
}


GTNode* findTheLowestLB(GTNode* node, int min_lb)
{
	// sprawdzic pisowanie adjustment
	GTNode* best_adjustment = new GTNode();

	if (node->lb < min_lb)
	{
		best_adjustment = node;
	}

	return best_adjustment;
}

GTNode* best_adjustment = new GTNode();

void preOrderHelp(GTNode* current_node)
{
	if (current_node != nullptr)
	{
		// visit this node here
		cout << current_node->lb << endl;
		preOrderHelp(current_node->child_ptr);
		preOrderHelp(current_node->sibling_ptr);
		cout << "new level" << endl;
	}
}
	

void preOrderTraversal(GTNode* current_node)
{
	int min_lb = INT_MAX;

	if (current_node != nullptr)
	{
		// visit this node here
		cout << current_node->lb << endl;

		if (current_node->lb < min_lb)
		{
			best_adjustment = current_node;
		}

		preOrderTraversal(current_node->child_ptr);
		preOrderTraversal(current_node->sibling_ptr);
	}

	min_lb = INT_MAX;
}


void createSiblings(GTNode* child, GTNode* parent, map<int, int> &m, int num)
{
	if (num - 1 != 0)
	{
		child->sibling_ptr = new GTNode();
		child->sibling_ptr->parent_ptr = parent;
		child->sibling_ptr->visited = parent->visited;
		child->sibling_ptr->level = parent->level + 1;

		setPath(child->sibling_ptr, m);
		calcLB(child->sibling_ptr);
		--num;

		createSiblings(child->sibling_ptr, parent, m, num);
	}
}


void createChildren(GTNode* parent, int num)
{
	GTNode* child = new GTNode();
	parent->child_ptr = child;
	child->parent_ptr = parent;
	child->visited = child->parent_ptr->visited;
	child->level = parent->level + 1;
	auto ancillary_map = parent->visited;

	setPath(child, ancillary_map);
	calcLB(child);
	createSiblings(child, parent, ancillary_map, num);
}


inline void initFindPath(GTNode* root)
{
	createChildren(root, (m_size - 1 - root->level));
}


void findPath()
{
	GTNode* root = new GTNode();

	root->visited[0] = 0;

	initFindPath(root);
	preOrderHelp(root);

	GTNode* temp = new GTNode();
	preOrderTraversal(root->child_ptr);
	int numOfChil = m_size - 1 - best_adjustment->level;
	createChildren(best_adjustment, numOfChil);
	preOrderHelp(root);
}


int main()
{
	for (int i = 0; i < m_size; ++i)
		unvisited.push_back(true);

	//printMatrix(matrix);

//////////////////////////////////////////////////////////////////
	//calc LB
	int LB = 0;
	int temp_LB = INT_MAX;

	//mby unordered_map

	for (int i = 0; i < m_size; i++)
		for (int j = 0; j < m_size; j++)
		{
			if (temp_LB > matrix[i][j] && matrix[i][j] != -1)
				temp_LB = matrix[i][j];

			if (j % (m_size - 1) == 0 && j != 0)
			{
				LB += temp_LB;
				//set the lowest value for each row
				memo.push_back(temp_LB);
				temp_LB = INT_MAX;
				//cout << "Memo map[" << i << "] = " << memo_map[i] << endl;
			}
		}

	cout << "LB: " << LB << endl;
//////////////////////////////////////////////////////////////////

	findPath();
	system("pause");
}