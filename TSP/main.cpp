#include <iostream>
#include <climits>
#include <map>
#include <vector>
#include <algorithm>
#include "Reader.h"

using namespace std;

int end_counter = 0;
int UB = INT_MAX;
int min_lb = INT_MAX;
const int m_size{ 5 };
bool ub_update = false;
vector<bool> unvisited;
vector<int> memo;
map<int, bool> visited_map;
map<int, int> path;

/*int matrix[][m_size] =
{ {-1, 2451, 713, 1018, 1631, 1374, 2408, 213, 2571, 875, 1420, 2145, 1972}, // New York
{2451, -1, 1745, 1524, 831, 1240, 959, 2596, 403, 1589, 1374, 357, 579}, // Los Angeles
{713, 1745, -1, 355, 920, 803, 1737, 851, 1858, 262, 940, 1453, 1260}, // Chicago
{1018, 1524, 355, -1, 700, 862, 1395, 1123, 1584, 466, 1056, 1280, 987}, // Minneapolis
{1631, 831, 920, 700, -1, 663, 1021, 1769, 949, 796, 879, 586, 371}, // Denver
{1374, 1240, 803, 862, 663, -1, 1681, 1551, 1765, 547, 225, 887, 999}, // Dallas
{2408, 959, 1737, 1395, 1021, 1681, -1, 2493, 678, 1724, 1891, 1114, 701}, // Seattle
{213, 2596, 851, 1123, 1769, 1551, 2493, -1, 2699, 1038, 1605, 2300, 2099}, // Boston
{2571, 403, 1858, 1584, 949, 1765, 678, 2699, -1, 1744, 1645, 653, 600}, // San Francisco
{875, 1589, 262, 466, 796, 547, 1724, 1038, 1744, -1, 679, 1272, 1162}, // St.Louis
{1420, 1374, 940, 1056, 879, 225, 1891, 1605, 1645, 679, -1, 1017, 1200}, // Houston
{2145, 357, 1453, 1280, 586, 887, 1114, 2300, 653, 1272, 1017, -1, 504}, // Phoenix
{1972, 579, 1260, 987, 371, 999, 701, 2099, 600, 1162, 1200, 504, -1 } }; // Salt Lake City*/

int matrix[][5] = {
	{  -1,10,8,9,7  },
	{  10,-1,10,5,6 },
	{  8,10,-1,8,9  },
	{  9,5,8,-1,6   },
	{  7,6,9,6,-1   }
};


/*int matrix[][m_size] = {
	{-1,34,36,37,31,33,35},
	{34,-1,29,23,22,25,24},
	{36,29,-1,17,12,18,17},
	{37,23,17,-1,32,30,29},
	{31,22,12,32,-1,26,24},
	{33,25,18,30,26,-1,19},
	{35,24,17,29,24,19,-1}
};*/

/*int matrix[][m_size] = {
   { -1,3,93,13,33,9 },
   { 4,-1,77,42,21,16 },
   { 45,17,-1,36,16,28 },
   { 39,90,80,-1,56,7 },
   { 28,46,88,33,-1, 25 },
   { 3,88,18,46,92,-1} };*/



void printMatrix(int m[][5]) {
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j) {
			cout << m[i][j];

			if (j == 4)
				cout << endl;
		}
}


class GTNode {
public:
	int lb = -1;
	int actual_cost = 0;
	int level = 0;
	bool was_visited = false;
	bool leaf = false;
	bool was_selected = false;
	// first int - which city, second int - on which position
	map<int, int> visited;
	GTNode* parent_ptr  = nullptr;
	GTNode* child_ptr   = nullptr;
	GTNode* sibling_ptr = nullptr;
	GTNode* prev_node = nullptr;

	GTNode()  {
		for (int i = 0; i < m_size; ++i)
			visited[i] = -1;
	}

	~GTNode() {
		// make up
	}
}; GTNode* root = new GTNode(); GTNode* best_adjustment = new GTNode();

void deleteChildrens(GTNode* current_node, GTNode* start_node)
{
	if (current_node != nullptr) {
		deleteChildrens(current_node->child_ptr, start_node);
		if (current_node != start_node)
			deleteChildrens(current_node->sibling_ptr, start_node);

		delete current_node;
	}
}
void cutOff(GTNode* current_node) {
	if (current_node != nullptr) {
		if (current_node->lb >= UB)
		{
			// usuwanie wêz³a z œrodka
			if (current_node->sibling_ptr != nullptr && current_node->prev_node != nullptr) {
				GTNode* help = current_node;
				current_node->prev_node->sibling_ptr = current_node->sibling_ptr;
				current_node->sibling_ptr->prev_node = current_node->prev_node;
				current_node = current_node->prev_node;

				deleteChildrens(help, help);

			}
			// usuwanie wêz³a z pocz¹tku (gdy jest dzieckiem)
			else if (current_node->parent_ptr->child_ptr == current_node && current_node->sibling_ptr != nullptr) {
				GTNode* help = current_node;
				current_node->parent_ptr->child_ptr = current_node->sibling_ptr;
				current_node->parent_ptr->child_ptr->prev_node = nullptr;
				current_node = current_node->parent_ptr->child_ptr;

				deleteChildrens(help, help);
			}
			// usuwanie wêz³a z koñca
			else if (current_node->sibling_ptr == nullptr && current_node->prev_node != nullptr)
			{
				GTNode* help = current_node;
				current_node = current_node->prev_node;
				current_node->sibling_ptr = nullptr;

				deleteChildrens(help, help);
			}
			// usuwanie gdy to jedyny wêze³ w rzêdzie
			else {

				GTNode* help = current_node;
				current_node = current_node->parent_ptr;
				current_node->child_ptr = nullptr;

				deleteChildrens(help, help);
			}
		}
		
		cutOff(current_node->child_ptr);
		cutOff(current_node->sibling_ptr);
	}
}


