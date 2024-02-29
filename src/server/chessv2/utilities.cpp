#include "utilities.h"
#include "game.h"


namespace util {
    string numstocoords(int x1, int  y1, int x2, int y2) {
        string result = "temp";
        result[0] = x1 + 97;
        result[1] = y1 + 49;
        result[2] = x2 + 97;
        result[3] = y2 + 49;
        return result;
    }

    //turn board to value board ready for Cnn
    //P=1,K=10, Q=9, R=5, B=4, N=3
    unordered_map<char, double> piece_to_double;

    void init_piece_to_double() {
        piece_to_double.insert({ 'p',-1.0 });
        piece_to_double.insert({ 'P', 1.0 });
        piece_to_double.insert({ 'n', -3.0 });
        piece_to_double.insert({ 'N', 3.0 });
        piece_to_double.insert({ 'b',-4.0 });
        piece_to_double.insert({ 'B', 4.0 });
        piece_to_double.insert({ 'r',-5.0 });
        piece_to_double.insert({ 'R', 5.0 });
        piece_to_double.insert({ 'q',-9.0 });
        piece_to_double.insert({ 'Q', 9.0 });
        piece_to_double.insert({ 'k',-10.0 });
        piece_to_double.insert({ 'K', 10.0 });
    }

    //TODO: TURN BOARD TO BITMAPS
    //thelw na epistrefei 12 bitmaps=uint64_ts
    //px uint64_t pawn = 0*8 1*8 6*8*0 0000000011111111000000000000000000000000000000000000000000000000
    //ta mavra pionia sthn arxikh thesh
    //me th seira: rnbqkpPRNBQK

    vector<uint64_t> board_to_bitmaps(char(&board)[8][8]) {
        vector<uint64_t> result;
        unordered_map<char, int> hashmap;
        hashmap.insert({ 'r', 7 });
        hashmap.insert({ 'n', 8 });
        hashmap.insert({ 'b', 9 });
        hashmap.insert({ 'q', 10 });
        hashmap.insert({ 'k', 11 });
        hashmap.insert({ 'p', 6 });
        hashmap.insert({ 'P', 5 });
        hashmap.insert({ 'R', 0 });
        hashmap.insert({ 'N', 1 });
        hashmap.insert({ 'B', 2 });
        hashmap.insert({ 'Q', 3 });
        hashmap.insert({ 'K', 4 });
        for (int i = 0; i < 12; ++i) result.push_back(0); //arxikopoihsh se mhdenika
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                uint64_t temp = 1;
                temp = temp << (64 - 8 * (i)-(j + 1));
                if (board[j][i] != ' ')result[hashmap[board[j][i]]] = result[hashmap[board[j][i]]] | temp;
            }
        }
        return result;
    }

    vector<double> serialize(vector<vector<double>>& in) {
        vector<double> result;
        for (int i = 0; i < in.size(); ++i) {
            for (int j = 0; j < in[i].size(); ++j) {
                result.push_back(in[i][j]);
            }
        }
        return result;
    }

    Logger& Logger::getInstance() {
       static Logger instance;
       return instance;
    }

    void Logger::logMessage(const std::string& message) {
        logStream << message << std::endl;
    }
    /*
    size_t getCurrentMemoryUsageKB() {
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
        return static_cast<size_t>(pmc.WorkingSetSize / 1024);  // WorkingSetSize is in bytes
    }
    */
}
