#ifndef GAME_RECORD_H
#define GAME_RECORD_H


#include <stdlib.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <map>

enum TABLE_COLUMN {
    ID = 1,
    PLAYER_NAME,
    TIME,
    LEVEL
};

class Score {
public:
    std::string getPlayerName() const;
    int getTime() const;
    void setPlayerName(const std::string name);
    void setTime(const int time);

private:
    std::string player_name;
    int time;
};

class Game_record
{
public:
    Game_record();
    bool connectMySQL();
    bool addGameRecord(std::string player_name, int second, int level);
    std::map<int, Score> getGameRecord(int level);
    void closeConnect();

private:
    sql::Driver *driver;
    std::unique_ptr<sql::Connection> connection;
};


#endif // GAME_RECORD_H
