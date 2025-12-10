#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split_by_delimiter(std::string str, char delim) {
    std::vector<std::string> words;
    std::istringstream iss(str);
    std::string word;

    while (std::getline(iss, word, delim)) {
        words.push_back(word);
    }

    return words;
}

int main() {
    int ans_p1 = 0;
    std::string input_line;

    while (std::getline(std::cin, input_line)) {
        std::vector<std::string> words = split_by_delimiter(input_line, ' ');

        // convert indicator lights to integer representation
        int lights = 0;
        for (int b = 0; b < (int)words[0].size() - 2; b++) {
            char ch = words[0][b+1];
            if (ch == '#') lights |= 1 << b;
        }

        // convert toggles to integer representation
        std::vector<int> toggles;
        for (int i = 1; i < (int)words.size() - 1; i++) {
            std::vector<std::string> splits = split_by_delimiter(
                words[i].substr(1, words[i].size() - 2), // remove brackets
                ','
            );

            int toggle = 0;
            for (std::string s : splits) {
                toggle |= 1 << stoi(s);
            }

            toggles.push_back(toggle);
        }

        // bitmask brute-force
        int toggle_options = toggles.size();
        int min_toggles = toggles.size();

        for (int i = 0; i < (1 << toggle_options); i++) {
            int xorsum = 0, used = 0;

            for (int b = 0; b < toggle_options; b++) {
                if (i & (1 << b)) {
                    xorsum ^= toggles[b];
                    used++;
                }
            }

            if (xorsum == lights) {
                min_toggles = std::min(min_toggles, used);
            }
        }

        ans_p1 += min_toggles;
    }

    std::cout << ans_p1 << '\n';

    return 0;
}