void preOrderHelp(GTNode* current_node)
{
	if (current_node != nullptr) {
		cout << current_node->lb << "   " << current_node->level << endl;
		preOrderHelp(current_node->child_ptr);
		preOrderHelp(current_node->sibling_ptr);
	}
}


void calcLB(GTNode* n) {
	auto last_visited_node = std::max_element(std::begin(n->visited), std::end(n->visited),
		[](const pair<int, int> &pair_1, const pair<int, int> &pair_2) {
			return pair_1.second < pair_2.second;
		});

	auto penultimate_visited_node = std::find_if(std::begin(n->visited), std::end(n->visited),
		[&](const pair<int, int> &pair) {
			return pair.second == (last_visited_node->second - 1);
		});

	n->actual_cost = matrix[penultimate_visited_node->first][last_visited_node->first];
	n->actual_cost += n->parent_ptr->actual_cost;
	n->lb = n->actual_cost;

	int counter = 0;

	if (m_size - n->level != 2) {
		std::for_each(std::begin(n->visited), std::end(n->visited),
			[&, counter](const pair<int, int> &pair) mutable {
			if (pair.second == last_visited_node->second || pair.second == -1)
				n->lb += memo[counter];

			++counter;
		});
	}
	
	else if (m_size - n->level == 2) {
		auto last_visited_node_1 = std::find_if(std::begin(n->visited), std::end(n->visited),
			[](const std::pair<int, int> &pair) {
			return pair.second == m_size - 3;
		});

		n->lb += matrix[last_visited_node_1->first][penultimate_visited_node->first];
		n->lb += matrix[last_visited_node->first][0];
		n->leaf = true;

		if (n->lb < UB) {
			UB = n->lb;
			cout << "LEAF UB: " << UB << endl;
			path = n->visited;
			ub_update = true;
		}	
	}
} 


void setPath(GTNode* current_node, map<int, int> &m) {
	auto first_unvisited_node = std::find_if(std::begin(m), std::end(m),
		[&](const pair<int, int> &pair) {
			if (end_counter == 1)
				return pair.second == -3;
			else
				return pair.second == -1;
		});
	
	// add new city to path

	current_node->visited[first_unvisited_node->first] = current_node->level;
	if (m_size - current_node->level == 2)
		first_unvisited_node->second = -2;
	else
		first_unvisited_node->second = true;

	if (m_size - current_node->level == 2) {
		auto last_unvisited_node = std::find_if(std::begin(m), std::end(m),
			[&](const pair<int, int> &pair) {
				if (end_counter != 0) 
					return pair.second == -2;
				else 
					return pair.second == -1;
			});

		last_unvisited_node->second = -3;
		current_node->visited[last_unvisited_node->first] = current_node->level + 1;
		end_counter++;
	}
}


bool wasVisited(GTNode* node) {
	GTNode* temp = new GTNode();

	if (node->child_ptr == nullptr)
		return false;
	else
		temp = node->child_ptr;

	if (temp->leaf == true) {
		node->was_visited = true;

		return true;
	}

	while (temp->sibling_ptr != nullptr) {
		temp = temp->sibling_ptr;

		if (temp->child_ptr == nullptr)
			return false;
	}
	
	node->was_visited = true;
	return true;
}


