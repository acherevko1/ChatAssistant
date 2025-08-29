#include <string>
#include <sqlite3.h>

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();
    
    void SaveResponseMessageId(int64_t chatId, const std::string& responseMessageId);
    std::string GetResponseMessageId(int64_t chatId);
    
private:
    sqlite3* db;
    void InitializeDatabase();
};