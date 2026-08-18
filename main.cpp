// Include necessary headers
#include <iostream>  // std::cout, std::cin
#include <cstdlib>   // std::abs
#include <fstream>   // std::ifstream, std::ofstream
#include <vector>    // std::vector
#include <string>    // std::string
#include <sstream>   // std::stringstream
#include <algorithm> // std::swap
#include "Fraction.h"
#include "Matrix.h"


std::vector<std::string> NumDimensionsChecker() {
    std::string entry;
    std::vector<std::string> dimensions;
    while (true) {
        try {
            std::cin >> entry;
            while (entry != "$") {
                dimensions.push_back(entry);
                std::cin >> entry;
            }
            if (dimensions.size() != 2) {
                throw std::runtime_error("You did not enter 2 dimensions for the matrix.");
            }
            // If dimensions are correct, break out of the loop
            break;

        }
        catch (const std::exception& e) {
            std::cout << e.what() << " Please enter again, followed by a \"$\"." << std::endl;
            dimensions.clear();  // Clear invalid dimensions and retry
        }
    }
    return dimensions;
}


bool DimensionChecker(const std::string& userString) {
    for (char ch : userString) {
        if (!isdigit(ch)) {
            throw std::invalid_argument("Dimension should contain only positive integers.");
        }
    }
    return true;
}

std::vector<std::string> NumEntriesChecker(int numEntries) {
    std::string entry;
    std::vector<std::string> entries;
    while (true) {
        try {
            entries.clear();
            int counter = 0;
            std::cin >> entry;
            while (entry != "$") {
                counter++;
                entries.push_back(entry);
                std::cin >> entry;
            }
            if (counter != numEntries) {
                throw std::invalid_argument("Incorrect number of entries.");
            }
            // If valid, break out of loop
            break;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << " Please enter the correct number of entries, ending with '$'." << std::endl;
            entries.clear();  // Clear invalid input for retry
        }
    }
    return entries;
}


bool MatrixEntriesChecker(const std::vector<std::string>& userEntries) {
    int divisorBarCounter = 0; // Counter for '/' characters
    for (int i = 0; i < userEntries.size(); i++) {
        const std::string& entry = userEntries.at(i);
        // Check each character in the entry
        for (int j = 0; j < entry.length(); j++) {
            char currentChar = entry.at(j);
            // If the character is not a digit, we handle different cases
            if (!isdigit(currentChar)) {
                if (currentChar == '-') {
                    // Handle negative numbers: '-' should be at the start and must not be the only character
                    if ((j != 0) || (j == 0 && entry.length() == 1) || (j == 0 && !isdigit(entry.at(1)))) {
                        throw std::invalid_argument("Invalid negative number formatting.");
                    }
                }
                else if (currentChar == '/') {
                    // Handle fractions
                    divisorBarCounter++;
                    if (j == 0 || j == entry.length() - 1) {
                        throw std::invalid_argument("Invalid fraction formatting.");
                    }
                }
                else {
                    throw std::invalid_argument("Invalid character in matrix entry.");
                }
            }
        }
        // Only one '/' character allowed
        if (divisorBarCounter > 1) {
            throw std::invalid_argument("Too many '/' characters in a fraction.");
        }
        // Reset the divisor bar counter for the next entry
        divisorBarCounter = 0;
    }
    return true;
}

// Function to convert strings to Fraction objects
std::vector<Fraction> StringSplitter(std::vector<std::string>& entries) {
    char delim = '/';
    long long numerator;
    long long denominator;
    std::vector<Fraction> Fractions;
    // Iterate through each entry
    for (int i = 0; i < entries.size(); i++) {
        bool isFraction = false;
        // Check if the entry is a fraction
        for (int j = 0; j < entries.at(i).length(); j++) {
            if (entries.at(i).at(j) == '/') {
                isFraction = true;
                break;
            }
        }
        // If entry is a fraction, split it into numerator and denominator
        if (isFraction) {
            int counter = 0;
            std::stringstream ss(entries.at(i));
            std::string item;
            while (std::getline(ss, item, delim)) {
                counter += 1;
                if (counter == 1) {
                    numerator = std::stoll(item);
                }
                else {
                    denominator = std::stoll(item);
                }
            }
            Fractions.push_back(Fraction(numerator, denominator));
        }
        // If entry is not a fraction, convert it directly to a Fraction object
        else {
            Fractions.push_back(Fraction(std::stoll(entries.at(i))));
        }
    }
    return Fractions;
}

