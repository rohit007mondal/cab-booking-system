#include<iostream>
#include<string>
#include<vector>
#include<list>
#include<queue>
#include<fstream>
#include<sstream>
#include<ctime>
#include<cstring>
#include<limits>
#include<algorithm>

using namespace std;

#define INF 0x3f3f3f3f

typedef pair<int, int> iPair;

// Structs
struct trip {
    int pickup, drop, cabtype, price, time;
    string cabno, driver_name, cab_name;
};

struct cab {
    int id;
    char cabno[9];
    int pickup;
    char driver_name[20];
    int drop;
    char cab_name[20];
    int cab_status;
    int avg_rating = 5, earnings = 0, noofbookings = 0;
};

struct user_record {
    string name, email, mob, password;
    char gender;
};

vector<string> areas = {"Sector 62", "Sector 128", "Botanical Garden", "Sector 18", "Kaushambi",
                         "Anand Vihar", "Rajiv Chowk", "Kashmere Gate", "Nehru Place", "Pari Chowk"};
cab cabs[50];
int noofcabs = 0;
vector<user_record> users;
vector<trip> trip_log;

class Graph {
    int V;
    list<pair<int, int>>* adj;
public:
    Graph(int V) {
        this->V = V;
        adj = new list<pair<int, int>>[V];
    }
    void addEdge(int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
    int shortestPath(int src, int dest) {
        priority_queue<iPair, vector<iPair>, greater<iPair>> pq;
        vector<int> dist(V, INF);
        pq.push({0, src});
        dist[src] = 0;
        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();
            for (auto& p : adj[u]) {
                int v = p.first;
                int weight = p.second;
                if (dist[v] > dist[u] + weight) {
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});
                }
            }
        }
        return dist[dest];
    }
};
Graph g(10);

void upper() {
    system("cls");
    cout << "\n\t\t\t\t CAB BOOKING SYSTEM \n";
    cout << "\t\t\t\t ------------------\n\n";
}

void delay(int seconds) {
    time_t start_time = time(NULL);
    while (time(NULL) - start_time < seconds);
}

void frontpage() {
    upper();
    cout << "\t\t\t\tWELCOME TO CAB BOOKING SYSTEM\n";
    cout << "\t\t\t\t==============================\n\n";
    delay(2);
}

void loadCabsFromFile() {
    ifstream file("cabdatabase.txt", ios::binary);
    if (!file) return;
    while (file.read(reinterpret_cast<char*>(&cabs[noofcabs]), sizeof(cab))) {
        noofcabs++;
    }
    file.close();
}

void saveCabsToFile() {
    ofstream file("cabdatabase.txt", ios::binary);
    for (int i = 0; i < noofcabs; ++i) {
        file.write(reinterpret_cast<char*>(&cabs[i]), sizeof(cab));
    }
    file.close();
}

void loadUsersFromFile() {
    ifstream file("users.txt");
    user_record u;
    while (file >> u.name >> u.email >> u.mob >> u.gender >> u.password) {
        users.push_back(u);
    }
    file.close();
}

void saveUsersToFile() {
    ofstream file("users.txt");
    for (auto& u : users) {
        file << u.name << ' ' << u.email << ' ' << u.mob << ' ' << u.gender << ' ' << u.password << '\n';
    }
    file.close();
}

bool userLogin(string& email, string& name) {
    upper();
    string password;
    cout << "Enter Email: "; cin >> email;
    cout << "Enter Password: "; cin >> password;
    for (auto& u : users) {
        if (u.email == email && u.password == password) {
            name = u.name;
            cout << "Login successful!\n";
            delay(2);
            return true;
        }
    }
    cout << "Invalid credentials.\n";
    delay(2);
    return false;
}

void userSignup() {
    user_record newUser;
    upper();
    cout << "Enter Name: "; cin.ignore(); getline(cin, newUser.name);
    cout << "Enter Email: "; getline(cin, newUser.email);
    cout << "Enter Mobile No: "; getline(cin, newUser.mob);
    cout << "Enter Gender (M/F): "; cin >> newUser.gender;
    cout << "Enter Password: "; cin >> newUser.password;
    users.push_back(newUser);
    saveUsersToFile();
    cout << "Signup successful! Please log in." << endl;
    delay(2);
}

int findAvailableCab(int pickup, int drop) {
    for (int i = 0; i < noofcabs; ++i) {
        if (cabs[i].cab_status == 1 && cabs[i].pickup == pickup && cabs[i].drop == drop) {
            return i;
        }
    }
    return -1;
}