void preOrderTraversal(GTNode* current_node) {
	if (current_node != nullptr) {
		if (current_node->was_selected == false &&
			current_node->lb <= min_lb &&
			current_node->lb != -1 &&
			current_node->was_visited == false &&
			wasVisited(current_node) == false)
		{
			best_adjustment = current_node;
			min_lb = current_node->lb;
		}

		else if (current_node->lb >= UB)
		{
			// usuwanie wêz³a z œrodka
			if (current_node->sibling_ptr != nullptr && current_node->prev_node != nullptr) {

				GTNode* help = current_node;
				current_node->prev_node->sibling_ptr = current_node->sibling_ptr;
				current_node->sibling_ptr->prev_node = current_node->prev_node;
				current_node = current_node->prev_node;


				deleteChildrens(help, help);

			}
			// usuwanie wêz³a z pocz¹tku (gdy jest dzieckiem)
			else if (current_node->parent_ptr->child_ptr == current_node && current_node->sibling_ptr != nullptr) {
				GTNode* help = current_node;
				current_node->parent_ptr->child_ptr = current_node->sibling_ptr;
				current_node->parent_ptr->child_ptr->prev_node = nullptr;
				current_node = current_node->parent_ptr->child_ptr;

				deleteChildrens(help, help);
			}
			// usuwanie wêz³a z koñca
			else if (current_node->sibling_ptr == nullptr && current_node->prev_node != nullptr)
			{
				GTNode* help = current_node;
				current_node = current_node->prev_node;
				current_node->sibling_ptr = nullptr;
				//cout << "usuwam z konca " << help->lb << " level " << " : " << current_node->prev_node << " : " << current_node->sibling_ptr << endl;

				deleteChildrens(help, help);

			}
			// usuwanie gdy to jedyny wêze³ w rzêdzie
			else {
				

				GTNode* help = current_node;
				current_node = current_node->parent_ptr;
				current_node->child_ptr = nullptr;

				deleteChildrens(help, help);
			}
		}

		preOrderTraversal(current_node->child_ptr);
		preOrderTraversal(current_node->sibling_ptr);
	}
}


void createSiblings(GTNode* child, GTNode* parent, map<int, int> &m, int num) {
	if (num - 1 != 0) {
		child->sibling_ptr = new GTNode();
		child->sibling_ptr->prev_node = child;
		child->sibling_ptr->parent_ptr = parent;
		child->sibling_ptr->visited = parent->visited;
		child->sibling_ptr->level = parent->level + 1;

		setPath(child->sibling_ptr, m);
		calcLB(child->sibling_ptr);
		--num;

		createSiblings(child->sibling_ptr, parent, m, num);
	}
	if (ub_update == true) {
		//cout << "before" << endl;
		preOrderHelp(root);
		// najlepiej wykonaæ jak bêdzie zrobiony ca³y wêze³
		cutOff(root);
		// << "after" << endl;
		//preOrderHelp(root);
		ub_update = false;
	}
}


void createChildren(GTNode* parent, int num) {
	GTNode* child = new GTNode();
	parent->child_ptr = child;
	child->parent_ptr = parent;
	child->visited = child->parent_ptr->visited;
	child->level = parent->level + 1;
	auto ancillary_map = parent->visited;

	if (child->parent_ptr->parent_ptr != nullptr)
		child->parent_ptr->parent_ptr->was_visited = false;

	setPath(child, ancillary_map);
	calcLB(child);
	createSiblings(child, parent, ancillary_map, num);
}


inline void initFindPath(GTNode* root) {
	createChildren(root, (m_size - 1 - root->level));
}


void mainLoop(GTNode* root)
{
	int numOfChil;
	int counter = 0;

	while (counter != 100000) {
		//cout << endl;
		preOrderTraversal(root);
		//min_lb = INT_MAX;
		numOfChil = m_size - 1 - best_adjustment->level;
		best_adjustment->was_selected = true;
		createChildren(best_adjustment, numOfChil);
		end_counter = 0;
		//preOrderHelp(root);
		++counter;
	}
}


void printPath() {
	cout << "Path: ";

	map<int, int> temp;
	auto it_temp = temp.begin();
	int i = 0;

	for (auto it = path.begin(); it != path.end(); ++it)
		temp.insert(it_temp, pair<int, int>(it->second, it->first));

	for (auto it = temp.begin(); it != temp.end(); ++it, ++i)
		cout << temp[i] + 1 << "->";

	cout << "1" << endl;
	cout << "Cost: " << UB << endl;
}


void findPath() {
	root->visited[0] = 0;

	initFindPath(root);
	preOrderHelp(root);

	mainLoop(root);
	printPath();

	delete root;
}

void preOrderDelete(GTNode* current_node)
{
	if (current_node != nullptr) {
		preOrderHelp(current_node->child_ptr);
		preOrderHelp(current_node->sibling_ptr);
		delete current_node;
	}
}


void calcInitLB() {
	for (int i = 0; i < m_size; ++i)
		unvisited.push_back(true);

	int LB = 0;
	int temp_LB = INT_MAX;

	for (int i = 0; i < m_size; i++)
		for (int j = 0; j < m_size; j++) {
			if (temp_LB > matrix[i][j] && matrix[i][j] != -1)
				temp_LB = matrix[i][j];

			if (j % (m_size - 1) == 0 && j != 0) {
				LB += temp_LB;
				memo.push_back(temp_LB);
				temp_LB = INT_MAX;
				//cout << "Memo map[" << i << "] = " << memo[i] << endl;
			}
		}
}


int main() {
	/*Reader reader;
	reader.WczytajMiasta("brazil58.tsp");*/

	calcInitLB();
	findPath();
	system("pause");
	return 0;
}