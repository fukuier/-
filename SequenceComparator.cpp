#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cctype>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <cstring>

using namespace std;

// ==================== 防篡改保护（仅保留版权自检）====================
static const char copyright_str1[] = "QUCHENYANG_COPYRIGHT_2025";
static const char copyright_str2[] = "MADE_BY_QUCHENYANG";
static const char copyright_str3[] = "NO_UNAUTHORIZED_COPY";

bool check_integrity() {
    // 逐字比较，任何改动都会导致失败
    if (strcmp(copyright_str1, "QUCHENYANG_COPYRIGHT_2025") != 0) return false;
    if (strcmp(copyright_str2, "MADE_BY_QUCHENYANG") != 0) return false;
    if (strcmp(copyright_str3, "NO_UNAUTHORIZED_COPY") != 0) return false;
    return true;
}
// ================================================================

// 原有功能代码（tokenize, parse_line 保持不变）
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
            i++;
        } else {
            tokens.push_back(string(1, s[i]));
            i++;
        }
    }
    return tokens;
}

pair<string, string> parse_line(const string& line) {
    size_t start = line.find_first_not_of(" \t");
    if (start == string::npos) {
        return make_pair("", "");
    }
    size_t end_species = line.find_first_of(" \t", start);
    if (end_species == string::npos) {
        return make_pair(line.substr(start), "");
    }
    string species = line.substr(start, end_species - start);
    size_t start_seq = line.find_first_not_of(" \t", end_species);
    if (start_seq == string::npos) {
        return make_pair(species, "");
    }
    string sequence = line.substr(start_seq);
    return make_pair(species, sequence);
}

int main(int argc, char* argv[]) {
    // ========== 防篡改校验 ==========
    if (!check_integrity()) {
        cerr << "ERROR: Program has been tampered with (copyright modified)." << endl;
        return 1;
    }
    // ================================

    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <fileA> <fileB>" << endl;
        return 1;
    }

    string fileA = argv[1];
    string fileB = argv[2];

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
    cout << "Made by QuChenyang" << endl;
    return 0;
}
