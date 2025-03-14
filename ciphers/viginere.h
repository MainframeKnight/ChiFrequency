#pragma once
#include"bigramfreq.h"
#include"caesar.h"
#include<string>
#include<vector>
#include<cmath>
#define MAX_KEYLEN 12
#define KEYLEN_ITERS 12
class ViginereCipher {
public:
    // Deciphers using key if it is given, otherwise attempts to brute-force.
    // The function returns allocated memory for both the key and resulting plaintext.
    const char* decrypt(const char* ciphertext, char **key) {
        if (key == nullptr) {
            throw std::invalid_argument{"key address cannot be a null pointer."};
        } else if (*key != nullptr) {
            // Keyed decryption.
            auto size{std::string(ciphertext).size()}, keysize{std::string(*key).size()};
            char* ret = new char[size + 1];
            int index{0};
            for (int i{0}; i < size; i++) {
                if (std::isalpha(ciphertext[i])) {
                    ret[i] = mod26(std::tolower(ciphertext[i]) - std::tolower((*key)[index])) + 'a';
                    index = (index + 1) % keysize;
                } else {
                    ret[i] = ciphertext[i];
                }
            }
            ret[size] = '\0';
            return ret;
        } else {
            auto ciph0{std::string(ciphertext)};
            auto ciph = clear_string(ciph0);
            LetterAnalyser la{};
            BigramAnalyser ba{};
            std::vector<std::tuple<int, int, double>> len_vec{};
            std::string text{};
            for (int keysize{1}; keysize <= MAX_KEYLEN; keysize++) {
                text.clear();
                for (int i{0}; i < ciph.size(); i += keysize) {
                    text += ciph[i];
                }
                int min_shift{0};
                double min_stat{INFINITY};
                caesarCipher caesar{};
                for (int i{0}; i < 26; i++) {
                    std::string s{caesar.caesar(text, 26 - i)};
                    double stat{la.computeTestStatistic(s)};
                    if (min_stat > stat) {
                        min_shift = i;
                        min_stat = stat;
                    }
                }
                len_vec.push_back(std::make_tuple(keysize, min_shift, min_stat));
            }
            std::sort(len_vec.begin(), len_vec.end(), [](auto& c1, auto& c2) { return 
                std::get<2>(c1) < std::get<2>(c2); });
            std::vector<std::tuple<std::string, double>> key_vec{};
            for (int i{0}; i < KEYLEN_ITERS; i++) {
                std::vector<int> maybeKey{std::get<1>(len_vec[i])};
                for (int j{1}; j < std::get<0>(len_vec[i]); j++) {
                    double min_stat{INFINITY};
                    int key_val{-1};
                    for (int k{0}; k < 26; k++) {
                        text.clear();
                        for (int q{j}; q < ciph.size(); q += std::get<0>(len_vec[i])) {
                            text += mod26(ciph[q - 1] - 'a' - maybeKey[j - 1]) + 'a';
                            text += mod26(ciph[q] - 'a' - k) + 'a';
                            text += ' ';
                        }
                        double stat{ba.computeTestStatistic(text)};
                        if (stat < min_stat) {
                            min_stat = stat;
                            key_val = k;
                        }
                    }
                    maybeKey.push_back(key_val);
                }
                std::string strkey{};
                for (auto& i : maybeKey) {
                    strkey += i + 'a';
                }
                std::string restext{};
                int index{0};
                for (auto& i : ciph0) {
                    if (std::isalpha(i)) {
                        restext += mod26(std::tolower(i) - std::tolower(strkey[index])) + 'a';
                        index = (index + 1) % strkey.size();
                    } else {
                        restext += i;
                    }
                }
                std::cout << "len " << i << ", " << restext << "\n";
                key_vec.push_back(std::make_tuple(strkey, 
                    ba.computeTestStatistic(restext)));
            }
            for (auto& [a, b] : key_vec) {
                std::cout << a << ": " << b << "\n";
            }
            auto& [ret_key, _] = *std::min_element(key_vec.begin(), key_vec.end(), [](auto& c1, auto& c2) { return 
                std::get<1>(c1) < std::get<1>(c2); });
            *key = new char[ret_key.size() + 1];
            for (int i{0}; i < ret_key.size(); i++) {
                (*key)[i] = ret_key[i];
            }
            (*key)[ret_key.size()] = '\0';
            return decrypt(ciphertext, key);
        }
    }
private:
    std::string clear_string(const std::string& s) {
        std::string res{};
        for (auto& i : s) {
            if (isalpha(i)) {
                res += static_cast<char>(std::tolower(i));
            }
        }
        return res;
    }
    inline int mod26(int x) {
        if (x < 0) {
            x += (x/26 + 1)*26;
        }
        return x % 26;
    }
};