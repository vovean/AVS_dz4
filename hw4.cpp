#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;

const double EPS = 1E-9;
int N, threadsCount;
vector<bool> usedLines;
int _rank;
vector<vector<int>> matrix;

int readIntegerFromUser(string prompt)
{
	int num;
	cout << prompt;
	cin >> num;
	if (cin.good())
	{
		return num;
	}
	cerr << "Invalid input\n";
	exit(1);
}

void generateRandomMatrix() {
	matrix = vector<vector<int>>(N, vector<int>(N));
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix.size(); j++)
		{
			matrix[i][j] = rand() % 200 - 100;
		}
	}
}

void getMatrixFromFile(string path)
{
	ifstream inFile(path);
	inFile >> N;
	matrix = vector<vector<int>>(N, vector<int>(N));
	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix.size(); j++)
		{
			inFile >> matrix[i][j];
		}
	}
}

void calculateMatrixRank() {
	usedLines = vector<bool>(matrix.size());
#pragma omp parallel for num_threads(threadsCount)
	for (int i = 0; i < matrix.size(); i++) {
		int j = 0;
		for (; j < matrix.size(); j++)
		{
			if (!usedLines[j] && abs(matrix[j][i]) > EPS)
			{
				break;
			}
		}
		if (j == matrix.size())
		{
			--_rank;
			continue;
		}
		usedLines[j] = true;
		for (int col = i + 1; col < matrix.size(); col++)
		{
			matrix[j][col] /= matrix[j][i];
		}
		for (int row = 0; row < matrix.size(); row++)
		{
			if (row != j && abs(matrix[row][i]) > EPS)
			{
				for (int col = i + 1; col < matrix.size(); col++)
				{
					matrix[row][col] -= matrix[j][col] * matrix[row][i];
				}
			}
		}
	}
}



int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		N = readIntegerFromUser("Input size of matrix (N x N). N: ");
		generateRandomMatrix();
	}
	else if (argc == 2)
	{
		getMatrixFromFile(argv[1]);
	}
	else
	{
		cerr << "Invalid amount of console arguments: " << argc;
		exit(2);
	}

	threadsCount = readIntegerFromUser("Number of threads to be used: ");
	_rank = N;

	cout << "\n|=============> Matrix <=============|\n";

	for (int i = 0; i < matrix.size(); i++)
	{
		for (int j = 0; j < matrix.size(); j++)
		{
			cout << matrix[i][j] << '\t';
		}
		cout << '\n';
	}

	auto startAt = chrono::steady_clock::now();
	calculateMatrixRank();
	auto endAt = chrono::steady_clock::now();
	auto msPassed = chrono::duration_cast<chrono::milliseconds>(endAt - startAt).count();
	cout << "\nMatrix rank is " << _rank << '\n';
	cout << "Milliseconds spent for calculations: " << msPassed;

	//clear memory allocated to the matrix (which is vector<vector<int>>)
	vector<vector<int>>().swap(matrix);
}




