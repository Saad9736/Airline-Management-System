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

        void saveCrew(const Crew& c)
    {
        ofstream file("crew.txt", ios::app);
        if (!file.is_open()) throw FileException("crew.txt");
        file << c.id << "|" << c.name << "|" << c.role << "\n";
    }

    void saveBooking(const Booking& b)
    {
        ofstream file("bookings.txt", ios::app);
        if (!file.is_open()) throw FileException("bookings.txt");
        file << b.bookingID << "|" << b.p.id << "|" << b.f.flightNo << "\n";
    }

    // ---------- Overwrite entire file (used after deletions) ----------

    void rewritePassengersFile(const Passenger passengers[], int count)
    {
        ofstream file("passengers.txt");
        if (!file.is_open()) throw FileException("passengers.txt");
        for (int i = 0; i < count; i++)
            file << passengers[i].id << "|" << passengers[i].name << "|" << passengers[i].passport << "\n";
    }

    void rewriteFlightsFile(const Flight flights[], int count)
    {
        ofstream file("flights.txt");
        if (!file.is_open()) throw FileException("flights.txt");
        for (int i = 0; i < count; i++)
            file << flights[i].flightNo << "|" << flights[i].source << "|" << flights[i].destination << "|"
            << flights[i].departureTime << "|" << flights[i].seats << "|" << flights[i].status << "\n";
    }

    void rewriteCrewFile(const Crew crewList[], int count)
    {
        ofstream file("crew.txt");
        if (!file.is_open()) throw FileException("crew.txt");
        for (int i = 0; i < count; i++)
            file << crewList[i].id << "|" << crewList[i].name << "|" << crewList[i].role << "\n";
    }

    void rewriteBookingsFile(const Booking bookings[], int count)
    {
        ofstream file("bookings.txt");
        if (!file.is_open()) throw FileException("bookings.txt");
        for (int i = 0; i < count; i++)
            file << bookings[i].bookingID << "|" << bookings[i].p.id << "|" << bookings[i].f.flightNo << "\n";
    }
};

// ============================================================
//  PASSENGER MANAGER
// ============================================================

class PassengerManager
{
public:
    Passenger   passengers[MAX_RECORDS];
    int         count;
    FileHandler fh;

    PassengerManager() : count(0) { loadPassengersFromFile(); }

    // Loads all passenger records from disk into memory on startup
    void loadPassengersFromFile()
    {
        ifstream file("passengers.txt");
        if (!file.is_open()) return;  // file may not exist yet — that's fine

        string line;
        while (getline(file, line) && count < MAX_RECORDS)
        {
            try
            {
                stringstream ss(line);
                Passenger p;
                string temp;

                getline(ss, temp, '|');
                if (temp.empty()) continue;

                p.id = stoi(temp);
                getline(ss, p.name, '|');
                getline(ss, p.passport);

                passengers[count++] = p;
            }
            catch (const invalid_argument&)
            {
                cout << "[Load Warning] Skipping corrupt passenger record: " << line << "\n";
            }
            catch (const out_of_range&)
            {
                cout << "[Load Warning] ID out of range in passenger record: " << line << "\n";
            }
        }
    }

    // Returns the array index of a passenger by ID, or -1 if not found
    int findPassengerIndex(int id) const
    {
        for (int i = 0; i < count; i++)
            if (passengers[i].id == id) return i;
        return -1;
    }

    // Adds a passenger; returns true on success, false on duplicate/full
    bool addPassenger(const Passenger& p)
    {
        try
        {
            if (count >= MAX_RECORDS)
                throw StorageFullException("Passenger storage");

            if (findPassengerIndex(p.id) != -1)
                throw DuplicateIDException("Passenger");

            passengers[count++] = p;
            fh.savePassenger(p);
            return true;
        }
        catch (const AirlineException& e)
        {
            cout << "[Error] " << e.what() << "\n";
            return false;
        }
    }

