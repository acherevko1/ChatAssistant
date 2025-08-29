#include "database.h"
#include <stdexcept>

Database::Database(const std::string& dbPath) {
    int result = sqlite3_open(dbPath.c_str(), &db);
    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }
    InitializeDatabase();
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

void Database::InitializeDatabase() {
    const char* createTableSql = R"(
        CREATE TABLE IF NOT EXISTS chat_responses (
            chat_id INTEGER PRIMARY KEY,
            response_message_id TEXT NOT NULL
        );
    )";
    
    char* errorMsg = nullptr;
    int result = sqlite3_exec(db, createTableSql, nullptr, nullptr, &errorMsg);
    
    if (result != SQLITE_OK) {
        std::string error = "Failed to create table: ";
        if (errorMsg) {
            error += errorMsg;
            sqlite3_free(errorMsg);
        }
        throw std::runtime_error(error);
    }
}

void Database::SaveResponseMessageId(int64_t chatId, const std::string& responseMessageId) {
    const char* sql = R"(
        INSERT OR REPLACE INTO chat_responses (chat_id, response_message_id) 
        VALUES (?, ?);
    )";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int64(stmt, 1, chatId);
    sqlite3_bind_text(stmt, 2, responseMessageId.c_str(), -1, SQLITE_STATIC);
    
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (result != SQLITE_DONE) {
        throw std::runtime_error("Failed to save response message ID: " + std::string(sqlite3_errmsg(db)));
    }
}

std::string Database::GetResponseMessageId(int64_t chatId) {
    const char* sql = "SELECT response_message_id FROM chat_responses WHERE chat_id = ?;";
    
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    
    if (result != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }
    
    sqlite3_bind_int64(stmt, 1, chatId);
    
    std::string responseMessageId = "";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (text) {
            responseMessageId = text;
        }
    }
    
    sqlite3_finalize(stmt);
    return responseMessageId;
}