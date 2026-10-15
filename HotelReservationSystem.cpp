/*
===========================================================
        HOTEL RESERVATION SYSTEM
Project Code       : CPP79
Enrollment Number  : IU2641231919
===========================================================
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <stdexcept>
#include <limits>

using namespace std;

// =========================================================
// ROOM CLASS
// =========================================================
class Room {
protected:
    int roomNumber;
    string type;
    double pricePerNight;
    bool available;

public:
    // Default Constructor
    Room() {
        roomNumber = 0;
        type = "Standard";
        pricePerNight = 0;
        available = true;
    }

    // Parameterized Constructor
    Room(int number, string roomType, double price, bool isAvailable = true) {
        roomNumber = number;
        type = roomType;
        pricePerNight = price;
        available = isAvailable;
    }

    // Destructor
    virtual ~Room() {}

    int getRoomNumber() const {
        return roomNumber;
    }

    string getType() const {
        return type;
    }

    double getPricePerNight() const {
        return pricePerNight;
    }

    bool isAvailable() const {
        return available;
    }

    void setAvailable(bool value) {
        available = value;
    }

    // Virtual function for runtime polymorphism
    virtual double calculateRate(int nights) const {
        return pricePerNight * nights;
    }

    virtual string getRoomCategory() const {
        return "Standard";
    }

    virtual string serialize() const {
        ostringstream out;

        out << roomNumber << "|"
            << type << "|"
            << fixed << setprecision(2)
            << pricePerNight << "|"
            << (available ? 1 : 0);

        return out.str();
    }
};


// =========================================================
// SUITE ROOM - INHERITANCE
// =========================================================
class SuiteRoom : public Room {
public:

    SuiteRoom() : Room(0, "Suite", 0, true) {}

    SuiteRoom(int number, double price, bool isAvailable = true)
        : Room(number, "Suite", price, isAvailable) {}

    ~SuiteRoom() override {}

    // Runtime polymorphism
    double calculateRate(int nights) const override {
        return Room::calculateRate(nights) * 1.10;
    }

    string getRoomCategory() const override {
        return "Suite";
    }
};


// =========================================================
// GUEST CLASS
// =========================================================
class Guest {
private:
    int guestId;
    string name;
    string phone;
    string email;

public:

    // Default Constructor
    Guest() {
        guestId = 0;
        name = "";
        phone = "";
        email = "";
    }

    // Parameterized Constructor
    Guest(int id, string guestName, string guestPhone, string guestEmail) {
        guestId = id;
        name = guestName;
        phone = guestPhone;
        email = guestEmail;
    }

    // Destructor
    ~Guest() {}

    int getId() const {
        return guestId;
    }

    string getName() const {
        return name;
    }

    string getPhone() const {
        return phone;
    }

    string getEmail() const {
        return email;
    }

    void setName(string value) {
        name = value;
    }

    void setPhone(string value) {
        phone = value;
    }

    void setEmail(string value) {
        email = value;
    }

    string serialize() const {
        ostringstream out;

        out << guestId << "|"
            << name << "|"
            << phone << "|"
            << email;

        return out.str();
    }
};


// =========================================================
// RESERVATION CLASS
// =========================================================
class Reservation {
private:
    int reservationId;
    int guestId;
    int roomNumber;
    int nights;
    string checkInDate;
    string status;

public:

    // Default Constructor
    Reservation() {
        reservationId = 0;
        guestId = 0;
        roomNumber = 0;
        nights = 0;
        checkInDate = "";
        status = "Active";
    }

    // Parameterized Constructor
    Reservation(int id, int guest, int room, int numberOfNights,
                string date, string reservationStatus = "Active") {

        reservationId = id;
        guestId = guest;
        roomNumber = room;
        nights = numberOfNights;
        checkInDate = date;
        status = reservationStatus;
    }

    // Destructor
    ~Reservation() {}

    int getId() const {
        return reservationId;
    }

    int getGuestId() const {
        return guestId;
    }

    int getRoomNumber() const {
        return roomNumber;
    }

    int getNights() const {
        return nights;
    }

    string getCheckInDate() const {
        return checkInDate;
    }

    string getStatus() const {
        return status;
    }

    string serialize() const {
        ostringstream out;

        out << reservationId << "|"
            << guestId << "|"
            << roomNumber << "|"
            << nights << "|"
            << checkInDate << "|"
            << status;

        return out.str();
    }
};


// =========================================================
// BILL CLASS
// =========================================================
class Bill {
protected:
    int billId;
    int reservationId;
    double roomCharge;
    double taxRate;

public:

    // Default Constructor
    Bill() {
        billId = 0;
        reservationId = 0;
        roomCharge = 0;
        taxRate = 0.05;
    }

    // Parameterized Constructor
    Bill(int id, int reservation, double charge, double tax = 0.05) {
        billId = id;
        reservationId = reservation;
        roomCharge = charge;
        taxRate = tax;
    }

    // Destructor
    virtual ~Bill() {}

    virtual double calculateTotal() const {
        return roomCharge + (roomCharge * taxRate);
    }

    virtual string getBillType() const {
        return "Regular";
    }

    int getId() const {
        return billId;
    }

    int getReservationId() const {
        return reservationId;
    }

    string serialize() const {
        ostringstream out;

        out << billId << "|"
            << reservationId << "|"
            << fixed << setprecision(2)
            << roomCharge << "|"
            << taxRate;

        return out.str();
    }
};


// =========================================================
// CORPORATE BILL - INHERITANCE
// =========================================================
class CorporateBill : public Bill {
public:

    CorporateBill() : Bill() {}

    CorporateBill(int id, int reservation, double charge)
        : Bill(id, reservation, charge, 0.12) {}

    ~CorporateBill() override {}

    double calculateTotal() const override {

        double discount = roomCharge * 0.90;

        return discount + (discount * taxRate);
    }

    string getBillType() const override {
        return "Corporate";
    }
};


// =========================================================
// HOTEL CLASS
// =========================================================
class Hotel {
private:

    string hotelName;

    // Dynamic memory
    Room** rooms;
    Guest* guests;
    Reservation* reservations;
    Bill* bills;

    int roomCount;
    int roomCapacity;

    int guestCount;
    int guestCapacity;

    int reservationCount;
    int reservationCapacity;

    int billCount;
    int billCapacity;


    // =====================================================
    // RESIZE ROOMS
    // =====================================================
    void resizeRooms() {

        int newCapacity = roomCapacity * 2;

        Room** temp = new Room*[newCapacity];

        for (int i = 0; i < newCapacity; i++)
            temp[i] = nullptr;

        for (int i = 0; i < roomCount; i++)
            temp[i] = rooms[i];

        delete[] rooms;

        rooms = temp;
        roomCapacity = newCapacity;
    }


    // =====================================================
    // RESIZE GUESTS
    // =====================================================
    void resizeGuests() {

        int newCapacity = guestCapacity * 2;

        Guest* temp = new Guest[newCapacity];

        for (int i = 0; i < guestCount; i++)
            temp[i] = guests[i];

        delete[] guests;

        guests = temp;
        guestCapacity = newCapacity;
    }


    // =====================================================
    // RESIZE RESERVATIONS
    // =====================================================
    void resizeReservations() {

        int newCapacity = reservationCapacity * 2;

        Reservation* temp = new Reservation[newCapacity];

        for (int i = 0; i < reservationCount; i++)
            temp[i] = reservations[i];

        delete[] reservations;

        reservations = temp;
        reservationCapacity = newCapacity;
    }


    // =====================================================
    // RESIZE BILLS
    // =====================================================
    void resizeBills() {

        int newCapacity = billCapacity * 2;

        Bill* temp = new Bill[newCapacity];

        for (int i = 0; i < billCount; i++)
            temp[i] = bills[i];

        delete[] bills;

        bills = temp;
        billCapacity = newCapacity;
    }


public:

    // =====================================================
    // DEFAULT CONSTRUCTOR
    // =====================================================
    Hotel() {

        hotelName = "Blue Horizon Hotel";

        roomCapacity = 4;
        guestCapacity = 4;
        reservationCapacity = 4;
        billCapacity = 4;

        roomCount = 0;
        guestCount = 0;
        reservationCount = 0;
        billCount = 0;

        // Dynamic memory allocation
        rooms = new Room*[roomCapacity];
        guests = new Guest[guestCapacity];
        reservations = new Reservation[reservationCapacity];
        bills = new Bill[billCapacity];

        for (int i = 0; i < roomCapacity; i++)
            rooms[i] = nullptr;
    }


    // =====================================================
    // PARAMETERIZED CONSTRUCTOR
    // =====================================================
    Hotel(string name) : Hotel() {
        hotelName = name;
    }


    // =====================================================
    // DESTRUCTOR
    // =====================================================
    ~Hotel() {

        for (int i = 0; i < roomCount; i++)
            delete rooms[i];

        delete[] rooms;
        delete[] guests;
        delete[] reservations;
        delete[] bills;
    }


    // =====================================================
    // ADD ROOM
    // =====================================================
    void addRoom(Room* room) {

        if (room == nullptr)
            throw invalid_argument("Invalid room.");

        if (roomCount == roomCapacity)
            resizeRooms();

        rooms[roomCount] = room;
        roomCount++;
    }


    // =====================================================
    // ADD GUEST
    // =====================================================
    void addGuest(const Guest& guest) {

        if (guestCount == guestCapacity)
            resizeGuests();

        guests[guestCount] = guest;
        guestCount++;
    }


    // =====================================================
    // ADD RESERVATION
    // =====================================================
    void addReservation(const Reservation& reservation) {

        if (reservationCount == reservationCapacity)
            resizeReservations();

        reservations[reservationCount] = reservation;
        reservationCount++;
    }


    // =====================================================
    // ADD BILL
    // =====================================================
    void addBill(const Bill& bill) {

        if (billCount == billCapacity)
            resizeBills();

        bills[billCount] = bill;
        billCount++;
    }


    // =====================================================
    // ADD RECORD
    // =====================================================
    void addRecord() {

        int choice;

        cout << "\n========== ADD RECORD ==========\n";
        cout << "1. Add Room\n";
        cout << "2. Add Guest\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {

            int number;
            int type;
            double price;

            cout << "Enter room number: ";
            cin >> number;

            cout << "1. Standard Room\n";
            cout << "2. Suite Room\n";
            cout << "Enter room type: ";
            cin >> type;

            cout << "Enter price per night: ";
            cin >> price;

            if (number <= 0 || price <= 0)
                throw invalid_argument("Invalid room details.");

            for (int i = 0; i < roomCount; i++) {

                if (rooms[i]->getRoomNumber() == number)
                    throw runtime_error("Room already exists.");
            }

            if (type == 2)
                addRoom(new SuiteRoom(number, price));
            else
                addRoom(new Room(number, "Standard", price));

            cout << "Room added successfully.\n";
        }


        else if (choice == 2) {

            int id;
            string name, phone, email;

            cout << "Enter Guest ID: ";
            cin >> id;

            if (id <= 0)
                throw invalid_argument("Guest ID must be positive.");

            for (int i = 0; i < guestCount; i++) {

                if (guests[i].getId() == id)
                    throw runtime_error("Guest ID already exists.");
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Enter name: ";
            getline(cin, name);

            cout << "Enter phone: ";
            getline(cin, phone);

            cout << "Enter email: ";
            getline(cin, email);

            if (name.empty() || phone.empty() || email.empty())
                throw invalid_argument("Guest details cannot be empty.");

            addGuest(Guest(id, name, phone, email));

            cout << "Guest added successfully.\n";
        }

        else {
            cout << "Invalid choice.\n";
        }
    }


    // =====================================================
    // DISPLAY RECORDS
    // =====================================================
    void displayRecords() const {

        cout << "\n========== ROOMS ==========\n";

        cout << left
             << setw(10) << "Room"
             << setw(15) << "Type"
             << setw(12) << "Price"
             << "Status\n";

        for (int i = 0; i < roomCount; i++) {

            cout << left
                 << setw(10) << rooms[i]->getRoomNumber()
                 << setw(15) << rooms[i]->getRoomCategory()
                 << setw(12) << rooms[i]->getPricePerNight()
                 << (rooms[i]->isAvailable()
                     ? "Available"
                     : "Booked")
                 << "\n";
        }


        cout << "\n========== GUESTS ==========\n";

        cout << left
             << setw(8) << "ID"
             << setw(20) << "Name"
             << setw(16) << "Phone"
             << "Email\n";

        for (int i = 0; i < guestCount; i++) {

            cout << left
                 << setw(8) << guests[i].getId()
                 << setw(20) << guests[i].getName()
                 << setw(16) << guests[i].getPhone()
                 << guests[i].getEmail()
                 << "\n";
        }


        cout << "\n========== RESERVATIONS ==========\n";

        cout << left
             << setw(8) << "ID"
             << setw(8) << "Guest"
             << setw(8) << "Room"
             << setw(8) << "Nights"
             << setw(15) << "Date"
             << "Status\n";

        for (int i = 0; i < reservationCount; i++) {

            cout << left
                 << setw(8) << reservations[i].getId()
                 << setw(8) << reservations[i].getGuestId()
                 << setw(8) << reservations[i].getRoomNumber()
                 << setw(8) << reservations[i].getNights()
                 << setw(15) << reservations[i].getCheckInDate()
                 << reservations[i].getStatus()
                 << "\n";
        }


        cout << "\n========== BILLS ==========\n";

        cout << left
             << setw(8) << "ID"
             << setw(15) << "Reservation"
             << setw(12) << "Type"
             << "Total\n";

        for (int i = 0; i < billCount; i++) {

            cout << left
                 << setw(8) << bills[i].getId()
                 << setw(15) << bills[i].getReservationId()
                 << setw(12) << bills[i].getBillType()
                 << fixed << setprecision(2)
                 << bills[i].calculateTotal()
                 << "\n";
        }
    }


    // =====================================================
    // SEARCH RECORD
    // =====================================================
    void searchRecord() const {

        int id;

        cout << "\n========== SEARCH RECORD ==========\n";
        cout << "Enter Guest ID: ";
        cin >> id;

        for (int i = 0; i < guestCount; i++) {

            if (guests[i].getId() == id) {

                cout << "\nGuest Found\n";
                cout << "ID    : " << guests[i].getId() << "\n";
                cout << "Name  : " << guests[i].getName() << "\n";
                cout << "Phone : " << guests[i].getPhone() << "\n";
                cout << "Email : " << guests[i].getEmail() << "\n";

                return;
            }
        }

        cout << "Guest not found.\n";
    }


    // =====================================================
    // UPDATE RECORD
    // =====================================================
    void updateRecord() {

        int id;

        cout << "\n========== UPDATE RECORD ==========\n";
        cout << "Enter Guest ID: ";
        cin >> id;

        for (int i = 0; i < guestCount; i++) {

            if (guests[i].getId() == id) {

                string name, phone, email;

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                cout << "Enter new name: ";
                getline(cin, name);

                cout << "Enter new phone: ";
                getline(cin, phone);

                cout << "Enter new email: ";
                getline(cin, email);

                if (name.empty() ||
                    phone.empty() ||
                    email.empty()) {

                    throw invalid_argument(
                        "Details cannot be empty."
                    );
                }

                guests[i].setName(name);
                guests[i].setPhone(phone);
                guests[i].setEmail(email);

                cout << "Guest updated successfully.\n";

                return;
            }
        }

        cout << "Guest not found.\n";
    }


    // =====================================================
    // DELETE RECORD
    // =====================================================
    void deleteRecord() {

        int id;

        cout << "\n========== DELETE RECORD ==========\n";
        cout << "Enter Guest ID: ";
        cin >> id;

        for (int i = 0; i < guestCount; i++) {

            if (guests[i].getId() == id) {

                for (int j = i; j < guestCount - 1; j++)
                    guests[j] = guests[j + 1];

                guestCount--;

                cout << "Guest deleted successfully.\n";

                return;
            }
        }

        cout << "Guest not found.\n";
    }


    // =====================================================
    // MAIN TRANSACTION / RESERVATIO
