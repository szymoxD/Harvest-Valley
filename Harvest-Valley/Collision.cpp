#include "Collision.h"
bool Collision::collide(std::array<float, 12> square, float angle) {
	float temp;
	if (angle > 90 && angle < 270) {
		int i = 0, j = 9;
		for (int k = 0; k < 3; k++) {
			temp = square[i + k];
			square[i + k] = square[j + k];
			square[j + k] = temp;
		}
		i = 3;
		j = 6;
		for (int k = 0; k < 3; k++) {
			temp = square[i + k];
			square[i + k] = square[j + k];
			square[j + k] = temp;
		}
	}
	bool left = false, right = false;
	for (int i = 0; i < 12; i += 3) {	//x<0, x>=0
		if (square[i] < 0) {
			left = true;
		}
		if (square[i] >= 0) {
			right = true;
		}
	}
	if (left) {

		for (int j = 0; j < 12; j += 3) {
			for (int i = 0; i < squaresleft.size(); i++) {
				if (square[j] > squaresleft[i][0] && square[j] < squaresleft[i][9] && square[j + 2]>squaresleft[i][2] && square[j + 2] < squaresleft[i][11])
					return true;
			}
		}
		for (int i = 0; i < squaresleft.size(); i++) {
			for (int k = 0; k < 12; k += 3) {
				if (squaresleft[i][k] > square[0] && squaresleft[i][k] < square[9] && squaresleft[i][k + 2] > square[2] && squaresleft[i][k + 2] < square[11])
					return true;
			}
		}

		for (int j = 0; j < 12; j += 3) {
			for (int i = 0; i < circlesleft.size(); i++) {
				if (sqrt(pow(square[j] - circlesleft[i][0], 2) + pow(square[j + 2] - circlesleft[i][2], 2)) <= circlesleft[i][3])
					return true;
			}
		}
	}
	if (right) {

		for (int j = 0; j < 12; j += 3) {
			for (int i = 0; i < squaresright.size(); i++) {
				if (square[j] > squaresright[i][0] && square[j] < squaresright[i][9] && square[j + 2]>squaresright[i][2] && square[j + 2] < squaresright[i][11])
					return true;
			}
		}
		for (int i = 0; i < squaresright.size(); i++) {
			for (int k = 0; k < 12; k += 3) {
				if (squaresright[i][k] > square[0] && squaresright[i][k] < square[9] && squaresright[i][k + 2] > square[2] && squaresright[i][k + 2] < square[11])
					return true;
			}
		}

		for (int j = 0; j < 12; j += 3) {
			for (int i = 0; i < circlesright.size(); i++) {
				if (sqrt(pow(square[j] - circlesright[i][0], 2) + pow(square[j + 2] - circlesright[i][2], 2)) <= circlesright[i][3])
					return true;
			}
		}
	}
	return false;
}
void Collision::addsquare(std::array<float, 12> square) {
	float temp;
	int min = 0;
	for (int i = 0; i < 12; i += 3) {
		min = i;
		for (int j = i + 3; j < 12; j += 3) {
			if (square[min] > square[j] || (square[min] == square[j] && square[min + 2] > square[j + 2])) {
				min = j;
			}
		}
		if (min != i) {
			for (int k = 0; k < 3; k++) {
				temp = square[i + k];
				square[i + k] = square[min + k];
				square[min + k] = temp;
			}
		}
	}
	bool left = false, right = false;
	for (int i = 0; i < 12; i += 3) {	//x<0, x>=0
		if (square[i] < 0) {
			left = true;
		}
		if (square[i] >= 0) {
			right = true;
		}
	}
	if (left)
		squaresleft.push_back(square);
	if (right)
		squaresright.push_back(square);
}
void Collision::addcircle(std::array<float, 4> circle) {
	bool left = false, right = false;
	if (circle[0] - circle[3] < 0) {
		left = true;
	}
	if (circle[0] + circle[3] >= 0) {
		right = true;
	}
	if (left)
		circlesleft.push_back(circle);
	if (right)
		circlesright.push_back(circle);
}
void Collision::popcircle(std::array<float, 4> circle) {
	bool left = false, right = false;
	if (circle[0] - circle[3] < 0) {
		left = true;
	}
	if (circle[0] + circle[3] >= 0) {
		right = true;
	}
	if (left) {
		int find = -1;
		for (int i=0; i < circlesleft.size(); i++) {
			if (circlesleft[i][0] == circle[0] && circlesleft[i][2] == circle[2] && circlesleft[i][3] == circle[3]) {
				find = i;
				break;
			}
		}
		if (find >= 0)
			circlesleft.erase(circlesleft.begin() + find);
	}
	if (right) {
		int find = -1;
		for (int i=0; i < circlesright.size(); i++) {
			if (circlesright[i][0] == circle[0] && circlesright[i][2] == circle[2] && circlesright[i][3] == circle[3]) {
				find = i;
				break;
			}
		}
		if (find >= 0)
			circlesright.erase(circlesright.begin() + find);
	}
}