    void removePassenger()
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("passengers");

            int id;
            while (true)
            {
                try
                {
                                        cout << "Enter Passenger ID to remove: ";
                    id = readInt("Passenger ID");
                    validateID(id, "Passenger ID");
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int index = findPassengerIndex(id);
            if (index == -1)
                throw RecordNotFoundException("Passenger");

            // Shift remaining records left to fill the gap
            for (int j = index; j < count - 1; j++)
                passengers[j] = passengers[j + 1];

            count--;
            fh.rewritePassengersFile(passengers, count);
            cout << "Passenger removed successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void viewPassengers() const
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("passengers");

            cout << "\n--- Passenger List ---\n";
            for (int i = 0; i < count; i++)
                passengers[i].display();
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }
};

// ============================================================
//  FLIGHT MANAGER
// ============================================================

class FlightManager
{
public:
    Flight      flights[MAX_RECORDS];
    int         count;
    FileHandler fh;

    FlightManager() : count(0) { loadFlightsFromFile(); }

    // Loads all flight records from disk into memory on startup
    void loadFlightsFromFile()
    {
        ifstream file("flights.txt");
        if (!file.is_open()) return;

        string line;
        while (getline(file, line) && count < MAX_RECORDS)
        {
            try
            {
                stringstream ss(line);
                Flight f;
                string temp;

                getline(ss, temp, '|');
                if (temp.empty()) continue;

                f.flightNo = stoi(temp);
                getline(ss, f.source, '|');
                getline(ss, f.destination, '|');
                getline(ss, f.departureTime, '|');

                getline(ss, temp, '|');
                f.seats = stoi(temp);

                getline(ss, f.status);
                flights[count++] = f;
            }
            catch (const invalid_argument&)
            {
                cout << "[Load Warning] Skipping corrupt flight record: " << line << "\n";
            }
            catch (const out_of_range&)
            {
                cout << "[Load Warning] Value out of range in flight record: " << line << "\n";
            }
        }
    }

    // Returns the array index of a flight by flight number, or -1 if not found
    int findFlightIndex(int flightNo) const
    {
        for (int i = 0; i < count; i++)
            if (flights[i].flightNo == flightNo) return i;
        return -1;
    }

    // Convenience: rewrites the entire flights file (e.g. after any change)
    void rewriteFlightsFile()
    {
        fh.rewriteFlightsFile(flights, count);
    }

