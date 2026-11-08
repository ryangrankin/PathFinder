
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <queue>

//bfs
std::vector<std::pair<int, int>> bfs(
	const std::vector<std::vector<int>>& grid,
	int startRow, int startCol,
	int endRow, int endCol,
	std::vector<std::pair<int, int>>& visitedOrder
) {
	int rows = grid.size();
	int cols = grid[0].size();

	std::queue<std::pair<int, int>> q;

	std::vector<std::vector<bool>> visited(
		rows, std::vector<bool>(cols, false)
	);

	std::vector<std::vector<std::pair<int, int>>> parent(
		rows, 
		std::vector<std::pair<int, int>>(cols, {-1, -1})
	);

	q.push({startRow, startCol});
	visited[startRow][startCol] = true;

	int dr[] = {-1, 1, 0, 0};
	int dc[] = {0, 0, -1, 1};

	while (!q.empty()) {
		auto [row, col] = q.front();
		q.pop();
		visitedOrder.push_back({row, col});

		if (row == endRow && col == endCol) {
			break;
		}

		for (int i = 0; i < 4; i++) {
			int nr = row + dr[i];
			int nc = col + dc[i];

			if (nr >= 0 && nr < rows &&
				nc >= 0 && nc < cols &&
				grid[nr][nc] == 0 &&
				!visited[nr][nc]){
				
				visited[nr][nc] = true;
				parent[nr][nc] = {row, col};
				q.push({nr, nc});
			}
		}
	}

	std::vector<std::pair<int, int>> path;

	if (!visited[endRow][endCol]) {
		return path;
	}


	int row = endRow;
	int col = endCol;

	while (row != startRow || col != startCol) {
		path.push_back({row, col});
		auto [pr, pc] = parent[row][col];
		row = pr;
		col = pc;
	} 

	std::reverse(path.begin(), path.end());

	return path;
}

//dijisktras alg 
std::vector<std::pair<int, int>> dijkstra(
	const std::vector<std::vector<int>>&grid,
	int startRow, int startCol, 
	int endRow, int endCol,
	std::vector<std::pair<int, int>>& visitedOrder
) {
	int rows = grid.size();
	int cols = grid[0].size();
	
	const int INF = std::numeric_limits<int>::max();

	std::vector<std::vector<int>> dist(
		rows, std::vector<int>(cols, INF)
	);

	std::vector<std::vector<std::pair<int, int>>> parent(
		rows,
		std::vector<std::pair<int, int>>(cols, {-1, -1})
	);

	//distance; row, col
	using Node = std::pair<int, std::pair<int, int>>;

	std::priority_queue<
		Node,
		std::vector<Node>,
		std::greater<Node>
	> pq;

	dist[startRow][startCol] = 0;
	pq.push({0, {startRow, startCol}});

	int dr[] = {-1, 1, 0, 0};
	int dc[] = {0, 0, -1, 1};

	while (!pq.empty()) {
		auto [distance, position] = pq.top();
		pq.pop();

		auto [row, col] = position;

		//ignore outaded priorty queue entries 
		if (distance != dist[row][col]) {
			continue;
		}

		visitedOrder.push_back({row, col});

		if (row == endRow && col == endCol) {
			break;
		}

		for (int i=0; i<4; i++) {
			int nr = row + dr[i];
			int nc = col + dc[i];

			if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && grid[nr][nc] == 0) {
				int newDist = distance + 1;

				if (newDist < dist[nr][nc]) {
					dist [nr][nc] = newDist;
					parent[nr][nc] = {row, col};

					pq.push({newDist, {nr, nc}});
				}
			}
		}
	}

	std::vector<std::pair<int, int>> path;
	
	if (dist[endRow][endCol] == INF) {
		return path;
	}

	int row = endRow;
	int col = endCol;

	while (row != startRow || col != startCol) {
		path.push_back({row, col});

		auto [pr, pc] = parent[row][col];
		row = pr;
		col = pc;
	}

	std::reverse(path.begin(), path.end());

	return path;
}

