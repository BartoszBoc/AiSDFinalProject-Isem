#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <tuple>
#include <random>

using namespace std;

class Point {
public:
    vector<double> values;
    int dimension;

    Point(vector<double> _values, int _dimension) : values(_values), dimension(_dimension) {
        if (values.size() != dimension) cout << "Error: the points dimension doesn't match.\n";
    };              // Pretty self-explanatory

    void writeout() {
        int i = 0;
        cout << "(";                    // Basic writing-out function
        for (double val : values) {
            cout << val;
            if (i < dimension - 1) cout << ", ";
            i++;
        }
        cout << ")";
    }

    double EuclideanDistance(Point point) {         // Calculating the distance, Euclidean-style
        if (dimension != point.dimension) {
            cout << "The dimensions of the points don't match! Returning 0...\n";
            return 0.0;
        }

        double result = 0.0;
        for (int i = 0; i < dimension; i++) {
            result += (point.values[i] - values[i]) * (point.values[i] - values[i]);
        }

        return sqrt(result);
    }
};

class DataBase {
    const string filename;              // Database name (the file)
public:
    vector<Point> points;
    int dimension;

    DataBase(string _filename) : filename(_filename) {};

    void FileManagement() {
        ifstream database(filename);

        if (!database.is_open()) {          // If the database can't be opened it means there isn't one - creating one
            cout << "Creating new database...\n";
            ofstream newDatabase(filename);
            if (!newDatabase.is_open()) {   // If it still isn't opened then that's just an error
                cout << "Error: cannot create new file.\n";
                return;
            }

            cout << "Insert the amount of dimensions you're willing to work with:\n";
            cin >> dimension;
            newDatabase << "dimensions: " << dimension << "\n"; // If we're creating a new base, we need it's dimensions
            newDatabase.close();
            return;
        }

        string vectorValues;
        int i = 1;

        while (getline(database, vectorValues)) {
            istringstream reader(vectorValues);
            if (i == 1) {   // This first iteration of the while loop is to take the dimension value
                string temp;
                reader >> temp;
                reader >> i;
                dimension = i;
                i = 0;
                continue;
            }
            vector<double> values;
            double value;
            while (reader >> value) values.push_back(value);    // Taking the points from the file
            points.emplace_back(values, dimension);
        }
        cout << "The database you'll be using has " << dimension << " dimensions.\n";
    }

    void insertPoint(Point point) {
        for (Point pnt : points) if (pnt.values == point.values) return;    // Not adding identical points...
        if (point.dimension != dimension) return;                           // ...and not adding ones with unmatched dimensions

        points.push_back(point);

        ofstream database(filename, ios::app);
        if (!database.is_open()) {
            cout << "Error: can not open the file.\n";
            return;
        }

        for (double val : point.values) database << val << " ";  // Writing out the point vals into the database
        database << "\n";

        database.close();
    }

    tuple<Point, double, int> closestPoint(int choice) {
        Point temp = points[choice - 1];
        Point result = points[0];
        double distance = 0, closestDistance = -1.0;
        int chosenID = 0;
        for (int i = 0; i < points.size(); i++) {
            if (i == choice - 1) continue;
            distance = temp.EuclideanDistance(points[i]);
            if (distance < closestDistance || closestDistance < 0) {
                closestDistance = distance;
                chosenID = i + 1;
                result = points[i];
            }
        }
        return { result, closestDistance, chosenID };   // Returning the point, the distance and the point's ID
    }

    void clearBase() {
        ifstream database(filename);
        if (!database.is_open()) {
            cout << "Error: file could not be opened.\n";
            return;
        }

        string Dimensions;
        getline(database, Dimensions);  // Taking the line with the info on the amount of dimensions
        database.close();

        ofstream clearedbase(filename, ios::trunc); // New stream
        if (!clearedbase.is_open()) {
            cout << "Error: file could not be opened.\n";
            return;
        }

        clearedbase << Dimensions << "\n";      // Only keeping the dimensions...
        clearedbase.close();
        points.clear();
    }

    void pointsWriteout() {
        int i = 1;
        for (Point point : points) {
            cout << i << ". ";
            point.writeout();
            cout << "\n";
            i++;
        }
    }
};

int main()
{
    string databaseName;
    cout << "=====\nInput the name of an existing database, or a completely new name to create one:\n=====\n";
    cin >> databaseName;

    DataBase database = DataBase(databaseName);
    database.FileManagement();

    // Those are merely for avoidance of declarations in switch cases
    vector<double> helpfulVector(database.dimension);
    Point temp = Point({ 0 }, 1);
    int i = 1, c = 0;
    double value = 0;
    // Those are merely for avoidance of declarations in switch cases

    cout << "\nWelcome to the program!\n";
    while (1) {
        cout << "Select one of the generously provided options below in order to progress: \n" <<
            "Type 1 to: add another point.\nType 2 to: display all the current points.\n" <<
            "Type 3 to: find the closest point to the point of your choice.\nType 4 to: " <<
            "run standardized test created by the developer.\nType 5 to: exit.\n\n";
        cin >> c;

        switch (c) {

        case 1:
            cout << "Type out the values of the point, separated by SPACE, ending in ENTER.\n" <<
                "Caution:\nIf not enough values are inputted, you can input more.\nAny extra values will be ignored by the program.\n";
            for (int i = 0; i < database.dimension; i++) {
                cin >> value;
                helpfulVector.push_back(value);
            }

            temp = Point(helpfulVector, database.dimension);
            database.insertPoint(temp);
            helpfulVector.clear();
            break;

        case 2:
            database.pointsWriteout();
            break;

        case 3:
            cout << "Choose which point you want to find the point closest to:\n";
            int choice;
            cin >> choice;

            tie(temp, value, i) = database.closestPoint(choice);
            cout << "The closest point to point no. (" << choice << ") is the point no. (" <<
                i << "):\n";
            temp.writeout();
            cout << "\n...with the distance of " << value << ".\n";
            break;

        case 4:
            database.clearBase();
            cout << "\nCurrent base has been cleared to conduct tests.\n";
            cout << "Adding 8 vectors with random numbers.\n";

            srand(time(NULL));
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < database.dimension; k++) {
                    helpfulVector[k] = (rand() % 20) * pow(-1, round(rand()));
                }
                temp = Point(helpfulVector, database.dimension);
                database.insertPoint(temp);
            }

            cout << "Writing out...\n\n";
            database.pointsWriteout();

            cout << "\nChecking closest points for every vector with even ID.\n\n";
            for (int j = 2; j <= 8; j += 2) {
                tie(temp, value, i) = database.closestPoint(j);
                cout << "Closest point to (" << j << ") is (" <<
                    i << "): ";
                temp.writeout();
                cout << " with the distance of " << value << ".\n\n";
            }

            cout << "Tests concluded!\n\n";

            break;

        case 5:
            cout << "Exiting, goodbye!";
            return 0;

        default:
            cout << "Incorrect input, try again.\n";
        }
    }
    return 0;
}