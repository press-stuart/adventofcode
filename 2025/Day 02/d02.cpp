#include <iostream>
#include <string>

long long make_invalid(long long part, long long p10, int rep) {
    long long inv = part;
    for (int i = 0; i < rep - 1; i++) {
        inv = inv * p10 + part;
    }
    return inv;
}

// returns indexed form of invalid ID no larger than given ID
long long get_invalid_no_larger(long long id, int rep) {
    std::string idstr = std::to_string(id);
    size_t n = idstr.length();

    long long p10 = 1;
    for (int i = 0; i < n / rep; i++) p10 *= 10LL;

    if (n % rep != 0) {
        return p10 - 1LL;
    }

    long long part = stoll(idstr.substr(0, n / rep));
    if (make_invalid(part, p10, rep) <= id) return part;
    return part - 1LL;
}

long long sum_invalid(int start, int end, int rep) {
    long long total = 0;

    // in each iteration, sum up invalid IDs with index in range:
    // [`start`, `end`] intersect [`p10`, 10 * `p10` - 1]
    for (int p10 = 1; p10 <= end; p10 *= 10) {
        long long part_start = std::max(start, p10);
        long long part_end = std::min(end, 10 * p10 - 1);
        if (part_start > part_end) continue;

        long long multiplier = make_invalid(1LL, 10 * p10, rep);
        long long series = part_end * (part_end + 1) / 2
                            - (part_start - 1) * part_start / 2;
        total += multiplier * series; 
    }

    return total;
}

int main() {
    const int REP_MULTIPLIERS[] = {0, 0, 1, 1, 0, 1, -1, 1, 0, 0, -1}; // expand where necessary
    const int REP_LIMIT = 10;

    std::string input;
    std::cin >> input;
    input = input + ",";

    size_t start_pos = 0, hyphen_pos;
    long long total_p1 = 0;
    long long total_p2 = 0;

    while ((hyphen_pos = input.find('-', start_pos)) != std::string::npos) {
        // extract one range in the input
        std::string start_id_str = input.substr(start_pos, hyphen_pos - start_pos);
        long long start_id = stoll(start_id_str);

        size_t comma_pos = input.find(',', hyphen_pos + 1);
        if (comma_pos == std::string::npos) {
            comma_pos = input.length();
        }

        std::string end_id_str = input.substr(hyphen_pos + 1, comma_pos - (hyphen_pos + 1));
        long long end_id = stoll(end_id_str);

        start_pos = comma_pos + 1;

        // find invalid IDs in range
        for (int rep = 2; rep <= REP_LIMIT; rep++) {
            if (REP_MULTIPLIERS[rep] == 0) continue;

            long long start_invalid = get_invalid_no_larger(start_id - 1, rep) + 1;
            long long end_invalid = get_invalid_no_larger(end_id, rep);
            long long partial = sum_invalid(start_invalid, end_invalid, rep);

            if (rep == 2) total_p1 += partial;
            total_p2 += partial * REP_MULTIPLIERS[rep];
        }
    }

    std::cout << total_p1 << '\n';
    std::cout << total_p2 << '\n';

    return 0;
}