int main() {
	const int CELL_SIZE = 40;
	const int ROWS = 15;
	const int COLS = 20;
	const int OFFSET = 50;

	//0= empty cell, 1=wall
	std::vector<std::vector<int>> grid(
		ROWS, std::vector<int>(COLS, 0)
	);	

	int startRow = 2;
	int startCol = 2;

	int endRow =12;
	int endCol = 17;

	//0= wall mode, 1 = start mode, 2 = end mode
	int mode = 0;
	//0: BFS, 1=Dijkstra
	int algorithm = 0;
	std::vector<std::pair<int, int>> path;	
	std::vector<std::pair<int, int>> visitedOrder;

	std::size_t animationIndex = 0;
	bool isAnimating = false;
	sf::Clock animationClock;

	sf::RenderWindow window(
		sf::VideoMode({900, 700}),
		"PathFinder"
	);

	window.setFramerateLimit(60);

	while (window.isOpen()) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {	
				
				window.close();
			}

			if (const auto* key =
				event->getIf<sf::Event::KeyPressed>()) {
			if (key->code == sf::Keyboard::Key::B) {
				algorithm = 0; // select BFS
			} else if (key->code == sf::Keyboard::Key::D) {
				algorithm = 1; //select Dijkstra
			} else if (key->code == sf::Keyboard::Key::W) {
				mode = 0;
			} else if (key->code == sf::Keyboard::Key::S) {
				mode = 1;
			} else if (key->code == sf::Keyboard::Key::E) {
				mode = 2;
			} else if (key->code == sf::Keyboard::Key::Space) {
				visitedOrder.clear();
				
				if (algorithm == 0) {
					path = bfs(grid, startRow, startCol, endRow, endCol, visitedOrder);
				} else if (algorithm == 1 ){
					path = dijkstra(grid, startRow, startCol, endRow, endCol, visitedOrder);
				}				
	
				animationIndex = 0;
				isAnimating = true;
				animationClock.restart();
			} else if ( key->code == sf::Keyboard::Key::R) {
				path.clear();
				visitedOrder.clear();
				animationIndex = 0;
				isAnimating = false;
			}
		}


		//handle mouse clicks
		if (const auto* mouse =
			event->getIf<sf::Event::MouseButtonPressed>()) {
	
			if (mouse->button == sf::Mouse::Button::Left) {
				int x = mouse->position.x - OFFSET;
				int y = mouse->position.y - OFFSET;

				if (x >= 0 && y >= 0) {
					int col = x / CELL_SIZE;
					int row = y / CELL_SIZE;

					if (row < ROWS && col < COLS) {
						//clear existing search when grid changes
						path.clear();
						visitedOrder.clear();
						animationIndex = 0;
						isAnimating = false;

						if (mode == 1) {
							if (row != endRow || col != endCol) {
								startRow = row;
								startCol = col;
								grid[row][col] = 0;
							}
						} else if (mode == 2) {
							if (row != startRow || col != startCol) {
								endRow = row;
								endCol = col;
								grid[row][col]=0;
							}
						} else {
							if (!((row == startRow && col == startCol) ||
								(row == endRow && col == endCol))) {
								grid[row][col] = 1 - grid[row][col];
							}
						}
					}
				}
			}
		}
	}

		//advance bfs animation every 3 ms 
		// Advance BFS animation every 30 milliseconds
		if (isAnimating &&
    		animationClock.getElapsedTime().asMilliseconds() >= 30) {

    			if (animationIndex < visitedOrder.size()) {
        			animationIndex++;
        			animationClock.restart();
    			} else {
        			isAnimating = false;
    			}
		}

		window.clear(sf::Color(25, 30, 45));

		//draw grid
		for (int row=0; row < ROWS; row++) {
			for (int col=0; col < COLS; col++) {
				sf::RectangleShape cell(
					sf::Vector2f(CELL_SIZE -1, CELL_SIZE -1)
				);

				cell.setPosition({
					static_cast<float>(col * CELL_SIZE + 50),
					static_cast<float>(row * CELL_SIZE + 50)
				});

				if (row == startRow && col == startCol) {
					cell.setFillColor(sf::Color(40, 200, 100));
				} else if (row == endRow && col == endCol) {
					cell.setFillColor(sf::Color(220, 70, 70));
				} else if ( grid[row][col] == 1) {
					cell.setFillColor(sf::Color(150, 160, 175));
				
				} else if (!isAnimating &&
					std::find(path.begin(), path.end(),
						std::make_pair(row, col)) != path.end()){
					cell.setFillColor(sf::Color(255, 210, 70));
				} else if (std::find(
					visitedOrder.begin(),
					visitedOrder.begin() +  animationIndex,
					std::make_pair(row, col)
					) != visitedOrder.begin() + animationIndex) {
					cell.setFillColor(sf::Color(65, 145, 220));

				} else {
					cell.setFillColor(sf::Color(45, 55, 75));
				}


				window.draw(cell);
			}
		}

		window.display();
	}

	return 0;
}
	
