#pragma once
#include"bigramfreq.h"
#include<utility>
#include<algorithm>
#include<iostream>
class caesarCipher {
public:
    // Deciphers using key if it is given, otherwise attempts to brute-force.
    // The function returns allocated memory for both the key and resulting plaintext.
    const char* decrypt(const char* ciphertext, char **key) {
        if (key == nullptr) {
            throw std::invalid_argument{"key address cannot be a null pointer."};
        }
        if (*key != nullptr) {
            std::string res{caesar(std::string(ciphertext), 26 - std::stoi(std::string(*key)))};
            char* ret = new char[res.size() + 1];
            for (int i{0}; i < res.size(); i++) {
                ret[i] = res[i];
            }
            ret[res.size()] = '\0';
            return ret;
        }
        BigramAnalyser freq{};
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
        for (auto& vars : variants) {
            std::cout << vars.first << " " << vars.second << " " << caesar(initial_string, vars.first) << "\n";
        }
        std::string key_result{std::to_string(26 - variants[0].first)};
        *key = new char[key_result.size() + 1];
        for (int i{0}; i < key_result.size(); i++) {
            (*key)[i] = key_result[i];
        }
        (*key)[key_result.size()] = '\0';
        string_copy = caesar(initial_string, variants[0].first);
        char* ret = new char[string_copy.size() + 1];
        for (int i{0}; i < string_copy.size(); i++) {
            ret[i] = string_copy[i];
        }
        ret[string_copy.size()] = '\0';
        return ret;
    }
    void caesarByOne(std::string& text) {
        for (auto& i : text) {
            if (isupper(i)) {
                i = (i - 'A' + 1) % 26 + 'A';
            } else if (islower(i)) {
                i = (i - 'a' + 1) % 26 + 'a';
            }
        }
    }
    std::string caesar(const std::string& text, int key) {
        std::string res{};
        for (auto& i : text) {
            if (!isalpha(i)) {
                res += i;
            }
            else if (islower(i)) {
                res += (i - 'a' + key) % 26 + 'a';
            } else {
                res += (i - 'A' + key) % 26 + 'A';
            }
        }
        return res;
    }
};