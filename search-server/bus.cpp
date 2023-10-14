#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};


istream& operator>>(istream& is, Query& q) {
    // Реализуйте эту функцию
    string request;
    is >> request;
    QueryType& type = q.type;
    int stops_count;
    string& bus = q.bus;
    string& stop = q.stop;
    vector<string>& stops = q.stops; 
    if (request == "NEW_BUS"s){
        type = QueryType::NewBus;
        is >> bus >> stops_count;
        stops.resize(stops_count);
        for (string& stop : stops) {
                is >> stop;
                //stops.push_back(stop);
        }
        /*cout << static_cast<int>(type) << " "s << bus << " "s << stops_count << " "s;
        for (string& stop : stops) {
                cout << stop << " "s;
        }
        cout << endl; */
    } else if (request == "BUSES_FOR_STOP"s){
        type = QueryType::BusesForStop;
        is >> stop;
        //cout << static_cast<int>(type) << " "s << stop << endl;
    } else if (request == "STOPS_FOR_BUS"s){
        type = QueryType::StopsForBus;
        is >> bus;
        //cout << static_cast<int>(type) << " "s <<bus << endl;
    } else if (request == "ALL_BUSES"s){
        type = QueryType::AllBuses;
        //cout << static_cast<int>(type) << endl;
    }  
    
    return is;
}


struct BusesForStopResponse {
    // Наполните полями эту структуру
   vector<string> buses; 
    
}; 

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    // Реализуйте эту функцию
    int i = 0;
    if (r.buses.size() == 0){
        os << "No stop" << endl;
        return os;
    }
    for (const auto& bus: r.buses){
        os << bus;
        if (i!= (r.buses.size() - 1)){
            os << " "s;
            ++i;      
        }
    }
    os << endl;
    return os;
}


struct StopsForBusResponse {
    // Наполните полями эту структуру
   map<string, vector<string>> stops_and_corresponding_buses;
   string bus;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    // Реализуйте эту функцию
    if (r.stops_and_corresponding_buses.size() == 0){
        os << "No bus"s;
  
    } else {
        for (const auto& [stop, buses] : r.stops_and_corresponding_buses){
            os << "Stop"s << " "s << stop << ":";
            for (const auto& bus : buses){
                if (buses.size() == 1 && bus == r.bus){
                    os << "no interchange"s;
                } else{
                    os << " "s << bus;
                }
            }
        }
    }
    os << endl;
    return os;
} 

struct AllBusesResponse {
    // Наполните полями эту структуру
     map<string, vector<string>> all_buses;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    // Реализуйте эту функцию
    if (r.all_buses.empty()){
        os << "No buses"s << endl;
    }
    for (const auto& [bus, stops]: r.all_buses){
        os << "Bus "s << bus << ":";
        for (const auto& stop : stops){
            os << " "s << stop;
        }
        os << endl;
    }
    return os;
} 

class BusManager {
public:
   
    void AddBus(const string& bus, const vector<string>& stops) {
        // Реализуйте этот метод
        buses_to_stops[bus] = stops;
        for (const auto& stop : buses_to_stops[bus]) {
            stops_to_buses[stop].push_back(bus);
        }
       // stops.clear();
        //bus.clear();
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        // Реализуйте этот метод
        BusesForStopResponse buses_for_stop;
        if (stops_to_buses.count(stop) != 0){
            buses_for_stop.buses = stops_to_buses.at(stop);
        } 
        
        return buses_for_stop;
        
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        // Реализуйте этот метод
        StopsForBusResponse stops_for_bus;
        if(buses_to_stops.count(bus) != 0){
            stops_for_bus.bus = bus;
            for (const auto& stop : buses_to_stops.at(bus)){
                stops_for_bus.stops_and_corresponding_buses.at(stop) = stops_to_buses.at(stop);
            }
        }
        return stops_for_bus;
    }

    AllBusesResponse GetAllBuses() const {
        // Реализуйте этот метод
        AllBusesResponse all_buses_response;
        all_buses_response.all_buses = buses_to_stops;
        return  all_buses_response;
    }
    
 private:
    map<string, vector<string>> buses_to_stops, stops_to_buses;
};

// Реализуйте функции и классы, объявленные выше, чтобы эта функция main
// решала задачу "Автобусные остановки"

int main() {
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}