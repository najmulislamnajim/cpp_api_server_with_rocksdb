#include "database.h"
#include <iostream>
#include <sstream>

Database::Database(const std::string& path, bool read_only) {
    rocksdb::Options options;
    options.create_if_missing = true; // Create database if it doesn’t exist

    rocksdb::Status status;
    if (read_only) {
        status = rocksdb::DB::OpenForReadOnly(options, path, &db_);
    } else {
        status = rocksdb::DB::Open(options, path, &db_);
    }
    if (!status.ok()) {
        std::cerr << "Failed to open database at " << path << ": " << status.ToString() << std::endl;
        throw std::runtime_error("Database initialization failed");
    }
}

Database::~Database() {
    delete db_; // Clean up when the object is destroyed
}

std::string Database::serializeArray(const std::vector<int>& arr) {
    std::ostringstream oss;
    for (size_t i = 0; i < arr.size(); ++i) {
        oss << arr[i];
        if (i < arr.size() - 1) oss << ",";
    }
    return oss.str();
}

std::vector<int> Database::deserializeArray(const std::string& data) {
    std::vector<int> arr;
    std::istringstream iss(data);
    std::string item;
    while (std::getline(iss, item, ',')) {
        arr.push_back(std::stoi(item));
    }
    return arr;
}

bool Database::put(const std::string& key, const std::vector<int>& value) {
    std::string serialized = serializeArray(value);
    rocksdb::Status status = db_->Put(rocksdb::WriteOptions(), key, serialized);
    if (!status.ok()) {
        std::cerr << "Failed to write key '" << key << "': " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

bool Database::get(const std::string& key, std::vector<int>& value) {
    std::string retrieved;
    rocksdb::Status status = db_->Get(rocksdb::ReadOptions(), key, &retrieved);
    if (!status.ok()) {
        std::cerr << "Failed to read key '" << key << "': " << status.ToString() << std::endl;
        return false;
    }
    value = deserializeArray(retrieved);
    return true;
}

bool Database::remove(const std::string& key) {
    rocksdb::Status status = db_->Delete(rocksdb::WriteOptions(), key);
    if (!status.ok()) {
        std::cerr << "Failed to delete key '" << key << "': " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

bool Database::put_json(const std::string& key, const json& value) {
    std::string json_str = value.dump(); // Serialize JSON to string
    rocksdb::Status status = db_->Put(rocksdb::WriteOptions(), key, json_str);
    if (!status.ok()) {
        std::cerr << "Failed to write key '" << key << "': " << status.ToString() << std::endl;
        return false;
    }
    // Flush to disk immediately
    status = db_->Flush(rocksdb::FlushOptions());
    if (!status.ok()) {
        std::cerr << "Failed to flush database: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

bool Database::get_json(const std::string& key, json& value) {
    std::string retrieved;
    rocksdb::Status status = db_->Get(rocksdb::ReadOptions(), key, &retrieved);
    if (!status.ok()) {
        std::cerr << "Failed to read key '" << key << "': " << status.ToString() << std::endl;
        return false;
    }
    value = json::parse(retrieved); // Parse string back to JSON
    return true;
}

std::vector<std::string> Database::get_all_keys() {
    std::vector<std::string> keys;
    rocksdb::ReadOptions read_options;
    read_options.snapshot = nullptr;
    rocksdb::Iterator* it = db_->NewIterator(read_options);
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        keys.push_back(it->key().ToString());
    }
    delete it; // Clean up the iterator
    return keys;
}