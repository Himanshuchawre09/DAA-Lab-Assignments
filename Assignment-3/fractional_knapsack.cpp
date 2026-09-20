#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Supply {
    string name;
    double weight;
    double utility;
    double ratio;
    bool divisible;
};

void sortByRatio(vector<Supply>& supplies) {
    for (size_t i = 1; i < supplies.size(); ++i) {
        Supply current = supplies[i];
        int position = static_cast<int>(i) - 1;

        while (position >= 0 && supplies[position].ratio < current.ratio) {
            supplies[position + 1] = supplies[position];
            --position;
        }

        supplies[position + 1] = current;
    }
}

int main() {
    double capacity;
    int numberOfSupplies;

    cout << "Enter boat capacity: ";
    cin >> capacity;

    cout << "Enter number of supplies: ";
    cin >> numberOfSupplies;
    cin.ignore();

    if (capacity <= 0 || numberOfSupplies <= 0) {
        cerr << "Capacity and number of supplies must be positive.\n";
        return 1;
    }

    vector<Supply> supplies;

    for (int i = 0; i < numberOfSupplies; ++i) {
        Supply supply;
        char divisibleChoice;

        cout << "\nSupply " << i + 1 << " name: ";
        getline(cin, supply.name);

        cout << "Weight: ";
        cin >> supply.weight;

        cout << "Utility value: ";
        cin >> supply.utility;

        cout << "Is this supply divisible? (y/n): ";
        cin >> divisibleChoice;
        cin.ignore();

        if (supply.weight <= 0 || supply.utility < 0) {
            cerr << "Weight must be positive and utility cannot be negative.\n";
            return 1;
        }

        supply.ratio = supply.utility / supply.weight;
        supply.divisible = divisibleChoice == 'y' || divisibleChoice == 'Y';
        supplies.push_back(supply);
    }

    sortByRatio(supplies);

    double remainingCapacity = capacity;
    double totalUtility = 0.0;

    cout << "\nSelected supplies:\n";
    cout << fixed << setprecision(2);

    for (const Supply& supply : supplies) {
        if (remainingCapacity <= 0) {
            break;
        }

        double amountTaken = 0.0;
        double utilityCollected = 0.0;

        if (supply.weight <= remainingCapacity) {
            amountTaken = supply.weight;
            utilityCollected = supply.utility;
        } else if (supply.divisible) {
            amountTaken = remainingCapacity;
            utilityCollected = amountTaken * supply.ratio;
        }

        if (amountTaken > 0) {
            remainingCapacity -= amountTaken;
            totalUtility += utilityCollected;

            cout << supply.name
                      << " | Amount taken: " << amountTaken
                      << " | Utility gained: " << utilityCollected << '\n';
        }
    }

    cout << "\nTotal utility: " << totalUtility << '\n';
    cout << "Remaining boat capacity: " << remainingCapacity << '\n';

    return 0;
}