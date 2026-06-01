#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>

using namespace std;

// 分割字符串为token序列
vector<string> tokenize(const string& s) {
    vector<string> tokens;
    size_t i = 0;
    while (i < s.length()) {
        if (s[i] == '{') {
            size_t j = s.find('}', i);
            if (j == string::npos) {
                tokens.push_back(s.substr(i));
                break;
            }
            tokens.push_back(s.substr(i, j - i + 1));
            i = j + 1;
        } else if (isspace(static_cast<unsigned char>(s[i]))) {
            // 跳过空格
            i++;
        } else {
            tokens.push_back(string(1, s[i]));
            i++;
        }
    }
    return tokens;
}

// 解析一行数据，分离物种名和序列
pair<string, string> parse_line(const string& line) {
    // 跳过行首空白
    size_t start = line.find_first_not_of(" \t");
    if (start == string::npos) {
        return make_pair("", "");
    }
    
    // 找到物种名结束位置
    size_t end_species = line.find_first_of(" \t", start);
    if (end_species == string::npos) {
        return make_pair(line.substr(start), "");
    }
    
    string species = line.substr(start, end_species - start);
    
    // 获取序列部分
    size_t start_seq = line.find_first_not_of(" \t", end_species);
    if (start_seq == string::npos) {
        return make_pair(species, "");
    }
    
    string sequence = line.substr(start_seq);
    return make_pair(species, sequence);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <fileA> <fileB>" << endl;
        return 1;
    }

    string fileA = argv[1];
    string fileB = argv[2];

    // 读取文件A
    ifstream finA(fileA);
    if (!finA) {
        cerr << "Error opening file: " << fileA << endl;
        return 1;
    }

    map<string, string> mapA;
    string line;
    while (getline(finA, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto species_seq = parse_line(line);
        if (!species_seq.first.empty()) {
            mapA[species_seq.first] = species_seq.second;
        }
    }
    finA.close();

    // 读取文件B
    ifstream finB(fileB);
    if (!finB) {
        cerr << "Error opening file: " << fileB << endl;
        return 1;
    }

    map<string, string> mapB;
    while (getline(finB, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        auto species_seq = parse_line(line);
        if (!species_seq.first.empty()) {
            mapB[species_seq.first] = species_seq.second;
        }
    }
    finB.close();

    // 比较每个物种
    for (const auto& entry : mapB) {
        string species = entry.first;
        string seqB = entry.second;

        if (mapA.find(species) == mapA.end()) {
            cerr << species << " not found in fileA!" << endl;
            continue;
        }

        string seqA = mapA[species];
        vector<string> tokensA = tokenize(seqA);
        vector<string> tokensB = tokenize(seqB);
        size_t min_len = min(tokensA.size(), tokensB.size());

        vector<tuple<size_t, string, string>> diffs;
        for (size_t i = 0; i < min_len; i++) {
            if (tokensA[i] != tokensB[i]) {
                diffs.push_back(make_tuple(i + 1, tokensA[i], tokensB[i]));
            }
        }

        // 输出结果
        cout << "Species: " << species << "\n";
        cout << "Total differences: " << diffs.size() << "\n";
        cout << "Common length: " << min_len << "\n";
        
        if (!diffs.empty()) {
            cout << "Differences (position: A vs B):\n";
            for (const auto& diff : diffs) {
                cout << "  " << get<0>(diff) << ": '" 
                     << get<1>(diff) << "' vs '" 
                     << get<2>(diff) << "'\n";
            }
        } else {
            cout << "No differences found in common length\n";
        }
        cout << "----------------------------------------\n";
    }
    return 0;
}
