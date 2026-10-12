#include <algorithm>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

struct Item {
    string name;
    int weight;
    long long utility;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long capacityInput;
    cout << "Step 1 - Enter the truck capacity as a non-negative whole number: "
         << flush;
    if (!(cin >> capacityInput) || capacityInput < 0 ||
        capacityInput > numeric_limits<int>::max()) {
        cout << "Invalid input: truck capacity must be a non-negative whole number "
             << "that fits in an int.\n";
        return 1;
    }
    const int capacity = static_cast<int>(capacityInput);

    long long itemCountInput;
    cout << "\nStep 2 - Enter the number of available items as a non-negative "
         << "whole number: " << flush;
    if (!(cin >> itemCountInput) || itemCountInput < 0 ||
        itemCountInput > numeric_limits<int>::max()) {
        cout << "Invalid input: number of items must be a non-negative whole "
             << "number that fits in an int.\n";
        return 1;
    }
    const int itemCount = static_cast<int>(itemCountInput);

    vector<Item> items;
    try {
        items.reserve(static_cast<size_t>(itemCount));
    } catch (const bad_alloc&) {
        cout << "Unable to store that many items. Please enter a smaller number.\n";
        return 1;
    } catch (const length_error&) {
        cout << "The number of items is too large. Please enter a smaller number.\n";
        return 1;
    }

    cout << "\nStep 3 - Enter each item using this format:\n"
         << "  Item name (one line; spaces are allowed)\n"
         << "  Weight Utility\n"
         << "For example:\n"
         << "  First-aid kit\n"
         << "  4 25\n"
         << "Weight must be a positive whole number. Utility must be a "
         << "non-negative whole number.\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for (int i = 0; i < itemCount; ++i) {
        string name;
        cout << "\nItem " << i + 1 << " of " << itemCount
             << " - Enter the item name: " << flush;
        if (!getline(cin, name)) {
            cout << "Invalid input: could not read the item name.\n";
            return 1;
        }
        if (name.find_first_not_of(" \t\r") == string::npos) {
            cout << "Invalid input: item names cannot be empty.\n";
            return 1;
        }

        string valuesLine;
        long long weightInput;
        long long utility;
        string extraToken;
        cout << "Enter its weight and utility, separated by a space: " << flush;
        if (!getline(cin, valuesLine)) {
            cout << "Invalid input: could not read the item's weight and utility.\n";
            return 1;
        }
        istringstream valuesInput(valuesLine);
        if (!(valuesInput >> weightInput >> utility) ||
            (valuesInput >> extraToken) || weightInput <= 0 ||
            weightInput > numeric_limits<int>::max() || utility < 0) {
            cout << "Invalid input: weight must be a positive whole number that "
                 << "fits in an int, and utility must be a non-negative whole "
                 << "number. Enter exactly two values.\n";
            return 1;
        }
        items.push_back({name, static_cast<int>(weightInput), utility});
    }

    vector<vector<long long>> dp;
    try {
        dp.assign(static_cast<size_t>(itemCount) + 1,
                  vector<long long>(static_cast<size_t>(capacity) + 1, 0));
    } catch (const bad_alloc&) {
        cout << "Unable to allocate the dynamic programming table. Please use a "
             << "smaller capacity or fewer items.\n";
        return 1;
    } catch (const length_error&) {
        cout << "The dynamic programming table would be too large. Please use a "
             << "smaller capacity or fewer items.\n";
        return 1;
    }

    // dp[i][w] is the greatest utility possible using the first i items
    // with a truck capacity of w.
    for (int i = 1; i <= itemCount; ++i) {
        const Item& item = items[i - 1];
        for (int w = 0; w <= capacity; ++w) {
            // First consider leaving the current item out.
            dp[i][w] = dp[i - 1][w];

            // Or include it once, if it fits; use the previous row so it
            // cannot be selected more than once.
            if (item.weight <= w) {
                const long long previousUtility = dp[i - 1][w - item.weight];
                if (item.utility >
                    numeric_limits<long long>::max() - previousUtility) {
                    cout << "Utility total is too large to represent. Please use "
                         << "smaller utility values.\n";
                    return 1;
                }
                dp[i][w] = max(dp[i][w], previousUtility + item.utility);
            }
        }
    }

    // Trace backwards through the table to find the chosen items.
    vector<int> selectedItems;
    int remainingCapacity = capacity;
    for (int i = itemCount; i > 0; --i) {
        if (dp[i][remainingCapacity] != dp[i - 1][remainingCapacity]) {
            selectedItems.push_back(i - 1);
            remainingCapacity -= items[i - 1].weight;
        }
    }
    reverse(selectedItems.begin(), selectedItems.end());

    int totalWeight = 0;
    for (int index : selectedItems)
        totalWeight += items[index].weight;

    cout << "\nMaximum total utility: " << dp[itemCount][capacity] << "\n";
    cout << "Selected items:\n";
    if (selectedItems.empty()) {
        cout << "  (No items selected)\n";
    } else {
        for (int index : selectedItems) {
            cout << "  " << items[index].name
                 << " (weight: " << items[index].weight
                 << ", utility: " << items[index].utility << ")\n";
        }
    }
    cout << "Total weight: " << totalWeight << "\n";
    cout << "Total utility: " << dp[itemCount][capacity] << "\n";
    cout << "Remaining truck capacity: " << capacity - totalWeight << "\n";

    return 0;
}
