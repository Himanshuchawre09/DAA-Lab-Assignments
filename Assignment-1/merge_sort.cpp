#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Order {
    string orderId;
    string customerId;
    string purchaseTimestamp;
};

vector<string> parseCsvLine(const string& line) {
    vector<string> fields;
    string field;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char current = line[i];

        if (current == '"') {
            if (insideQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (current == ',' && !insideQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += current;
        }
    }

    fields.push_back(field);
    return fields;
}

void merge(vector<Order>& orders, vector<Order>& temporary,
        size_t left, size_t middle, size_t right) {
    size_t i = left;
    size_t j = middle + 1;
    size_t k = left;

    while (i <= middle && j <= right) {
        if (orders[i].purchaseTimestamp <= orders[j].purchaseTimestamp) {
            temporary[k++] = orders[i++];
        } else {
            temporary[k++] = orders[j++];
        }
    }

    while (i <= middle) {
        temporary[k++] = orders[i++];
    }

    while (j <= right) {
        temporary[k++] = orders[j++];
    }

    for (size_t index = left; index <= right; ++index) {
        orders[index] = temporary[index];
    }
}

void mergeSort(vector<Order>& orders, vector<Order>& temporary,
               size_t left, size_t right) {
    if (left >= right) {
        return;
    }

    size_t middle = left + (right - left) / 2;
    mergeSort(orders, temporary, left, middle);
    mergeSort(orders, temporary, middle + 1, right);
    merge(orders, temporary, left, middle, right);
}

int main() {
    ifstream inputFile("olist_orders_dataset.csv");

    if (!inputFile) {
        cerr << "Error: Could not open olist_orders_dataset.csv\n";
        return 1;
    }

    vector<Order> orders;
    string line;

    getline(inputFile, line);

    while (getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> fields = parseCsvLine(line);
        if (fields.size() >= 4) {
            orders.push_back({fields[0], fields[1], fields[3]});
        }
    }

    cout << "Total number of orders loaded: " << orders.size() << "\n";

    if (!orders.empty()) {
        vector<Order> temporary(orders.size());
        mergeSort(orders, temporary, 0, orders.size() - 1);
    }

    size_t ordersToDisplay = orders.size() < 20 ? orders.size() : 20;
    cout << "\nFirst " << ordersToDisplay << " sorted orders:\n";

    for (size_t i = 0; i < ordersToDisplay; ++i) {
        cout << orders[i].purchaseTimestamp << '\n';
    }

    return 0;
}