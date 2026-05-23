#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <limits>

using namespace std;

const int MAX_RECORDS = 100;

class AirlineException : public exception
{
protected:
    string message;
public:
    AirlineException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidNameException : public AirlineException
{
public:
    InvalidNameException(const string& field = "Name")
        : AirlineException(field + " must contain only letters and spaces (no digits or special characters).") {
    }
};

class InvalidIDException : public AirlineException
{
public:
    InvalidIDException(const string& field = "ID")
        : AirlineException(field + " must be a positive integer.") {
    }
};

class InvalidPassportException : public AirlineException
{
public:
    InvalidPassportException()
        : AirlineException("Passport number must be alphanumeric (letters and digits only), 5-20 characters.") {
    }
};

class InvalidSeatsException : public AirlineException
{
public:
    InvalidSeatsException()
        : AirlineException("Number of seats must be a positive integer (greater than 0).") {
    }
};

class InvalidDepartureTimeException : public AirlineException
{
public:
    InvalidDepartureTimeException()
        : AirlineException("Departure time cannot be empty.") {
    }
};

class InvalidLocationException : public AirlineException
{
public:
    InvalidLocationException(const string& field = "Location")
        : AirlineException(field + " must contain only letters and spaces (no digits).") {
    }
};

class DuplicateIDException : public AirlineException
{
public:
    DuplicateIDException(const string& entity = "Record")
        : AirlineException(entity + " ID already exists in the system.") {
    }
};

class StorageFullException : public AirlineException
{
public:
    StorageFullException(const string& entity = "Storage")
        : AirlineException(entity + " is full. Cannot add more records.") {
    }
};

class RecordNotFoundException : public AirlineException
{
public:
    RecordNotFoundException(const string& entity = "Record")
        : AirlineException(entity + " not found in the system.") {
    }
};

class NoSeatsAvailableException : public AirlineException
{
public:
    NoSeatsAvailableException()
        : AirlineException("No seats available on this flight.") {
    }
};

class FlightInAirException : public AirlineException
{
public:
    FlightInAirException()
        : AirlineException("Cannot book a flight that is currently in the air.") {
    }
};

class NoRecordsException : public AirlineException
{
public:
    NoRecordsException(const string& entity = "Records")
        : AirlineException("No " + entity + " found in the system.") {
    }
};

class FileException : public AirlineException
{
public:
    FileException(const string& filename)
        : AirlineException("Failed to open file: " + filename) {
    }
};

class InvalidMenuChoiceException : public AirlineException
{
public:
    InvalidMenuChoiceException()
        : AirlineException("Invalid menu choice. Please enter a valid option number.") {
    }
};

class InvalidInputTypeException : public AirlineException
{
public:
    InvalidInputTypeException(const string& field = "Input")
        : AirlineException(field + ": expected a number but received non-numeric input.") {
    }
};

// ============================================================
//  INPUT VALIDATION HELPERS
// ============================================================

// Reads a single integer from cin; throws if input is non-numeric
int readInt(const string& fieldName = "Input")
{
    int value;
    if (!(cin >> value))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw InvalidInputTypeException(fieldName);
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Validates that a string contains only letters and spaces
void validateName(const string& name, const string& fieldName = "Name")
{
    if (name.empty())
        throw InvalidNameException(fieldName);

    for (char c : name)
        if (!isalpha(c) && c != ' ')
            throw InvalidNameException(fieldName);
}

// Validates that an ID is a positive integer
void validateID(int id, const string& fieldName = "ID")
{
    if (id <= 0)
        throw InvalidIDException(fieldName);
}

// Validates passport: alphanumeric only, 5-20 characters
void validatePassport(const string& passport)
{
    if (passport.size() < 5 || passport.size() > 20)
        throw InvalidPassportException();

    for (char c : passport)
        if (!isalnum(c))
