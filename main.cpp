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
            throw InvalidPassportException();
}

// Validates that seat count is positive
void validateSeats(int seats)
{
    if (seats <= 0)
        throw InvalidSeatsException();
}

// Validates that a location string has no digits
void validateLocation(const string& loc, const string& fieldName = "Location")
{
    if (loc.empty())
        throw InvalidLocationException(fieldName);

    for (char c : loc)
        if (isdigit(c))
            throw InvalidLocationException(fieldName);
}

// Validates that a departure time string is not empty
void validateDepartureTime(const string& dt)
{
    if (dt.empty())
        throw InvalidDepartureTimeException();
}

// ============================================================
//  DATA CLASSES
// ============================================================

class Passenger
{
public:
    int    id;
    string name;
    string passport;

    Passenger() : id(0), name(""), passport("") {}

    // Prompts the user to fill in all passenger fields with validation
    void input()
    {
        while (true)
        {
            try
            {
                cout << "Enter Passenger ID   : ";
                id = readInt("Passenger ID");
                validateID(id, "Passenger ID");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Enter Name           : ";
                getline(cin, name);
                validateName(name, "Passenger Name");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Enter Passport Number: ";
                getline(cin, passport);
                validatePassport(passport);
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }
    }

    void display() const
    {
        cout << "ID: " << id
            << " | Name: " << name
            << " | Passport: " << passport
            << endl;
    }
};

// -------------------------------------------------------

class Flight
{
public:
    int    flightNo;
    string source;
    string destination;
    string departureTime;
    int    seats;
    string status;

    Flight() : flightNo(0), seats(0), status("Scheduled") {}

    // Prompts the user to fill in all flight fields with validation
    void input()
    {
        while (true)
        {
            try
            {
                cout << "Enter Flight No     : ";
                flightNo = readInt("Flight No");
                validateID(flightNo, "Flight No");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Source              : ";
                getline(cin, source);
                validateLocation(source, "Source");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Destination         : ";
                getline(cin, destination);
                validateLocation(destination, "Destination");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Departure Time      : ";
                getline(cin, departureTime);
                validateDepartureTime(departureTime);
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Available Seats     : ";
                seats = readInt("Available Seats");
                validateSeats(seats);
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        status = "Scheduled";
    }

    void display() const
    {
        cout << "Flight No: " << flightNo
            << " | " << source << " -> " << destination
            << " | Departure: " << departureTime
            << " | Seats: " << seats
            << " | Status: " << status
            << endl;
    }
};

// -------------------------------------------------------

class Crew
{
public:
    int    id;
    string name;
    string role;

    Crew() : id(0), name(""), role("") {}

    // Prompts the user to fill in all crew fields with validation
    void input()
    {
                while (true)
        {
            try
            {
                cout << "Enter Crew ID : ";
                id = readInt("Crew ID");
                validateID(id, "Crew ID");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Name          : ";
                getline(cin, name);
                validateName(name, "Crew Name");
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }

        while (true)
        {
            try
            {
                cout << "Role          : ";
                getline(cin, role);
                validateName(role, "Crew Role");  // role uses same rule: letters and spaces only
                break;
            }
            catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
        }
    }

    void display() const
    {
        cout << "ID: " << id
            << " | Name: " << name
            << " | Role: " << role
            << endl;
    }
};

// -------------------------------------------------------

class Booking
{
public:
    int       bookingID;
    Passenger p;
    Flight    f;

    Booking() : bookingID(0) {}

    void display() const
    {
        cout << "\nBooking ID: " << bookingID << "\n";
        cout << "Passenger -> "; p.display();
        cout << "Flight    -> "; f.display();
    }
};

// ============================================================
//  FILE HANDLER
//  Responsible for all read/write operations on data files
// ============================================================

class FileHandler
{
public:
    // ---------- Append single records ----------

    void savePassenger(const Passenger& p)
    {
        ofstream file("passengers.txt", ios::app);
        if (!file.is_open()) throw FileException("passengers.txt");
        file << p.id << "|" << p.name << "|" << p.passport << "\n";
    }

    void saveFlight(const Flight& f)
    {
        ofstream file("flights.txt", ios::app);
        if (!file.is_open()) throw FileException("flights.txt");
        file << f.flightNo << "|" << f.source << "|" << f.destination << "|"
            << f.departureTime << "|" << f.seats << "|" << f.status << "\n";
    }
