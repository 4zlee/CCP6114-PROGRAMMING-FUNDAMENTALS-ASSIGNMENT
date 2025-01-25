// *********************************************************
// Program: TC3L_G04.cpp
// Course: CCP6114 Programming Fundamentals
// Lecture Class: TC3L
// Tutorial Class: TT5L
// Trimester: 2430
// Member_1: 242UC244Q9 | NURUL AZLEA UMAIRAH BINTI MOHD AZMAN | NURUL.AZLEA.UMAIRAH@student.mmu.edu.my | 010-958 4438
// Member_2: 242UC244QP | NURUL FAZLINA ZAFUAN BINTI IDRUS | NURUL.FAZLINA.ZAFUAN@student.mmu.edu.my | 017-330 5877
// Member_3: 242UC244TP | SITI HAJAR BINTI MOHD ROZAIDDIN | SITI.HAJAR.MOHD@student.mmu.edu.my | 019-261 5560
// Member_4: 242UC241GC | SITI UMAIRAH BINTI MOHD ROZAIDDIN | SITI.UMAIRAH.MOHD@student.mmu.edu.my | 019-211 5560
// *********************************************************
// Task Distribution
// Member_1: Created the base code and command handling.
// Member_2: Created the INSERT INTO table function.
// Member_3: Created the CREATE TABLE function.
// Member_4: Created the data type validation feature and clean-up the codes.
// *********************************************************
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

// Function prototypes
bool has_substring(const string &line, const string &substring);
void create_output_screen_and_file(ofstream &fileOutput);
void create_table(ofstream &fileOutput, vector<vector<string>> &table, string &tableName, const string &command);
void insert_into_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command);
void select_all_from_table_in_csv_mode(ofstream &fileOutput, const vector<vector<string>> &table, const string &tableName);

int main() {
    ifstream fileInput;
    ofstream fileOutput;

    string fileInputName;
    string fileOutputName;

    fileInputName = "C:\\AssignmentGrp\\fileInput1.mdb";
//    fileInputName = "C:\\AssignmentGrp\\fileInput2.mdb";
//    fileInputName = "C:\\AssignmentGrp\\fileInput3.mdb";

    fileOutputName = "fileOutput1.txt";
//    fileOutputName = "fileOutput2.txt";
//    fileOutputName = "fileOutput3.txt";

    vector<vector<string>> table; // Represents the table
    string tableName; // Stores the name of the current table

    fileInput.open(fileInputName);
    if (!fileInput.is_open()) {
        cout << "Unable to open input file" << endl;
        return -1;
    }

    fileOutput.open(fileOutputName);
    if (!fileOutput.is_open()) {
        cout << "Unable to open output file" << endl;
        return -1;
    }

    string accumulatedCommand, line;
    while (getline(fileInput, line)) {
        if (!line.empty()) {
            accumulatedCommand += line;

            // Check if the command ends with a semicolon
            if (line.find(";") != string::npos) {
                // Process the complete command
                if (has_substring(accumulatedCommand, "CREATE TABLE")) {
                    create_table(fileOutput, table, tableName, accumulatedCommand);
                } else if (has_substring(accumulatedCommand, "INSERT INTO")) {
                    insert_into_table(fileOutput, table, accumulatedCommand);
                } else if (has_substring(accumulatedCommand, "SELECT *")) {
                    select_all_from_table_in_csv_mode(fileOutput, table, tableName);
                } else if (has_substring(accumulatedCommand, "CREATE")) {
                    fileOutput << "> CREATE " << fileOutputName << endl; //Made it easier to output the file name
                } else if (has_substring(accumulatedCommand, "DATABASES;")) {
                    fileOutput << "> " << accumulatedCommand << endl;
                    fileOutput << fileInputName << endl;
                } else if (has_substring(accumulatedCommand, "TABLES;")) {
                    fileOutput << "> " << accumulatedCommand << endl;
                    fileOutput << tableName << endl;
                } else {
                    fileOutput << "Error: Invalid input command - " << accumulatedCommand << endl;
                }
                accumulatedCommand.clear(); // Clear for next command
            }
        }
    }

    fileInput.close();
    fileOutput.close();
    return 0;
}

// Helper function to check for substrings
bool has_substring(const string &line, const string &substring) {
    return line.find(substring) != string::npos;
}

vector<string> columnHeaders; // Store column headers separately
vector<vector<string>> table; // Store table rows only
vector<string> globalColumnNames; // Global variable for column names


//separate column names and their data types
vector<pair<string, string>> columns;