Matrix MatrixCreator() {
    std::string userRows, userCols;
    int numRows = 0, numCols = 0;
    // Validate and get matrix dimensions
    while (true) {
        try {
            std::vector<std::string> dimensions = NumDimensionsChecker();
            userRows = dimensions.at(0);
            userCols = dimensions.at(1);
            // Validate the dimensions, throws if invalid
            DimensionChecker(userRows);
            DimensionChecker(userCols);
            // Convert to integers
            numRows = std::stoi(userRows);
            numCols = std::stoi(userCols);
            break;  // Valid dimensions, exit loop
        }
        catch (const std::exception& e) {
            std::cout << e.what() << " Please enter valid dimensions followed by '$'." << std::endl;
        }
    }
    int numEntries = numRows * numCols;
    std::vector<std::string> entries;
    // Validate matrix entries
    while (true) {
        try {
            std::cout << "Enter the entries of the matrix, ending the list with '$'." << std::endl;
            entries = NumEntriesChecker(numEntries);
            MatrixEntriesChecker(entries);  // Validate the entries
            break;  // If valid, exit loop
        }
        catch (const std::exception& e) {
            std::cout << e.what() << " Please enter valid matrix entries followed by '$'." << std::endl;
        }
    }
    std::vector<Fraction> fractions = StringSplitter(entries);
    Matrix myMatrix(numRows, numCols);
    // Set the matrix grid with the fractions
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            myMatrix.SetGrid(i, j, fractions.at(i * numCols + j));
        }
    }
    return myMatrix;
}

Matrix MatrixMultiplier(Matrix matrix1, Matrix matrix2) {
    try {
        while (matrix1.GetColDim() != matrix2.GetRowDim()) {
            throw std::runtime_error("You cannot multiply these matrices together because the product is undefined.");
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        std::cout << "For the product to be defined, the row dimension of the second matrix must be ";
        std::cout << "the same as the column dimension of the first." << std::endl;
        // Re-enter the matrix dimensions in a loop until valid matrices are entered
        while (true) {
            try {
                std::cout << "Enter dimensions of the first matrix. Afterwards, enter a \"$\"." << std::endl;
                matrix1 = MatrixCreator();  // Create the first matrix

                std::cout << "Now, enter the dimensions of the second matrix. ";
                std::cout << "Afterwards, enter a \"$\"." << std::endl;
                matrix2 = MatrixCreator();  // Create the second matrix

                if (matrix1.GetColDim() != matrix2.GetRowDim()) {
                    throw std::invalid_argument("The product is undefined with these dimensions. Please re-enter.");
                }
                break;  // Exit loop if matrices are valid
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << std::endl;
            }
        }
    }

    int sameNum = matrix1.GetColDim();
    int rowNum = matrix1.GetRowDim();
    int colNum = matrix2.GetColDim();
    Matrix product(rowNum, colNum); // Construct a Matrix object that is to be the product
    Fraction Sum(0, 1); // This Sum (Fraction object) is to be used to find the sum of each row and column products

    // This for loop is the formula for multiplying two matrices together
    // It finds the correct sum for each row and column and enters the Sum fraction as the entry of the Product object's grid
    for (int i = 0; i < rowNum; i++) {
        for (int j = 0; j < colNum; j++) {
            for (int k = 0; k < sameNum; k++) {
                Sum = Sum + matrix1.GetGrid()[i][k] * matrix2.GetGrid()[k][j];
            }
            product.SetGrid(i, j, Sum);
            Sum.SetNumerator(0); // Sum must be reset
            Sum.SetDenominator(1);
        }
    }
    return product;
}

int main() {
    std::cout << "Enter dimensions of the first matrix. For example, a 4x7 matrix would be  \"4 7\". ";
    std::cout << "Afterwards, enter a \"$\"." << std::endl;
    Matrix matrix1 = MatrixCreator();
    std::cout << "Now, enter the dimensions of the second matrix. " << std::endl;
    std::cout << "Afterwards, enter a \"$\"." << std::endl;
    Matrix matrix2 = MatrixCreator();
    Matrix product = MatrixMultiplier(matrix1, matrix2);
    product.Print(); // Prints the product
    return 0;
}