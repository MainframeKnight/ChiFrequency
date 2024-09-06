#include<array>
#include<string>
#include<stdexcept>
#include<fstream>
#include<iostream>
#include<queue>
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
        for (long i{1}; i < cleared_str.size(); i++) {
            if (cleared_str[i-1] != ' ' && cleared_str[i] != ' ') {
                sample_bigrams[cleared_str[i-1] - 'a'][cleared_str[i] - 'a']++;
            } else if (cleared_str[i] != ' ') {
                spaces++;
            }
        }
        long n{text.size() - 1 - spaces};
        std::priority_queue<double> space_queue{};
        if (!spaces) {
            spaces = text.size()*10/47; // average number of spaces
        }
        double chi_squared{0};
        for (int i{0}; i < 26; i++) {
            for (int j{0}; j < 26; j++) {
                double val{((sample_bigrams[i][j]/static_cast<double>(n) - bigrams[i][j])/bigrams[i][j])
                    *(sample_bigrams[i][j]/static_cast<double>(n) - bigrams[i][j])};
                if (space_queue.empty() || val > space_queue.top()) {
                    space_queue.push(val);
                }
                chi_squared += val;
            }
        }
        for (int i{0}; i < spaces; i++) {
            chi_squared -= space_queue.top();
            space_queue.pop();
        }
        return chi_squared * n;
    }
    bool validate(const std::string& text) {
        return computeTestStatistic(text) <= 763.41; // critical value for chi-square with 675 DOF and test power of 0.01.
    }
private:
    void clearString(std::string& str) const {
        for (long i{0}; i < str.size(); i++) {
            if (std::string(",.;!?()").find(str[i]) != std::string::npos) {
                str.erase(str.begin() + i);
            } else {
                str[i] = std::tolower(str[i]);
            }
        }
    }
    std::array<std::array<double, 26>, 26> bigrams;
};