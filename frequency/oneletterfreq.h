#include<vector>
#include<string>
#include<stdexcept>
#include<iostream>
#include<fstream>
class FrequencyAnalyser {
public:
    FrequencyAnalyser(const std::string& filename) : freq{} {
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
    bool validate(const std::string& text) const {
        std::vector<long> freq_sample{};
        for (int i{0}; i < 26; i++) {
            freq_sample.emplace(freq_sample.begin(), 0);
        }
        for (auto& i : text) {
            char l{std::tolower(i)};
            if (l < 'a' || l > 'z') {
                throw std::invalid_argument{"Non-letter symbols in the text."};
            }
            freq_sample[l - 'a']++;
        }
        long n{0};
        for (long i : freq_sample) {
            n += i;
        }
        double chi_squared{0};
        for (int i{0}; i < 26; i++) {
            chi_squared += (freq_sample[i] - n*freq[i])*(freq_sample[i] - n*freq[i])/(n * freq[i]);
        }
        return chi_squared <= 44.3141; // the test statistic is not greater than the critical value
    }
private:
    std::vector<double> freq;
};