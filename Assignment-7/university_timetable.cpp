#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Room {
    string name;
    int capacity;
};

int main() {
    int numberOfCourses;
    cout << "Enter number of courses: ";
    cin >> numberOfCourses;

    if (numberOfCourses <= 0) {
        cerr << "Number of courses must be positive.\n";
        return 1;
    }

    vector<string> courses(numberOfCourses);
    unordered_map<string, int> courseIndex;
    cout << "Enter course names (one word each):\n";

    for (int i = 0; i < numberOfCourses; ++i) {
        cin >> courses[i];
        if (courseIndex.count(courses[i]) > 0) {
            cerr << "Course names must be unique.\n";
            return 1;
        }
        courseIndex[courses[i]] = i;
    }

    vector<vector<bool>> adjacent(
        numberOfCourses, vector<bool>(numberOfCourses, false));
    vector<int> numberOfStudents(numberOfCourses, 0);

    int numberOfStudentsInput;
    cout << "Enter number of students: ";
    cin >> numberOfStudentsInput;
    if (numberOfStudentsInput < 0) {
        cerr << "Number of students cannot be negative.\n";
        return 1;
    }

    for (int student = 0; student < numberOfStudentsInput; ++student) {
        string studentName;
        int numberOfEnrollments;
        cout << "Enter student name and number of enrolled courses: ";
        cin >> studentName >> numberOfEnrollments;

        if (numberOfEnrollments < 0 || numberOfEnrollments > numberOfCourses) {
            cerr << "Invalid number of course enrollments.\n";
            return 1;
        }

        vector<int> enrolledCourses;
        vector<bool> alreadyEnrolled(numberOfCourses, false);
        cout << "Enter the course names: ";
        for (int i = 0; i < numberOfEnrollments; ++i) {
            string courseName;
            cin >> courseName;

            if (courseIndex.count(courseName) == 0) {
                cerr << "Unknown course: " << courseName << '\n';
                return 1;
            }

            int index = courseIndex[courseName];
            if (!alreadyEnrolled[index]) {
                enrolledCourses.push_back(index);
                alreadyEnrolled[index] = true;
                ++numberOfStudents[index];
            }
        }

        for (int i = 0; i < static_cast<int>(enrolledCourses.size()); ++i) {
            for (int j = i + 1; j < static_cast<int>(enrolledCourses.size()); ++j) {
                int firstCourse = enrolledCourses[i];
                int secondCourse = enrolledCourses[j];
                adjacent[firstCourse][secondCourse] = true;
                adjacent[secondCourse][firstCourse] = true;
            }
        }
    }

    vector<int> degree(numberOfCourses, 0);
    vector<int> sortedCourses(numberOfCourses);
    for (int course = 0; course < numberOfCourses; ++course) {
        sortedCourses[course] = course;
        for (int other = 0; other < numberOfCourses; ++other) {
            if (adjacent[course][other]) {
                ++degree[course];
            }
        }
    }

    sort(sortedCourses.begin(), sortedCourses.end(),
         [&degree](int first, int second) {
             return degree[first] > degree[second];
         });

    vector<int> color(numberOfCourses, -1);
    for (int course : sortedCourses) {
        vector<bool> availableColors(numberOfCourses, true);
        for (int other = 0; other < numberOfCourses; ++other) {
            if (adjacent[course][other] && color[other] != -1) {
                availableColors[color[other]] = false;
            }
        }

        for (int candidateColor = 0; candidateColor < numberOfCourses;
             ++candidateColor) {
            if (availableColors[candidateColor]) {
                color[course] = candidateColor;
                break;
            }
        }
    }

    int numberOfRooms;
    cout << "Enter number of rooms: ";
    cin >> numberOfRooms;
    if (numberOfRooms < 0) {
        cerr << "Number of rooms cannot be negative.\n";
        return 1;
    }

    vector<Room> rooms(numberOfRooms);
    cout << "Enter each room name and capacity:\n";
    for (Room& room : rooms) {
        cin >> room.name >> room.capacity;
        if (room.capacity < 0) {
            cerr << "Room capacity cannot be negative.\n";
            return 1;
        }
    }

    vector<int> sortedRooms(numberOfRooms);
    for (int i = 0; i < numberOfRooms; ++i) {
        sortedRooms[i] = i;
    }
    sort(sortedRooms.begin(), sortedRooms.end(),
         [&rooms](int first, int second) {
             return rooms[first].capacity > rooms[second].capacity;
         });

    vector<int> assignedRoom(numberOfCourses, -1);
    int numberOfTimeSlots = 0;
    for (int assignedColor : color) {
        numberOfTimeSlots = max(numberOfTimeSlots, assignedColor + 1);
    }

    for (int timeSlot = 0; timeSlot < numberOfTimeSlots; ++timeSlot) {
        vector<int> exams;
        for (int course = 0; course < numberOfCourses; ++course) {
            if (color[course] == timeSlot) {
                exams.push_back(course);
            }
        }

        sort(exams.begin(), exams.end(),
             [&numberOfStudents](int first, int second) {
                 return numberOfStudents[first] > numberOfStudents[second];
             });

        vector<bool> roomOccupied(numberOfRooms, false);
        for (int course : exams) {
            for (int room : sortedRooms) {
                if (!roomOccupied[room] &&
                    rooms[room].capacity >= numberOfStudents[course]) {
                    assignedRoom[course] = room;
                    roomOccupied[room] = true;
                    break;
                }
            }
        }
    }

    cout << "\nFinal Timetable:\n";
    for (int course = 0; course < numberOfCourses; ++course) {
        cout << "Course: " << courses[course] << " -> Time Slot: "
             << color[course] << " -> Room: ";
        if (assignedRoom[course] == -1) {
            cout << "No room available\n";
        } else {
            cout << rooms[assignedRoom[course]].name << '\n';
        }
    }

    return 0;
}
