#include <iostream>
#include <vector>
#include <climits>
#include <map>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

int end_counter = 0;

const int m_size{ 5 };
vector<bool> unvisited; // sprawdzic czy da sie zainicjowac
vector<int> memo;
map<int, bool> visited_map;
int UB = INT_MAX;
int min_lb = INT_MAX;


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

	int lb = -1;
	int actual_cost = 0;
	int level = 0;
	bool was_visitied = false;
	bool leaf = false;
	// first int - which city, second int - on which position
	map<int, int> visited; // sprawdzi� czy da sie zainicjowac
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

	auto penultimate_visited_node = std::find_if(std::begin(n->visited), std::end(n->visited),
		[&](const pair<int, int> &pair)
		{
			return pair.second == (last_visited_node->second - 1);
		});

	n->actual_cost = matrix[penultimate_visited_node->first][last_visited_node->first];
	n->actual_cost += n->parent_ptr->actual_cost;
	n->lb = n->actual_cost;

	int counter = 0;

	if (m_size - n->level != 2)
	{
		std::for_each(std::begin(n->visited), std::end(n->visited),
			[&, counter](const pair<int, int> &pair) mutable
		{
			if (pair.second == last_visited_node->second || pair.second == -1)
				n->lb += memo[counter];

			++counter;
		});
	}
	

	else if (m_size - n->level == 2)
	{
		auto last_visited_node_1 = std::find_if(std::begin(n->visited), std::end(n->visited),
			[](const std::pair<int, int> &pair)
		{
			return pair.second == m_size - 3;
		});

		n->lb += matrix[last_visited_node_1->first][penultimate_visited_node->first];
		n->lb += matrix[last_visited_node->first][0];

		n->leaf = true;

		if (n->lb < UB)
			UB = n->lb;
	}
}


void setPath(GTNode* current_node, map<int, int> &m)
{
	//current_node->visited = current_node->parent_ptr->visited;

	auto first_unvisited_node = std::find_if(std::begin(m), std::end(m),
		[&](const pair<int, int> &pair)
		{
			// you should consider only last result of this cout 
			if (end_counter == 1)
				return pair.second == -3;
			else
				return pair.second == -1;
		});
	
	// add new city to path
	if (m_size - current_node->level == 2)
		first_unvisited_node->second = -2;
	else
		first_unvisited_node->second = true;


	current_node->visited[first_unvisited_node->first] = current_node->level; // pos

	if (m_size - current_node->level == 2)
	{
		auto last_unvisited_node = std::find_if(std::begin(m), std::end(m),
			[&](const pair<int, int> &pair)
			{
				// you should consider only last result of this cout 
				if (end_counter != 0) {
					//end_counter = 0;
					return pair.second == -2;
				}
				else {
					return pair.second == -1;
				}
			});

		last_unvisited_node->second = -3;
		current_node->visited[last_unvisited_node->first] = current_node->level + 1;
		end_counter++;
	}
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
		cout << current_node->lb << " visited: " << current_node->was_visitied << endl;
		preOrderHelp(current_node->child_ptr);
		preOrderHelp(current_node->sibling_ptr);
	}
}

bool checkSiblings(GTNode* node)
{
	if (node->child_ptr == nullptr && !node->child_ptr->leaf)
	{
		return false;
	}

	return true;
}
	
bool wasVisited(GTNode* node)
{
	GTNode* temp = new GTNode();
	if (node->child_ptr == nullptr)
		return false;
	else
		temp = node->child_ptr;

	if (temp->leaf == true)
	{
		node->was_visitied = true;
		return true;
	}

	while (temp->sibling_ptr != nullptr)
	{
		temp = temp->sibling_ptr;

		if (temp->child_ptr == nullptr)
			return false;
	}
	
	node->was_visitied = true;
	return true;
}

void preOrderTraversal(GTNode* current_node)
{
	cout << "min_lb: " << min_lb << endl;
	if (current_node != nullptr)
	{
		// visit this node here

		cout << current_node->lb << " visited: " << current_node->was_visitied << endl;

		if (current_node->lb <= min_lb && current_node->lb != -1 && wasVisited(current_node) == false)
		{
			best_adjustment = current_node;
			min_lb = current_node->lb;
		}

		preOrderTraversal(current_node->child_ptr);
		preOrderTraversal(current_node->sibling_ptr);
	}

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

	if (child->parent_ptr->parent_ptr != nullptr)
	{
		child->parent_ptr->parent_ptr->was_visitied = false;
	}

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

	preOrderTraversal(root);
	min_lb = INT_MAX;
	int numOfChil = m_size - 1 - best_adjustment->level;
	createChildren(best_adjustment, numOfChil);

	preOrderHelp(root);

	preOrderTraversal(root);
	min_lb = INT_MAX;
	numOfChil = m_size - 1 - best_adjustment->level;
	createChildren(best_adjustment, numOfChil);
	end_counter = 0;

	preOrderHelp(root);

	preOrderTraversal(root);
	min_lb = INT_MAX;
	numOfChil = m_size - 1 - best_adjustment->level;
	createChildren(best_adjustment, numOfChil);
	end_counter = 0;

	preOrderHelp(root);
	cout << "UB: " << UB << endl;
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