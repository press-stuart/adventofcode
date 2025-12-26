#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Point {
    int x, y;

    Point(int _x, int _y) : x(_x), y(_y) {}
};

std::vector<std::string> split_by_delimiter(std::string str, char delim) {
    std::vector<std::string> words;
    std::istringstream iss(str);
    std::string word;

    while (std::getline(iss, word, delim)) {
        words.push_back(word);
    }

    return words;
}

template <typename T, typename U>
void sorted_order_to_value(std::map<T, U> &m) {
    typename std::map<T, U>::iterator it = m.begin();
    U u = 0;

    while (it != m.end()) {
        it->second = u;
        it++;
        u++;
    }
}

// 0: left; 1: down; 2: right; 3: up
int get_edge_dir(Point u, Point v) {
    if (u.y == v.y) {
        if (u.x < v.x) return 2;
        return 0;
    }
    if (u.y < v.y) return 3;
    return 1;
}

bool is_anticlockwise_turn(Point u, Point v, Point w) {
    int d1 = get_edge_dir(u, v);
    int d2 = get_edge_dir(v, w);
    return (d1 + 1) % 4 == d2;
}

int main() {
    std::string input_line;
    std::vector<Point> points;

    while (std::getline(std::cin, input_line)) {
        std::vector<std::string> numbers = split_by_delimiter(input_line, ',');
        Point point(stoi(numbers[0]), stoi(numbers[1]));
        points.push_back(point);
    }

    int num_points = points.size();

    /* reorder points */
    // the first 2 points should have the smallest x-coordinate and decreasing
    // y-coordinate (leftmost edge, anticlockwise in plot view)
    int min_x = INT_MAX;
    int start_id = -1;
    int multiplier = 1; // 1: original order; -1: reverse order
    std::vector<Point> temp_points;

    for (int i = 0; i < num_points; i++) {
        int next_i = (i + 1) % num_points;
        if (points[i].x == points[next_i].x && points[i].x < min_x) {
            min_x = points[i].x;
            start_id = i;
        }
    }

    int next_i = (start_id + 1) % num_points;
    if (points[start_id].y < points[next_i].y) {
        start_id = next_i;
        multiplier = -1;
    }

    for (int i = 0; i < num_points; i++) {
        int id = (start_id + i * multiplier + num_points) % num_points;
        temp_points.push_back(points[id]);
    }

    points.swap(temp_points);

    /* determine key coordinate values and discretize grid */
    // ASSUMPTION: no red/green tiles with negative coordinates so that the grid starts at
    // coordinates (-1, -1) (otherwise, this is easily fixed by changing -1 to minimum
    // coordinate value - 1)
    std::map<int, int> impt_x, impt_y;
    impt_x[-1] = 0;
    impt_y[-1] = 0;

    for (int i = 0; i < num_points; i++) {
        impt_x[points[i].x] = 0;
        impt_x[points[i].x + 1] = 0;
        impt_y[points[i].y] = 0;
        impt_y[points[i].y + 1] = 0;
    }

    sorted_order_to_value(impt_x);
    sorted_order_to_value(impt_y);

    int size_x = impt_x.size();
    int size_y = impt_y.size();

    // translate all coordinates to important coordinates now and memoize
    std::vector<Point> impt_points;
    for (int i = 0; i < num_points; i++) {
        Point point(impt_x[points[i].x], impt_y[points[i].y]);
        impt_points.push_back(point);
    }

    /* identify all boundaries */
    bool boundaries_x[size_x][size_y];
    bool boundaries_y[size_x][size_y];
    int boundary_dir = 0; // 0: left; 1: down; 2: right; 3: up

    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            boundaries_x[x][y] = false;
            boundaries_y[x][y] = false;
        }
    }

    // add boundaries along the edge between i and i+1 (mod), and
    // on the corner of i+1 (mod)
    for (int i = 0; i < num_points; i++) {
        Point curr = impt_points[i];
        Point succ = impt_points[(i + 1) % num_points];
        Point suc2 = impt_points[(i + 2) % num_points];

        int x1 = curr.x;
        int y1 = curr.y;
        int x2 = succ.x;
        int y2 = succ.y;

        bool ccw = is_anticlockwise_turn(curr, succ, suc2);
        int next_boundary_dir = 
            (boundary_dir + 4 + (ccw ? 1 : -1)) % 4;
        
        int offset1 = (boundary_dir & 2) ? 1 : 0;
        int offset2 = (next_boundary_dir & 2) ? 1 : 0;

        if (boundary_dir & 1) { // horizontal edge
            // boundaries along edge
            int xmin = std::min(x1, x2);
            int xmax = std::max(x1, x2);

            for (int x = xmin + 1; x <= xmax - 1; x++) {
                boundaries_y[x][y2 + offset1] = !boundaries_y[x][y2 + offset1];
            }

            // boundaries on corner
            if (ccw) {
                boundaries_y[x2][y2 + offset1] = !boundaries_y[x2][y2 + offset1];
                boundaries_x[x2 + offset2][y2] = !boundaries_x[x2 + offset2][y2];
            }
        }
        
        else { // vertical edge
            // boundaries along edge
            int ymin = std::min(y1, y2);
            int ymax = std::max(y1, y2);

            for (int y = ymin + 1; y <= ymax - 1; y++) {
                boundaries_x[x2 + offset1][y] = !boundaries_x[x2 + offset1][y];
            }

            // boundaries on corner
            if (ccw) {
                boundaries_x[x2 + offset1][y2] = !boundaries_x[x2 + offset1][y2];
                boundaries_y[x2][y2 + offset2] = !boundaries_y[x2][y2 + offset2];
            }
        }

        boundary_dir = next_boundary_dir;
    }

    /* mark valid regions in discretized grid */
    bool valid_x[size_x][size_y], valid_y[size_x][size_y], valid[size_x][size_y];

    // y-aligned borders
    for (int x = 0; x < size_x; x++) {
        bool parity = false;

        for (int y = 0; y < size_y; y++) {
            if (boundaries_y[x][y]) parity = !parity;
            valid_y[x][y] = parity;
        }
    }

    // x-aligned borders
    for (int y = 0; y < size_y; y++) {
        bool parity = false;

        for (int x = 0; x < size_x; x++) {
            if (boundaries_x[x][y]) parity = !parity;
            valid_x[x][y] = parity;
        }
    }

    // combine conditions
    for (int x = 0; x < size_x; x++) {
        for (int y = 0; y < size_y; y++) {
            valid[x][y] = valid_x[x][y] && valid_y[x][y];
        }
    }

    /* speed up using prefix sums */
    // add padding for ease of code: valid_psum[1 + size_x][1 + size_y]
    // (unfortunately less readable)
    int** valid_psum = new int*[1 + size_x];
    for (int x = 0; x < 1 + size_x; x++) {
        valid_psum[x] = new int[1 + size_y];
    }

    for (int x = 0; x < 1 + size_x; x++) {
        for (int y = 0; y < 1 + size_y; y++) {
            if (x == 0 || y == 0) {
                valid_psum[x][y] = 0;
                continue;
            }

            valid_psum[x][y] = valid_psum[x][y-1] + valid_psum[x-1][y]
                - valid_psum[x-1][y-1] + (valid[x-1][y-1] ? 1 : 0);
        }
    }

    /* test all pairs of red points */
    long long ans_p1 = 0;
    long long ans_p2 = 0;

    for (int u = 0; u < num_points; u++) {
        for (int v = u + 1; v < num_points; v++) {
            long long dx = 1 + abs(points[u].x - points[v].x);
            long long dy = 1 + abs(points[u].y - points[v].y);
            long long area = dx * dy;
            ans_p1 = std::max(ans_p1, area);

            int xmin = std::min(impt_points[u].x, impt_points[v].x);
            int xmax = std::max(impt_points[u].x, impt_points[v].x);
            int ymin = std::min(impt_points[u].y, impt_points[v].y);
            int ymax = std::max(impt_points[u].y, impt_points[v].y);

            // sum over valid[xmin..xmax][ymin..ymax]
            int valid_sum = valid_psum[xmax + 1][ymax + 1] - valid_psum[xmax + 1][ymin]
                - valid_psum[xmin][ymax + 1] + valid_psum[xmin][ymin];

            if (valid_sum == (xmax - xmin + 1) * (ymax - ymin + 1)) {
                ans_p2 = std::max(ans_p2, area);
            }
        }
    }

    for (int x = 0; x < 1 + size_x; x++) {
        delete[] valid_psum[x];
    }
    delete[] valid_psum;

    std::cout << ans_p1 << '\n';
    std::cout << ans_p2 << '\n';

    return 0;
}