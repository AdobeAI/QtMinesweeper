#include "game_record.h"

Game_record::Game_record()
{

}

bool Game_record::connectMySQL() {
    try {
        driver = get_driver_instance();
        connection.reset(driver->connect("127.0.0.1:3306", "root", "Rbrian-a477"));

        connection->setAutoCommit(false);
        //如果連接處於自動認可模式，則它的所有 SQL 陳述式都會當做個別交易來執行及認可。
        //否則，它的 SQL 陳述式會分組成許多交易，這些交易會透過 commit 方法或 rollback 方法的呼叫來結束。
        //根據預設，新的連接會處於自動認可模式。

        connection->setSchema("mine_sweeper");
    } catch(sql::SQLException e) {
        std::cout << "Could not connection to server. Error message: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Game_record::addGameRecord(std::string player_name, int second, int level) {

    std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("INSERT INTO mine_sweeper_record (id, player_name, time, level) VALUES (?,?,?,?)"));
    try {
        pstmt->setNull(1, sql::DataType::INTEGER);
        pstmt->setString(2, player_name);
        pstmt->setInt(3, second);
        pstmt->setInt(4, level);
        std::cout << pstmt->executeUpdate() << std::endl;
        connection->commit();
    } catch (sql::SQLException e) {
        std::cout << "Could not insert to server. Error message: " << e.what() << std::endl;
        return false;
    }
    pstmt->close();

    return true;
}

std::map<int, Score> Game_record::getGameRecord(int level) {
    std::map<int, Score> scores;
    std::unique_ptr<sql::ResultSet> resultSet(nullptr);
    std::unique_ptr<sql::PreparedStatement> pstmt(connection->prepareStatement("SELECT * FROM mine_sweeper_record WHERE level = ? ORDER BY time ASC"));
    try {
        pstmt->setInt(1, level);
        resultSet.reset(pstmt->executeQuery());
        int i = 0;
        while(resultSet->next()) {
            Score score;
            score.setPlayerName(resultSet->getString(TABLE_COLUMN::PLAYER_NAME));
            score.setTime(resultSet->getInt(TABLE_COLUMN::TIME));

            scores.insert(std::make_pair(i, score));
            i++;
        }
    } catch (sql::SQLException e) {
        std::cout << "Could not select to server. Error message: " << e.what() << std::endl;
    }
    pstmt->close();
    resultSet->close();
    return scores;
}

void Game_record::closeConnect() {
    connection->close();
}


std::string Score::getPlayerName() const {
    return player_name;
}
int Score::getTime() const {
    return time;
}
void Score::setPlayerName(const std::string name) {
    player_name = name;
}
void Score::setTime(const int _time) {
    time = _time;
}
