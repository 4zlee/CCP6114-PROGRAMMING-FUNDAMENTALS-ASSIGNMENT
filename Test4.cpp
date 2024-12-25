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

    fileInputName = "C:\\AssignmentGrp\\fileInput1test.mdb";
    //fileInputName = "C:\\AssignmentGrp\\fileInput1.mdb";

    fileOutputName = "fileOutput1test.txt";
    //fileOutputName = "fileOutput1.txt";

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
                    fileOutput << "> " << accumulatedCommand << endl;
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
    fileOutput << "Table \"" << tableName << "\" created with columns: ";
    for (size_t i= 0; i < columnHeaders.size(); ++i){
        fileOutput << columns[i].first << " " << columns[i].second;
        if (i < columns.size() -1) {
            fileOutput << ", ";
        }
    }
    fileOutput << "." << endl;
}

void insert_into_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command) {
    fileOutput << "> " << command << endl;

    // Extract column names from INSERT INTO
    size_t start = command.find("(");
    size_t end = command.find(")", start);
    if (start != string::npos && end != string::npos) {
        string columnNamesStr = command.substr(start + 1, end - start - 1);
        if (globalColumnNames.empty()) { // Update only if not set
            stringstream ss(columnNamesStr);
            string column;
            while (getline(ss, column, ',')) {
                column.erase(remove(column.begin(), column.end(), ' '), column.end());
                globalColumnNames.push_back(column);
            }
        }
    }

    // Extract values
    size_t valuesStart = command.find("VALUES") + 6;
    string valuesStr = command.substr(valuesStart);
    valuesStr.erase(remove(valuesStr.begin(), valuesStr.end(), '('), valuesStr.end());
    valuesStr.erase(remove(valuesStr.begin(), valuesStr.end(), ')'), valuesStr.end());

    stringstream ss(valuesStr);
    string value;
    vector<string> row;
    size_t columnIndex = 0;
    bool isValid = true;

    while (getline(ss, value, ',')) {
        value.erase(remove(value.begin(), value.end(), '\''), value.end());

        // Validate data type based on the column name
        string columnName = globalColumnNames[columnIndex];
        string columnType;

        // Find the column's data type
        for (const auto &col : columns) {
            if (col.first == columnName) {
                columnType = col.second;
                break;
            }
        }

        // Perform type checking
        if (columnType == "INT") {
            try {
                stoi(value); // Try to convert to integer
            } catch (invalid_argument &) {
                fileOutput << "Error: Column " << columnName << " expects INT but got '" << value << "'." << endl;
                isValid = false;
                break; // Stop further validation
            }
        } else if (columnType == "TEXT") {
            // No validation needed for TEXT
        }

        row.push_back(value);
        columnIndex++;
    }

    // Only add the row if all values are valid
    if (isValid) {
        table.push_back(row);
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
