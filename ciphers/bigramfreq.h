#pragma once
#include<array>
#include<string>
#include<stdexcept>
#include<fstream>
#include<iostream>
#include<vector>
class BigramAnalyser {
public:
    BigramAnalyser(const std::string& filename = "bigrams.txt") : bigrams{} {
        double input{};
        std::string input_bigram{};
        std::ifstream file{filename};
        if (file.fail()) {
            throw std::invalid_argument{"File could not be opened."};
        }
        for (int i{0}; i < 676; i++) {
            file >> input_bigram >> input;
            ordered_bigrams[i] = std::make_pair(input_bigram[0] - 'a', input_bigram[1] - 'a');
            if (input < 10e-12) {
                bigrams[input_bigram[0] - 'a'][input_bigram[1] - 'a'] = 10e-10;
            } else {
                bigrams[input_bigram[0] - 'a'][input_bigram[1] - 'a'] = input;
            }
        }
        file.close();
    }
    double computeTestStatistic(const std::string& text) const {
        long spaces{0};
        std::string cleared_str{text};
        std::array<std::array<long, 26>, 26> sample_bigrams;
        for (int i{0}; i < 26; i++) {
            for (int j{0}; j < 26; j++) {
                sample_bigrams[i][j] = 0;
            }
        }
        clearString(cleared_str);
        if (!cleared_str.size()) {
            throw std::invalid_argument{"Input string too short."};
        }
        long n{0};
        for (long i{1}; i < cleared_str.size(); i++) {
            if (isalpha(cleared_str[i-1]) && isalpha(cleared_str[i])) {
                n++;
                sample_bigrams[cleared_str[i-1] - 'a'][cleared_str[i] - 'a']++;
            } else if (cleared_str[i] == ' ' && cleared_str[i-1] != ' ') {
                spaces++;
            }
        }
        if (!spaces) {
            spaces = text.size()*10/47; // average number of spaces
            for (int i{675}; i >= 0 && spaces > 0; i--) {
                auto& [fst, snd] = ordered_bigrams[i];
                while (sample_bigrams[fst][snd] > 0 && spaces > 0) {
                    spaces--;
                    sample_bigrams[fst][snd]--;
                    n--;
                }
            }
        }
        double chi_squared{0};
        for (int i{0}; i < 26; i++) {
            for (int j{0}; j < 26; j++) {
                double val{((sample_bigrams[i][j]/static_cast<double>(n) - bigrams[i][j])/bigrams[i][j])
                    *(sample_bigrams[i][j]/static_cast<double>(n) - bigrams[i][j])};
                chi_squared += val;
            }
        }
        return chi_squared * n;
    }
    bool validate(const std::string& text) {
        return computeTestStatistic(text) <= 763.41; // critical value for chi-square with 675 DOF and test power of 0.01.
    }
private:
    void clearString(std::string& str) const {
        for (long i{0}; i < str.size(); i++) {
            if (!isalpha(str[i])) {
                str[i] = ' ';
            } else {
                str[i] = std::tolower(str[i]);
            }
        }
    }
    std::array<std::pair<int, int>, 676> ordered_bigrams;
    std::array<std::array<double, 26>, 26> bigrams;
};

// For index of coincidence first iteration in Viginere cipher.
class LetterAnalyser {
    public:
        LetterAnalyser(const std::string& filename = "letter.txt") : freq{} {
            double input;
            std::ifstream file{filename};
            if (file.fail()) {
                throw std::invalid_argument{"File could not be opened."};
            }
            for (int i{0}; i < 26; i++) {
                file >> input;
                freq.emplace_back(input);
            }
            file.close();
        }
        double computeTestStatistic(const std::string& text) const {
            std::vector<long> freq_sample{};
            for (int i{0}; i < 26; i++) {
                freq_sample.emplace(freq_sample.begin(), 0);
            }
            long n{0};
            for (auto& i : text) {
                char l{static_cast<char>(std::tolower(i))};
                if (l < 'a' || l > 'z') {
                    continue;
                }
                n++;
                freq_sample[l - 'a']++;
            }
            double chi_squared{0};
            for (int i{0}; i < 26; i++) {
                chi_squared += (freq_sample[i] - n*freq[i])*(freq_sample[i] - n*freq[i])/(n * freq[i]);
            }
            return chi_squared;
        }
    private:
        std::vector<double> freq;
    };