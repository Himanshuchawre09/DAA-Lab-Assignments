#include <iostream>
#include <limits>
#include <vector>

using namespace std;

struct Route {
    int start;
    int end;
    int cost;
};

struct DeliveryRequest {
    int source;
    int destination;
};

struct RouteResult {
    int totalCost;
    vector<int> path;
};

const int INF = numeric_limits<int>::max();

RouteResult findMinimumRoute(const vector<int>& stages,
                             const vector<Route>& routes,
                             int source, int destination) {
    vector<int> minimumCost(stages.size(), INF);
    vector<int> nextNode(stages.size(), -1);
    int destinationStage = stages[destination];

    minimumCost[destination] = 0;

    for (int currentStage = destinationStage - 1;
         currentStage >= stages[source]; --currentStage) {
        for (int node = 0; node < static_cast<int>(stages.size()); ++node) {
            if (stages[node] != currentStage) {
                continue;
            }

            for (const Route& route : routes) {
                if (route.start != node || stages[route.end] <= currentStage ||
                    stages[route.end] > destinationStage ||
                    minimumCost[route.end] == INF) {
                    continue;
                }

                int routeCost = route.cost + minimumCost[route.end];
                if (routeCost < minimumCost[node]) {
                    minimumCost[node] = routeCost;
                    nextNode[node] = route.end;
                }
            }
        }
    }

    RouteResult result;
    result.totalCost = minimumCost[source];

    if (result.totalCost == INF) {
        return result;
    }

    int current = source;
    while (current != -1) {
        result.path.push_back(current);
        if (current == destination) {
            break;
        }
        current = nextNode[current];
    }

    return result;
}

void displayRequests(const vector<int>& stages, const vector<Route>& routes,
                     const vector<DeliveryRequest>& requests) {
    cout << "\nMinimum-cost delivery routes:\n";

    for (int i = 0; i < static_cast<int>(requests.size()); ++i) {
        const DeliveryRequest& request = requests[i];
        RouteResult result = findMinimumRoute(
            stages, routes, request.source, request.destination);

        cout << "Request " << i + 1 << " (" << request.source << " -> "
             << request.destination << "): ";

        if (result.totalCost == INF || result.path.back() != request.destination) {
            cout << "No valid route\n";
            continue;
        }

        cout << "Path: ";
        for (int j = 0; j < static_cast<int>(result.path.size()); ++j) {
            cout << result.path[j];
            if (j + 1 < static_cast<int>(result.path.size())) {
                cout << " -> ";
            }
        }
        cout << " | Total cost: " << result.totalCost << '\n';
    }
}

int main() {
    int numberOfNodes;
    int numberOfStages;
    int numberOfRoutes;

    cout << "Enter number of stages: ";
    cin >> numberOfStages;
    cout << "Enter number of nodes: ";
    cin >> numberOfNodes;

    if (numberOfStages <= 0 || numberOfNodes <= 0) {
        cerr << "Stages and nodes must be positive.\n";
        return 1;
    }

    vector<int> stages(numberOfNodes);
    cout << "Enter the stage number for each node (0 to "
         << numberOfStages - 1 << "):\n";

    for (int node = 0; node < numberOfNodes; ++node) {
        cout << "Stage of node " << node << ": ";
        cin >> stages[node];

        if (stages[node] < 0 || stages[node] >= numberOfStages) {
            cerr << "Invalid stage number.\n";
            return 1;
        }
    }

    cout << "Enter number of directed routes: ";
    cin >> numberOfRoutes;

    if (numberOfRoutes < 0) {
        cerr << "Number of routes cannot be negative.\n";
        return 1;
    }

    vector<Route> routes;
    cout << "Enter each route as: start end cost\n";

    for (int i = 0; i < numberOfRoutes; ++i) {
        Route route;
        cin >> route.start >> route.end >> route.cost;

        if (route.start < 0 || route.start >= numberOfNodes ||
            route.end < 0 || route.end >= numberOfNodes || route.cost < 0 ||
            stages[route.start] >= stages[route.end]) {
            cerr << "Invalid route. Routes must move to a later stage.\n";
            return 1;
        }

        routes.push_back(route);
    }

    int numberOfRequests;
    cout << "Enter number of delivery requests: ";
    cin >> numberOfRequests;

    if (numberOfRequests <= 0) {
        cerr << "Number of delivery requests must be positive.\n";
        return 1;
    }

    vector<DeliveryRequest> requests(numberOfRequests);
    cout << "Enter each request as: source destination\n";

    for (DeliveryRequest& request : requests) {
        cin >> request.source >> request.destination;

        if (request.source < 0 || request.source >= numberOfNodes ||
            request.destination < 0 || request.destination >= numberOfNodes ||
            stages[request.source] >= stages[request.destination]) {
            cerr << "Invalid delivery request. It must move forward by stage.\n";
            return 1;
        }
    }

    displayRequests(stages, routes, requests);

    int numberOfUpdates;
    cout << "\nEnter number of route cost updates: ";
    cin >> numberOfUpdates;

    for (int i = 0; i < numberOfUpdates; ++i) {
        int routeNumber;
        int newCost;

        cout << "Update as: route_number new_cost\n";
        cin >> routeNumber >> newCost;

        if (routeNumber < 1 || routeNumber > static_cast<int>(routes.size()) ||
            newCost < 0) {
            cerr << "Invalid route update.\n";
            return 1;
        }

        routes[routeNumber - 1].cost = newCost;
        cout << "Route cost updated. Recalculating all requests...\n";
        displayRequests(stages, routes, requests);
    }

    return 0;
}