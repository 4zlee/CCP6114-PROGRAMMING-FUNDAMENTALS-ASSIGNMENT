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
void delete_from_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command); // Added delete function prototype
void update_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command);
void count_row(vector<vector<string>> &table, ofstream &fileOutput, const string &command);

int main() {
    ifstream fileInput;
    ofstream fileOutput;

    string fileInputName;
    string fileOutputName;

    fileInputName = "C:\\AssignmentGrp\\fileInput1.mdb";
    fileOutputName = "fileOutput1.txt";

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
                } else if (has_substring(accumulatedCommand, "DELETE FROM")) { // Handle DELETE command
                    delete_from_table(fileOutput, table, accumulatedCommand);
                    } else if (has_substring(accumulatedCommand, "UPDATE")) {
                    update_table(fileOutput, table, accumulatedCommand);
                } else if (has_substring(accumulatedCommand, "CREATE")) {
                    fileOutput << "> CREATE " << fileOutputName << endl;
                } else if (has_substring(accumulatedCommand, "DATABASES;")) {
                    fileOutput << "> " << accumulatedCommand << endl;
                    fileOutput << fileInputName << endl;
                } else if (has_substring(accumulatedCommand, "TABLES;")) {
                    fileOutput << "> " << accumulatedCommand << endl;
                    fileOutput << tableName << endl;
                }else if (has_substring(accumulatedCommand, "SELECT COUNT(*)FROM")) {
                    count_row(table, fileOutput, accumulatedCommand);
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
}

// Function to handle INSERT INTO command
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

// Function to handle DELETE FROM command (added function)
void delete_from_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command) {
    fileOutput << "> " << command << endl;

    // Extract table name from the command
    size_t tableStart = command.find("FROM") + 5;
    size_t wherePos = command.find("WHERE");
    if (tableStart == string::npos || wherePos == string::npos) {
        fileOutput << "Error: Invalid DELETE syntax" << endl;
        return;
    }

    // Get the table name
    string tableName = command.substr(tableStart, wherePos - tableStart);
    tableName.erase(remove(tableName.begin(), tableName.end(), ' '), tableName.end());

    // Extract condition (where clause)
    string condition = command.substr(wherePos + 6); // Skipping "WHERE"
    condition.erase(remove(condition.begin(), condition.end(), ' '), condition.end());

    // Assume the condition is simple: columnName = value
    size_t equalPos = condition.find("=");
    if (equalPos == string::npos) {
        fileOutput << "Error: Invalid condition in DELETE command" << endl;
        return;
    }

    string columnName = condition.substr(0, equalPos);
    string value = condition.substr(equalPos + 1);
    columnName.erase(remove(columnName.begin(), columnName.end(), ' '), columnName.end());
    value.erase(remove(value.begin(), value.end(), ' '), value.end());

    // Check if value is empty
    if (value.empty()) {
        fileOutput << "Error: No value provided for deletion condition." << endl;
        return;
    }

    // Find the column index in the table
    int columnIndex = -1;
    for (size_t i = 0; i < columnHeaders.size(); ++i) {
        if (columnHeaders[i] == columnName) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex == -1) {
        fileOutput << "Error: Column " << columnName << " does not exist in table " << tableName << endl;
        return;
    }

    // Convert value to integer for comparison if column type is INT
    bool isIntColumn = false;
    for (const auto &col : columns) {
        if (col.first == columnName && col.second == "INT") {
            isIntColumn = true;
            break;
        }
    }

    bool found = false;

    if (isIntColumn) {
        try {
            // Convert string value to integer
            int intValue = stoi(value);

            // Check if the value exists in the table before deleting
            for (const auto &row : table) {
                if (stoi(row[columnIndex]) == intValue) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                fileOutput << "Error: No matching record found for ID " << intValue << "." << endl;
                return;
            }

            // Delete rows where the column value matches
            table.erase(remove_if(table.begin(), table.end(),
                                  [columnIndex, intValue](const vector<string> &row) {
                                      return stoi(row[columnIndex]) == intValue;
                                  }),
                        table.end());
        } catch (invalid_argument &) {
            fileOutput << "Error: Invalid INT value '" << value << "'." << endl;
            return;
        }
    } else {
        // For TEXT columns, check if the value matches the exact string (without quotes)
        if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.size() - 2); // Remove single quotes
        }

        // Check if the value exists in the table before deleting
        for (const auto &row : table) {
            if (row[columnIndex] == value) {
                found = true;
                break;
            }
        }

        if (!found) {
            fileOutput << "Error: No matching record found for value '" << value << "'." << endl;
            return;
        }

        // Delete rows where the column value matches
        table.erase(remove_if(table.begin(), table.end(),
                              [columnIndex, &value](const vector<string> &row) {
                                  return row[columnIndex] == value;
                              }),
                    table.end());
    }
}

