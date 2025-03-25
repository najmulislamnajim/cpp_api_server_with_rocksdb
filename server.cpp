/*
Author : Najmul Islam
E-mail: najmulislamru@gmail.com 
Date: 2025-03-25
Last Modified: 2025-03-25
How to Run: g++ -o myapp app.cpp database.cpp -lrocksdb -std=c++17 -Icpp-httplib
*/
#include <iostream>
#include <httplib.h>
#include "database.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {

    std::string db_path = "/mnt/test/Desktop/Impala/learning/rocksdb/learning";
    try {
        Database db(db_path);

        // Create an HTTP server
        httplib::Server svr;

        // Set up MariaDB connection
        sql::Driver *driver;
        sql::Connection *con;

        // Initialize MySQL driver and create a connection
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://178.128.94.148:3306", "root", "&j}HS9L02z");

        // Connect to the 'odms_dev_db' database
        con->setSchema("odms_dev_db");

        // Define the POST API endpoint: /api/data
        svr.Post("/api/bill", [&db, con](const httplib::Request& req, httplib::Response& res) {
            try {
                // Parse the incoming JSON data from the request body
                json data = json::parse(req.body);

                // Extract a key from the JSON (e.g., "key" field) or generate one
                std::string key;
                if (data.contains("key") && data["key"].is_string()) {
                    key = data["key"].get<std::string>();
                } else {
                    // If no key is provided, generate a unique one (e.g., timestamp)
                    key = "data_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
                }

                // Store the JSON data in RocksDB
                bool rocksdb_success = db.put_json(key, data);
                if (!rocksdb_success) {
                    res.set_content("{\"status\": \"error\", \"message\": \"RocksDB write failed\"}", "application/json");
                    res.status = 400; // Bad Request
                    return;
                }

                // Extract fields from JSON for MariaDB
                std::string billing_doc_no = data["billing_doc_no"].is_number() ? std::to_string(data["billing_doc_no"].get<int>()) : "";
                std::string billing_date = data["billing_date"].get<std::string>();
                int da_code = data["da_code"].get<int>();

                // Store the JSON data in MariaDB
                std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
                    "INSERT INTO rdl_test (billing_doc_no, billing_date, da_code) VALUES (?, ?, ?)"
                ));
                pstmt->setString(1, billing_doc_no);
                pstmt->setString(2, billing_date);
                pstmt->setInt(3, da_code);
                pstmt->execute();

                // If both operations succeed, return success
                res.set_content("{\"status\": \"success\", \"key\": \"" + key + "\"}", "application/json");
                res.status = 201; // Created
            } catch (const json::parse_error& e) {
                res.set_content("{\"status\": \"error\", \"message\": \"Invalid JSON: " + std::string(e.what()) + "\"}", "application/json");
                res.status = 400; // Bad Request
            } catch (const sql::SQLException& e) {
                res.set_content("{\"status\": \"error\", \"message\": \"MariaDB error: " + std::string(e.what()) + "\"}", "application/json");
                res.status = 500; // Internal Server Error
            } catch (const std::exception& e) {
                res.set_content("{\"status\": \"error\", \"message\": \"Server error: " + std::string(e.what()) + "\"}", "application/json");
                res.status = 500; // Internal Server Error
            }
        });

        // Optional: Add a simple GET endpoint to verify the server is running
        svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
            res.set_content("Welcome to the RocksDB API server!", "text/plain");
        });

        // Start the server
        std::cout << "Server starting on port 8080..." << std::endl;
        svr.listen("0.0.0.0", 8080);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}