// Function to handle CREATE TABLE command
void create_table(ofstream &fileOutput, vector<vector<string>> &table, string &tableName, const string &command) {
    fileOutput << "> " << command << endl;

    size_t tableStart = command.find("TABLE") + 6;
    size_t columnsStart = command.find("(");
    size_t columnsEnd = command.find(")");

    if (tableStart == string::npos || columnsStart == string::npos || columnsEnd == string::npos) {
        fileOutput << "Error: Invalid CREATE TABLE syntax" << endl;
        return;
    }

    // Extract table name
    tableName = command.substr(tableStart, columnsStart - tableStart);
    tableName.erase(remove(tableName.begin(), tableName.end(), ' '), tableName.end());

    // Extract and clean column definitions
    string columnsStr = command.substr(columnsStart + 1, columnsEnd - columnsStart - 1);
    stringstream ss(columnsStr);
    string column;
    columnHeaders.clear();
    columns.clear();

    while (getline(ss, column, ',')) {
        // Remove whitespace and data types
        column.erase(remove(column.begin(), column.end(), ' '), column.end());
        size_t typePos = column.find_first_of("INTTEXT");

        if (typePos != string::npos) {
            string columnName = column.substr(0, typePos);
            string columnType = column.substr(typePos);
            columns.emplace_back(columnName, columnType);
            columnHeaders.push_back(columnName);
        }
    }

    table.clear(); // Clear table data
    //fileOutput << "Table \"" << tableName << "\" created with columns: ";
//    for (size_t i= 0; i < columnHeaders.size(); ++i){
//        fileOutput << columns[i].first << " " << columns[i].second;
//        if (i < columns.size() -1) {
//            fileOutput << ", ";
//        }
//    }
    //fileOutput << "." << endl;
}

void insert_into_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command) {
    fileOutput << "> " << command << endl;

    size_t start = command.find("(");
    size_t end = command.find(")", start);
    if (start != string::npos && end != string::npos) {
        string columnNamesStr = command.substr(start + 1, end - start - 1);
        if (globalColumnNames.empty()) {
            stringstream ss(columnNamesStr);
            string column;
            while (getline(ss, column, ',')) {
                column.erase(remove(column.begin(), column.end(), ' '), column.end());
                globalColumnNames.push_back(column);
            }
        }
    }

    size_t valuesStart = command.find("VALUES") + 6;
    string valuesStr = command.substr(valuesStart);

    // Remove parentheses
    valuesStr.erase(remove(valuesStr.begin(), valuesStr.end(), '('), valuesStr.end());
    valuesStr.erase(remove(valuesStr.begin(), valuesStr.end(), ')'), valuesStr.end());

    // Remove trailing semicolon
    if (!valuesStr.empty() && valuesStr.back() == ';') {
        valuesStr.pop_back();
    }

    stringstream ss(valuesStr);
    string value;
    vector<string> row;
    size_t columnIndex = 0;
    bool isValid = true;

    while (getline(ss, value, ',')) {
        value.erase(remove(value.begin(), value.end(), ' '), value.end());

        string columnName = globalColumnNames[columnIndex];
        string columnType;

        for (const auto &col : columns) {
            if (col.first == columnName) {
                columnType = col.second;
                break;
            }
        }

        if (columnType == "INT") {
            try {
                stoi(value);
            } catch (invalid_argument &) {
                fileOutput << "Error: Column " << columnName << " expects INT but got '" << value << "'." << endl;
                isValid = false;
                break;
            }
        } else if (columnType == "TEXT") {
            // Ensure the value is enclosed in single quotes
            if (value.front() != '\'' || value.back() != '\'') {
                fileOutput << "Error: Column " << columnName << " expects TEXT enclosed in single quotes but got '" << value << "'." << endl;
                isValid = false;
                break;
            } else {
                // Remove the enclosing single quotes
                value = value.substr(1, value.size() - 2);
            }
        }

        row.push_back(value); // Add the cleaned value to the row
        columnIndex++;
    }

    if (isValid) {
        table.push_back(row); // Add the row to the table if all values are valid
    }
}

// Function to handle SELECT * FROM command
void select_all_from_table_in_csv_mode(ofstream &fileOutput, const vector<vector<string>> &table, const string &tableName) {
    fileOutput << "> SELECT * FROM " << tableName << ";" << endl;

    if (columnHeaders.empty()) {
        fileOutput << "Error: Table " << tableName << " does not exist." << endl;
        return;
    }

    // Print column headers
    for (size_t i = 0; i < columnHeaders.size(); ++i) {
        fileOutput << columnHeaders[i];
        if (i < columnHeaders.size() - 1) fileOutput << ",";
    }
    fileOutput << endl;

    // Print table data
    for (const auto &row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            fileOutput << row[i];
            if (i < row.size() - 1) fileOutput << ",";
        }
        fileOutput << endl;
    }
}
