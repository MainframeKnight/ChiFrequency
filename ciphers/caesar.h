#include"oneletterfreq.h"
#include<utility>
#include<algorithm>
#include<iostream>
class caesarCipher {
public:
    const char* decrypt(const char* ciphertext) {
        FrequencyAnalyser freq{};
        std::string initial_string{ciphertext};
        std::string string_copy{initial_string};
        std::vector<std::pair<int, double>> variants{};
        variants.emplace_back(0, freq.computeTestStatistic(string_copy));
        for (int i{0}; i < 25; i++) {
            caesarByOne(string_copy);
            variants.emplace_back(i + 1, freq.computeTestStatistic(string_copy));
        }
        std::sort(variants.begin(), variants.end(), [](auto& e1, auto& e2) {
            return e1.second < e2.second;
        });
        string_copy = caesar(initial_string, variants[0].first);
        char* ret = new char[string_copy.size()];
        for (int i{0}; i < string_copy.size(); i++) {
            ret[i] = string_copy[i];
        }
        return ret;
    }
private:
    void caesarByOne(std::string& text) {
        for (auto& i : text) {
            if (i + 1 > 'z') {
                i = 'a';
            } else {
                i++;
            }
        }
    }
    std::string caesar(const std::string& text, int key) {
        std::string res;
        for (auto& i : text) {
            if (i + key > 'z') {
                res += std::string(1, i + key - 'z' + 'a' - 1);
            } else {
                res += std::string(1, i + key);
            }
        }
        return res;
    }
};