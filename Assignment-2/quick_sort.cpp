#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Movie {
    string title;
    double rating;
    int releaseYear;
    int votes;
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

double toDouble(const string& value) {
    if (value.empty()) {
        return 0.0;
    }

    try {
        return stod(value);
    } catch (...) {
        return 0.0;
    }
}

int toInteger(const string& value) {
    if (value.empty()) {
        return 0;
    }

    try {
        return stoi(value);
    } catch (...) {
        return 0;
    }
}

int getReleaseYear(const string& releaseDate) {
    if (releaseDate.length() < 4) {
        return 0;
    }

    return toInteger(releaseDate.substr(0, 4));
}

bool comesBefore(const Movie& first, const Movie& second, int parameter) {
    if (parameter == 1) {
        return first.title > second.title;
    }
    if (parameter == 2) {
        return first.rating > second.rating;
    }
    if (parameter == 3) {
        return first.releaseYear > second.releaseYear;
    }
    return first.votes > second.votes;
}

int partition(vector<Movie>& movies, int low, int high, int parameter) {
    Movie pivot = movies[high];
    int smallerIndex = low - 1;

    for (int current = low; current < high; ++current) {
        if (comesBefore(movies[current], pivot, parameter)) {
            ++smallerIndex;
            Movie temporary = movies[smallerIndex];
            movies[smallerIndex] = movies[current];
            movies[current] = temporary;
        }
    }

    Movie temporary = movies[smallerIndex + 1];
    movies[smallerIndex + 1] = movies[high];
    movies[high] = temporary;

    return smallerIndex + 1;
}

void quickSort(vector<Movie>& movies, int low, int high, int parameter) {
    if (low >= high) {
        return;
    }

    int pivotIndex = partition(movies, low, high, parameter);
    quickSort(movies, low, pivotIndex - 1, parameter);
    quickSort(movies, pivotIndex + 1, high, parameter);
}

int main() {
    ifstream inputFile("final_dataset.csv");

    if (!inputFile) {
        cerr << "Error: Could not open final_dataset.csv\n";
        return 1;
    }

    vector<Movie> movies;
    string line;
    getline(inputFile, line);

    while (getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> fields = parseCsvLine(line);
        if (fields.size() >= 17) {
            movies.push_back({
                fields[1],
                toDouble(fields[4]),
                getReleaseYear(fields[16]),
                toInteger(fields[5])
            });
        }
    }

    int parameter;
    cout << "Choose a parameter to sort by:\n";
    cout << "1. Movie title\n";
    cout << "2. IMDb rating\n";
    cout << "3. Release year\n";
    cout << "4. Votes\n";
    cout << "Enter your choice: ";
    cin >> parameter;

    if (parameter < 1 || parameter > 4) {
        cerr << "Invalid choice.\n";
        return 1;
    }

    if (!movies.empty()) {
        quickSort(movies, 0, static_cast<int>(movies.size()) - 1, parameter);
    }

    cout << "\nTotal number of movies loaded: " << movies.size() << "\n";
    size_t moviesToDisplay = movies.size() < 20 ? movies.size() : 20;
    cout << "First " << moviesToDisplay << " sorted movies:\n";

    for (size_t i = 0; i < moviesToDisplay; ++i) {
        cout << movies[i].title;

        if (parameter == 2) {
            cout << " | IMDb Rating: " << movies[i].rating;
        } else if (parameter == 3) {
            cout << " | Release Year: " << movies[i].releaseYear;
        } else if (parameter == 4) {
            cout << " | Votes: " << movies[i].votes;
        }

        cout << '\n';
    }

    return 0;
}