    void addFlight()
    {
        try
        {
            if (count >= MAX_RECORDS)
                throw StorageFullException("Flight storage");

            Flight temp;
            temp.input();

            if (findFlightIndex(temp.flightNo) != -1)
                throw DuplicateIDException("Flight");

            flights[count++] = temp;
            fh.saveFlight(temp);
            cout << "Flight added successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void removeFlight()
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("flights");

            int no;
            while (true)
            {
                try
                {
                    cout << "Enter Flight Number to remove: ";
                    no = readInt("Flight Number");
                    validateID(no, "Flight Number");
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int index = findFlightIndex(no);
            if (index == -1)
                throw RecordNotFoundException("Flight");

            for (int j = index; j < count - 1; j++)
                flights[j] = flights[j + 1];

            count--;
            rewriteFlightsFile();
            cout << "Flight removed successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void viewFlights() const
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("flights");

            cout << "\n--- Flight List ---\n";
            for (int i = 0; i < count; i++)
                flights[i].display();
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void updateFlightStatus()
    {
        try
        {
            int no;
            while (true)
            {
                try
                {
                    cout << "Enter Flight Number: ";
                    no = readInt("Flight Number");
                    validateID(no, "Flight Number");
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int index = findFlightIndex(no);
            if (index == -1)
                throw RecordNotFoundException("Flight");

            cout << "1. Scheduled\n"
                << "2. Taking Off\n"
                << "3. In Air\n"
                << "4. Landing\n"
                << "5. Landed\n";

            int ch;
            while (true)
            {
                try
                {
                    cout << "Choice: ";
                    ch = readInt("Status choice");
                    if (ch < 1 || ch > 5) throw InvalidMenuChoiceException();
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            const string statusLabels[] = { "", "Scheduled", "Taking Off", "In Air", "Landing", "Landed" };
            flights[index].status = statusLabels[ch];

            rewriteFlightsFile();
            cout << "Status updated successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

    void viewFlightsInAir() const
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("flights");

            cout << "\n--- Flights Currently In Air ---\n";
            bool found = false;
            for (int i = 0; i < count; i++)
            {
                if (flights[i].status == "In Air")
                {
                    flights[i].display();
                    found = true;
                }
            }

            if (!found)
                cout << "No flights are currently in the air.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }
};

// ============================================================
//  CREW MANAGER
// ============================================================

class CrewManager
{
public:
    Crew        crewList[MAX_RECORDS];
    int         count;
    FileHandler fh;

    CrewManager() : count(0) { loadCrewFromFile(); }

    // Loads all crew records from disk into memory on startup
    void loadCrewFromFile()
    {
        ifstream file("crew.txt");
        if (!file.is_open()) return;

        string line;
        while (getline(file, line) && count < MAX_RECORDS)
        {
            try
            {
                stringstream ss(line);
                Crew c;
                string temp;

                getline(ss, temp, '|');
                if (temp.empty()) continue;

                c.id = stoi(temp);
                getline(ss, c.name, '|');
                getline(ss, c.role);

                crewList[count++] = c;
            }
            catch (const invalid_argument&)
            {
                cout << "[Load Warning] Skipping corrupt crew record: " << line << "\n";
            }
            catch (const out_of_range&)
            {
                cout << "[Load Warning] ID out of range in crew record: " << line << "\n";
            }
        }
    }

    // Returns the array index of a crew member by ID, or -1 if not found
    int findCrewIndex(int id) const
    {
        for (int i = 0; i < count; i++)
            if (crewList[i].id == id) return i;
        return -1;
    }

    void addCrew()
    {
        try
        {
            if (count >= MAX_RECORDS)
                throw StorageFullException("Crew storage");

            Crew temp;
            temp.input();

            if (findCrewIndex(temp.id) != -1)
                throw DuplicateIDException("Crew");

            crewList[count++] = temp;
            fh.saveCrew(temp);
            cout << "Crew member added successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void removeCrew()
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("crew members");

            int id;
            while (true)
            {
                try
                {
                    cout << "Enter Crew ID to remove: ";
                    id = readInt("Crew ID");
                    validateID(id, "Crew ID");
                    break;
                }
                                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int index = findCrewIndex(id);
            if (index == -1)
                throw RecordNotFoundException("Crew member");

            for (int j = index; j < count - 1; j++)
                crewList[j] = crewList[j + 1];

            count--;
            fh.rewriteCrewFile(crewList, count);
            cout << "Crew member removed successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void viewCrew() const
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("crew members");

            cout << "\n--- Crew List ---\n";
            for (int i = 0; i < count; i++)
                crewList[i].display();
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }
};

// ============================================================
//  BOOKING MANAGER
// ============================================================

class BookingManager
{
public:
    Booking     bookings[MAX_RECORDS];
    int         count;
    int         nextBookingID;
    FileHandler fh;

    BookingManager() : count(0), nextBookingID(1) {}

    // Loads bookings from disk; needs passenger and flight managers to resolve references
    void loadBookingsFromFile(const PassengerManager& pm, const FlightManager& fm)
    {
        ifstream file("bookings.txt");
        if (!file.is_open()) return;

        string line;
        int maxID = 0;

        while (getline(file, line) && count < MAX_RECORDS)
        {
            try
            {
                stringstream ss(line);
                Booking b;
                string temp;
                int passengerID, flightNo;

                getline(ss, temp, '|');
                if (temp.empty()) continue;
                b.bookingID = stoi(temp);

                getline(ss, temp, '|');
                passengerID = stoi(temp);

                getline(ss, temp);
                flightNo = stoi(temp);

                int passengerIndex = pm.findPassengerIndex(passengerID);
                int flightIndex = fm.findFlightIndex(flightNo);

                // Only load booking if both the passenger and flight still exist
                if (passengerIndex != -1 && flightIndex != -1)
                {
                    b.p = pm.passengers[passengerIndex];
                    b.f = fm.flights[flightIndex];
                    bookings[count++] = b;

                    if (b.bookingID > maxID) maxID = b.bookingID;
                }
            }
            catch (const invalid_argument&)
            {
                cout << "[Load Warning] Skipping corrupt booking record: " << line << "\n";
            }
            catch (const out_of_range&)
            {
                cout << "[Load Warning] Value out of range in booking record: " << line << "\n";
            }
        }

        nextBookingID = maxID + 1;
    }

    // Returns the array index of a booking by booking ID, or -1 if not found
    int findBookingIndex(int bookingID) const
    {
        for (int i = 0; i < count; i++)
            if (bookings[i].bookingID == bookingID) return i;
        return -1;
    }

    void rewriteBookingsFile()
    {
        fh.rewriteBookingsFile(bookings, count);
    }

    void bookFlight(PassengerManager& pm, FlightManager& fm)
    {
        try
        {
            if (fm.count == 0)
                throw NoRecordsException("flights");

            if (count >= MAX_RECORDS)
                throw StorageFullException("Booking storage");

            fm.viewFlights();

            int fid;
            while (true)
            {
                try
                {
                    cout << "Enter Flight No: ";
                    fid = readInt("Flight No");
                    validateID(fid, "Flight No");
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int flightIndex = fm.findFlightIndex(fid);
            if (flightIndex == -1)
                throw RecordNotFoundException("Flight");

            if (fm.flights[flightIndex].status == "In Air")
                throw FlightInAirException();

            if (fm.flights[flightIndex].seats <= 0)
                throw NoSeatsAvailableException();

            // Collect passenger details
            Passenger temp;
            temp.input();

            if (!pm.addPassenger(temp))
                throw AirlineException("Booking unsuccessful — passenger could not be added.");

            // Build the booking record
            Booking b;
            b.bookingID = nextBookingID;
            b.p = temp;
            b.f = fm.flights[flightIndex];

            // Decrement available seats on the chosen flight
            fm.flights[flightIndex].seats--;
            fm.rewriteFlightsFile();

            bookings[count++] = b;
            fh.saveBooking(b);
            nextBookingID++;

            cout << "Booking successful! Your Booking ID is " << b.bookingID << ".\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void cancelBooking(FlightManager& fm)
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("bookings");

            int id;
            while (true)
            {
                try
                {
                    cout << "Enter Booking ID: ";
                    id = readInt("Booking ID");
                    validateID(id, "Booking ID");
                    break;
                }
                catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
            }

            int bookingIndex = findBookingIndex(id);
            if (bookingIndex == -1)
                throw RecordNotFoundException("Booking");

            // Restore the seat on the corresponding flight (if it still exists)
            int flightIndex = fm.findFlightIndex(bookings[bookingIndex].f.flightNo);
            if (flightIndex != -1)
            {
                fm.flights[flightIndex].seats++;
                fm.rewriteFlightsFile();
            }

            for (int j = bookingIndex; j < count - 1; j++)
                bookings[j] = bookings[j + 1];

            count--;
            rewriteBookingsFile();
            cout << "Booking cancelled successfully.\n";
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }

    void viewBookings() const
    {
        try
        {
            if (count == 0)
                throw NoRecordsException("bookings");

            cout << "\n--- Booking List ---\n";
            for (int i = 0; i < count; i++)
                bookings[i].display();
        }
        catch (const AirlineException& e) { cout << "[Error] " << e.what() << "\n"; }
    }
};