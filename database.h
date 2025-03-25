#ifndef DATABASE_H
#define DATABASE_H 

#include <string>
#include <vector>
#include<rocksdb/db.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Database {
    public:
        Database(const std::string& path, bool read_only = false); // Constructor opens the database
        ~Database();                       // Destructor closes the database
    
        bool put(const std::string& key, const std::vector<int>& value); // Write an array
        bool get(const std::string& key, std::vector<int>& value);       // Read an array
        bool remove(const std::string& key);                             // Delete a key (optional)
        bool put_json(const std::string& key, const json& value);
        bool get_json(const std::string& key, json& value);
        std::vector<std::string> get_all_keys();
    
    private:
        rocksdb::DB* db_;               // Pointer to the RocksDB instance
        std::string serializeArray(const std::vector<int>& arr); // Helper to serialize
        std::vector<int> deserializeArray(const std::string& data); // Helper to deserialize
    };
    
    #endif