void bookCab(const string& userEmail) {
    int pickup, drop;
    upper();
    cout << "Book a Cab - Choose Area\n";
    for (int i = 0; i < areas.size(); ++i) {
        cout << i << ". " << areas[i] << "\n";
    }
    cout << "Enter Pickup Index: "; cin >> pickup;
    cout << "Enter Drop Index: "; cin >> drop;

    int distance = g.shortestPath(pickup, drop);
    int fare = distance * 10;

    int cabIndex = findAvailableCab(pickup, drop);
    if (cabIndex == -1) {
        cout << "No cabs available for this route right now.\n";
        delay(2);
        return;
    }

    trip t;
    t.pickup = pickup;
    t.drop = drop;
    t.price = fare;
    t.time = time(0);
    t.cabno = cabs[cabIndex].cabno;
    t.driver_name = cabs[cabIndex].driver_name;
    t.cab_name = cabs[cabIndex].cab_name;

    cabs[cabIndex].earnings += fare;
    cabs[cabIndex].noofbookings++;
    cabs[cabIndex].cab_status = 0; // Now busy

    cout << "\nTrip booked successfully!\n";
    cout << "Fare: Rs. " << fare << "\n";
    trip_log.push_back(t);
    delay(3);
}

void rateTrip() {
    string cabno;
    int rating;
    upper();
    cout << "Enter Cab Number to rate: ";
    cin >> cabno;
    for (int i = 0; i < noofcabs; ++i) {
        if (cabno == cabs[i].cabno) {
            cout << "Enter Rating (1-5): ";
            cin >> rating;
            cabs[i].avg_rating = (cabs[i].avg_rating + rating) / 2;
            cabs[i].cab_status = 1; // Set to available again
            cout << "Thank you for rating!\n";
            delay(2);
            return;
        }
    }
    cout << "Cab not found!\n";
    delay(2);
}

void adminFlow() {
    string pass;
    upper();
    cout << "Enter Admin Password: ";
    cin >> pass;
    if (pass != "admin123") {
        cout << "Incorrect password.\n";
        delay(2);
        return;
    }

    int choice;
    bool back = false;
    while (!back) {
        upper();
        cout << "ADMIN MENU\n";
        cout << "1. Add Cab\n2. Show All Cabs\n3. Logout\nEnter choice: ";
        cin >> choice;
        switch (choice) {
            case 1: {
                cab newCab;
                newCab.id = noofcabs + 1;
                cout << "Cab No: "; cin >> newCab.cabno;
                cout << "Driver Name: "; cin.ignore(); cin.getline(newCab.driver_name, 20);
                cout << "Cab Name: "; cin.getline(newCab.cab_name, 20);
                cout << "Pickup Index: "; cin >> newCab.pickup;
                cout << "Drop Index: "; cin >> newCab.drop;
                newCab.cab_status = 1;
                cabs[noofcabs++] = newCab;
                saveCabsToFile();
                cout << "Cab added!\n";
                delay(2);
                break;
            }
            case 2: {
                upper();
                cout << "Registered Cabs:\n";
                for (int i = 0; i < noofcabs; ++i) {
                    cout << "Cab ID: " << cabs[i].id << " | Cab No: " << cabs[i].cabno
                         << " | Driver: " << cabs[i].driver_name
                         << " | Status: " << (cabs[i].cab_status ? "Available" : "Busy")
                         << " | Earnings: Rs. " << cabs[i].earnings
                         << " | Bookings: " << cabs[i].noofbookings << "\n";
                }
                system("pause");
                break;
            }
            case 3:
                back = true;
                break;
            default:
                cout << "Invalid choice.\n";
                delay(2);
        }
    }
}


void userFlow(const string& email, const string& name) {
    int choice;
    bool back = false;
    while (!back) {
        upper();
        cout << "Welcome " << name << "! What would you like to do?\n";
        cout << "1. Book a Cab\n2. Rate a Cab\n3. Logout\nEnter choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                bookCab(email);
                break;
            case 2:
                rateTrip();
                break;
            case 3:
                back = true;
                break;
            default:
                cout << "Invalid input!\n";
                delay(2);
        }
    }
}


int main() {
    frontpage();
    loadCabsFromFile();
    loadUsersFromFile();

    int choice;
    string currentUser, name;
    bool running = true;
    while (running) {
        upper();
        cout << string(9, '\t') << "********* MAIN MENU **********" << endl << endl;
        cout << "1. USER LOGIN\n2. USER SIGNUP\n3. ADMIN LOGIN\n4. EXIT\nEnter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                if (userLogin(currentUser, name)) userFlow(currentUser, name);
                break;
            case 2:
                userSignup();
                break;
            case 3:
                adminFlow();
                break;
            case 4:
                running = false;
                break;
            default:
                cout << "Invalid input. Please try again." << endl;
                delay(2);
        }
    }
    saveCabsToFile();
    saveUsersToFile();
    return 0;
}