void count_row(vector<vector<string>> &table, ofstream &fileOutput, const string &command){

fileOutput << "> " << command << endl;
fileOutput << table.size();
}

void update_table(ofstream &fileOutput, vector<vector<string>> &table, const string &command) {
    fileOutput << "> " << command << endl;

    // Parse the command
    size_t setStart = command.find("SET") + 4;
    size_t whereStart = command.find("WHERE") + 6;

    if (setStart == string::npos || whereStart == string::npos) {
        fileOutput << "Error: Invalid UPDATE syntax" << endl;
        return;
    }

    // Extract SET and WHERE clauses
    string setClause = command.substr(setStart, whereStart - setStart - 6);
    string whereClause = command.substr(whereStart);

    // Parse SET clause
    size_t equalsPosSet = setClause.find("=");
    if (equalsPosSet == string::npos) {
        fileOutput << "Error: Invalid SET condition" << endl;
        return;
    }
    string setColumn = setClause.substr(0, equalsPosSet);
    string setValue = setClause.substr(equalsPosSet + 1);
    setColumn.erase(remove(setColumn.begin(), setColumn.end(), ' '), setColumn.end());
    setValue.erase(remove(setValue.begin(), setValue.end(), ' '), setValue.end());

    if (setValue.front() == '\'' && setValue.back() == '\'') {
        setValue = setValue.substr(1, setValue.size() - 2); // Remove single quotes
    }

    // Parse WHERE clause
    size_t equalsPosWhere = whereClause.find("=");
    if (equalsPosWhere == string::npos) {
        fileOutput << "Error: Invalid WHERE condition" << endl;
        return;
    }
    string whereColumn = whereClause.substr(0, equalsPosWhere);
    string whereValue = whereClause.substr(equalsPosWhere + 1);
    whereColumn.erase(remove(whereColumn.begin(), whereColumn.end(), ' '), whereColumn.end());
    whereValue.erase(remove(whereValue.begin(), whereValue.end(), ' '), whereValue.end());

    // Find column indices for SET and WHERE clauses
    int setColIndex = -1, whereColIndex = -1;
    for (size_t i = 0; i < columnHeaders.size(); ++i) {
        if (columnHeaders[i] == setColumn) setColIndex = i;
        if (columnHeaders[i] == whereColumn) whereColIndex = i;
    }

    if (setColIndex == -1 || whereColIndex == -1) {
        fileOutput << "Error: Invalid column name in SET or WHERE clause" << endl;
        return;
    }

    // Check column type for WHERE clause
    bool isIntColumn = false;
    for (const auto &col : columns) {
        if (col.first == whereColumn && col.second == "INT") {
            isIntColumn = true;
            break;
        }
    }

    // Convert whereValue to INT if necessary
    int intWhereValue = 0;
    if (isIntColumn) {
        try {
            intWhereValue = stoi(whereValue);
        } catch (invalid_argument &) {
            fileOutput << "Error: Invalid INT value in WHERE condition" << endl;
            return;
        }
    }

    // Update rows
    bool found = false;
    for (auto &row : table) {
        bool match = (isIntColumn && stoi(row[whereColIndex]) == intWhereValue) ||
                     (!isIntColumn && row[whereColIndex] == whereValue);

        if (match) {
            fileOutput << "Before update: " << columnHeaders[setColIndex] << "='" << row[setColIndex] << "'" << endl;
            row[setColIndex] = setValue;
            fileOutput << "After update: " << columnHeaders[setColIndex] << "='" << row[setColIndex] << "'" << endl;
            found = true;
        }
    }

    if (!found) {
        fileOutput << "Error: No matching row found for WHERE " << whereColumn << "=" << whereValue << endl;
    }
}
