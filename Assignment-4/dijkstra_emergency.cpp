#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

using namespace std;

struct Edge {
    int destination;
    int travelTime;
};

const int INF = numeric_limits<int>::max();

void addRoad(vector<vector<Edge>>& graph, int first, int second, int travelTime) {
    graph[first].push_back({second, travelTime});
    graph[second].push_back({first, travelTime});
}

void updateRoad(vector<vector<Edge>>& graph, int first, int second, int travelTime) {
    for (Edge& edge : graph[first]) {
        if (edge.destination == second) {
            edge.travelTime = travelTime;
        }
    }

    for (Edge& edge : graph[second]) {
        if (edge.destination == first) {
            edge.travelTime = travelTime;
        }
    }
}

void dijkstra(const vector<vector<Edge>>& graph, int source,
              vector<int>& distances, vector<int>& previous) {
    distances.assign(graph.size(), INF);
    previous.assign(graph.size(), -1);

    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>> pending;

    distances[source] = 0;
    pending.push({0, source});

    while (!pending.empty()) {
        int currentTime = pending.top().first;
        int currentNode = pending.top().second;
        pending.pop();

        if (currentTime != distances[currentNode]) {
            continue;
        }

        for (const Edge& edge : graph[currentNode]) {
            int newTime = currentTime + edge.travelTime;

            if (newTime < distances[edge.destination]) {
                distances[edge.destination] = newTime;
                previous[edge.destination] = currentNode;
                pending.push({newTime, edge.destination});
            }
        }
    }
}

void displayRoute(int source, int hospital, const vector<int>& distances,
                  const vector<int>& previous) {
    if (distances[hospital] == INF) {
        cout << "Hospital " << hospital << ": unreachable\n";
        return;
    }

    vector<int> path;
    int current = hospital;

    while (current != -1) {
        path.push_back(current);
        current = previous[current];
    }

    cout << "Hospital " << hospital
         << " | Travel time: " << distances[hospital] << " minutes"
         << " | Path: ";

    for (int i = static_cast<int>(path.size()) - 1; i >= 0; --i) {
        cout << path[i];
        if (i > 0) {
            cout << " -> ";
        }
    }

    if (path.back() != source) {
        cout << " (invalid path)";
    }

    cout << '\n';
}

void displayAllRoutes(const vector<vector<Edge>>& graph, int source,
                      const vector<int>& hospitals) {
    vector<int> distances;
    vector<int> previous;
    dijkstra(graph, source, distances, previous);

    int bestHospital = -1;
    int bestTime = INF;

    cout << "\nShortest routes from ambulance source " << source << ":\n";

    for (int hospital : hospitals) {
        displayRoute(source, hospital, distances, previous);

        if (distances[hospital] < bestTime) {
            bestTime = distances[hospital];
            bestHospital = hospital;
        }
    }

    if (bestHospital == -1) {
        cout << "No hospital is reachable from the ambulance source.\n";
    } else {
        cout << "Optimal hospital: " << bestHospital
             << " with travel time " << bestTime << " minutes\n";
        cout << "Optimal path: ";

        int current = bestHospital;
        vector<int> bestPath;
        while (current != -1) {
            bestPath.push_back(current);
            current = previous[current];
        }

        for (int i = static_cast<int>(bestPath.size()) - 1; i >= 0; --i) {
            cout << bestPath[i];
            if (i > 0) {
                cout << " -> ";
            }
        }
        cout << '\n';
    }
}

int main() {
    int numberOfIntersections;
    int numberOfRoads;

    cout << "Enter number of intersections: ";
    cin >> numberOfIntersections;

    cout << "Enter number of roads: ";
    cin >> numberOfRoads;

    if (numberOfIntersections <= 0 || numberOfRoads < 0) {
        cerr << "Invalid number of intersections or roads.\n";
        return 1;
    }

    vector<vector<Edge>> graph(numberOfIntersections);

    cout << "Enter each road as: start end travel_time\n";
    for (int i = 0; i < numberOfRoads; ++i) {
        int first;
        int second;
        int travelTime;
        cin >> first >> second >> travelTime;

        if (first < 0 || first >= numberOfIntersections ||
            second < 0 || second >= numberOfIntersections || travelTime <= 0) {
            cerr << "Invalid road details.\n";
            return 1;
        }

        addRoad(graph, first, second, travelTime);
    }

    int source;
    int numberOfHospitals;
    cout << "Enter ambulance source intersection: ";
    cin >> source;
    cout << "Enter number of hospitals: ";
    cin >> numberOfHospitals;

    if (source < 0 || source >= numberOfIntersections || numberOfHospitals <= 0) {
        cerr << "Invalid source or number of hospitals.\n";
        return 1;
    }

    vector<int> hospitals(numberOfHospitals);
    cout << "Enter hospital intersections: ";
    for (int& hospital : hospitals) {
        cin >> hospital;
        if (hospital < 0 || hospital >= numberOfIntersections) {
            cerr << "Invalid hospital intersection.\n";
            return 1;
        }
    }

    displayAllRoutes(graph, source, hospitals);

    int numberOfUpdates;
    cout << "\nEnter number of traffic updates: ";
    cin >> numberOfUpdates;

    for (int i = 0; i < numberOfUpdates; ++i) {
        int first;
        int second;
        int newTravelTime;

        cout << "Update road as: start end new_travel_time\n";
        cin >> first >> second >> newTravelTime;

        if (first < 0 || first >= numberOfIntersections ||
            second < 0 || second >= numberOfIntersections || newTravelTime <= 0) {
            cerr << "Invalid traffic update.\n";
            return 1;
        }

        updateRoad(graph, first, second, newTravelTime);
        cout << "Traffic updated. Recalculating shortest routes...\n";
        displayAllRoutes(graph, source, hospitals);
    }

    return 0;
}