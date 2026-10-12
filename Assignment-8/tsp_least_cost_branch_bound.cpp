#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <limits>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace std;

const double INF = numeric_limits<double>::infinity();

struct Node {
    vector<vector<double>> matrix; // Reduced matrix for this partial route
    vector<int> path;              // Cities visited so far
    double bound;                  // Lower bound on any completion of this route
};

struct CompareBound {
    // priority_queue is a max-heap by default; reverse the comparison for a min-heap.
    bool operator()(const Node& a, const Node& b) const {
        return a.bound > b.bound;
    }
};

// Subtract each row minimum and then each column minimum.
// The reductions add to the node's lower bound. Return INF if no completion
// can leave a required row or enter a required column.
double reduceMatrix(vector<vector<double>>& matrix) {
    const int n = static_cast<int>(matrix.size());
    double reductionCost = 0.0;

    for (int i = 0; i < n; ++i) {
        double smallest = INF;
        for (int j = 0; j < n; ++j)
            smallest = min(smallest, matrix[i][j]);

        if (isinf(smallest)) continue; // This row has no possible edge.
        if (smallest > 0.0) {
            reductionCost += smallest;
            for (int j = 0; j < n; ++j)
                if (!isinf(matrix[i][j])) matrix[i][j] -= smallest;
        }
    }

    for (int j = 0; j < n; ++j) {
        double smallest = INF;
        for (int i = 0; i < n; ++i)
            smallest = min(smallest, matrix[i][j]);

        if (isinf(smallest)) continue; // This column has no possible edge.
        if (smallest > 0.0) {
            reductionCost += smallest;
            for (int i = 0; i < n; ++i)
                if (!isinf(matrix[i][j])) matrix[i][j] -= smallest;
        }
    }
    return reductionCost;
}

bool parseCost(const string& token, double& value) {
    if (token == "-1" || token == "INF" || token == "inf" ||
        token == "X" || token == "x") {
        value = INF;
        return true;
    }

    try {
        size_t used = 0;
        value = stod(token, &used);
        return used == token.size() && isfinite(value) && value >= 0.0;
    } catch (const invalid_argument&) {
        return false;
    } catch (const out_of_range&) {
        return false;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cout << "Step 1 - Enter the number of cities (a positive whole number): "
         << flush;
    if (!(cin >> n) || n <= 0) {
        cout << "Invalid input: number of cities must be a positive integer.\n";
        return 1;
    }

    vector<string> cityNames(n);
    cout << "\nStep 2 - Enter each city name on its own line.\n"
         << "The first city you enter is the starting city (city 0).\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < n; ++i) {
        cout << "City " << i + 1 << ": " << flush;
        getline(cin, cityNames[i]);
        if (cityNames[i].empty()) {
            cout << "Invalid input: city names cannot be empty.\n";
            return 1;
        }
    }

    vector<vector<double>> original(n, vector<double>(n, INF));
    cout << "\nStep 3 - Enter the " << n << " x " << n
         << " travel-cost matrix, one row at a time.\n"
         << "Each row needs " << n
         << " space-separated costs, in the same city order as the names.\n"
         << "Rows represent travel FROM a city; columns represent travel TO a city.\n"
         << "Use a non-negative number for a direct route. Use -1, INF, or X "
         << "when there is no direct route.\n"
         << "Example row for 4 cities: 0 10 X 20\n"
         << "Enter the rows below:\n";
    for (int i = 0; i < n; ++i) {
        cout << "Row " << i + 1 << " (from " << cityNames[i] << "): "
             << flush;
        for (int j = 0; j < n; ++j) {
            string token;
            double cost;
            if (!(cin >> token) || !parseCost(token, cost)) {
                cout << "Invalid input: costs must be non-negative numbers or an impossible-edge marker.\n";
                return 1;
            }
            // A tour never uses a city-to-itself edge, regardless of input.
            if (i != j) original[i][j] = cost;
        }
    }

    // A single city is a trivial cycle with no travel.
    if (n == 1) {
        cout << "\nOptimal route: " << cityNames[0] << " -> " << cityNames[0]
             << "\nMinimum total cost: 0\n";
        return 0;
    }

    vector<vector<double>> rootMatrix = original;
    double rootBound = reduceMatrix(rootMatrix);
    if (isinf(rootBound)) {
        cout << "\nNo Hamiltonian cycle exists for the supplied costs.\n";
        return 0;
    }

    priority_queue<Node, vector<Node>, CompareBound> open;
    open.push({rootMatrix, {0}, rootBound}); // City 0 is the fixed start.

    double bestCost = INF;
    vector<int> bestPath;

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        // Every completion costs at least its bound, so this node can be pruned.
        if (current.bound >= bestCost) continue;

        int lastCity = current.path.back();
        if (static_cast<int>(current.path.size()) == n) {
            // The reduced matrix still contains the final edge back to city 0.
            double returnEdge = current.matrix[lastCity][0];
            if (!isinf(returnEdge)) {
                double completeCost = current.bound + returnEdge;
                if (completeCost < bestCost) {
                    bestCost = completeCost;
                    bestPath = current.path;
                }
            }
            continue;
        }

        for (int nextCity = 0; nextCity < n; ++nextCity) {
            if (find(current.path.begin(), current.path.end(), nextCity) != current.path.end())
                continue;

            // Use the edge's value in the reduced matrix. The original cost
            // would double-count reductions already included in current.bound.
            double selectedEdge = current.matrix[lastCity][nextCity];
            if (isinf(selectedEdge)) continue;

            Node child;
            child.matrix = current.matrix;
            child.path = current.path;
            child.path.push_back(nextCity);

            // The selected edge is now fixed: leave lastCity once and enter
            // nextCity once. Do not allow a premature return to the start.
            for (int k = 0; k < n; ++k) {
                child.matrix[lastCity][k] = INF;
                child.matrix[k][nextCity] = INF;
            }
            if (static_cast<int>(child.path.size()) < n)
                child.matrix[nextCity][0] = INF;

            double extraReduction = reduceMatrix(child.matrix);
            child.bound = current.bound + selectedEdge + extraReduction;

            if (child.bound < bestCost)
                open.push(child);
        }
    }

    if (bestPath.empty()) {
        cout << "\nNo Hamiltonian cycle exists for the supplied costs.\n";
        return 0;
    }

    cout << fixed << setprecision(2);
    cout << "\nOptimal route: ";
    for (int city : bestPath) cout << cityNames[city] << " -> ";
    cout << cityNames[0] << "\n";
    cout << "Minimum total cost: " << bestCost << "\n";
    return 0;
}
