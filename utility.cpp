#pragma once

#include <bits\stdc++.h>
#include <format>

// #include <iostream>
// #include <iomanip>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <string>
// #include <algorithm>
// #include <unordered_set>
// #include <random>
// #include <chrono>
// #include <functional>
// #include <cmath>
// #include <math.h>
// #include <numeric>
// #include <climits>
// #include <numbers>
// #include <limits>

using namespace std;

string root_dir = ".";
string images_path = "./All images";
#define M_PI 3.14159265358979323846
using int_pair = pair<int, int>;

auto seed1 = chrono::system_clock::now().time_since_epoch().count();
auto seed2 = chrono::steady_clock::now().time_since_epoch().count();
auto seed3 = random_device()();
auto seed = seed3;

default_random_engine gen1(seed);
mt19937 gen2(seed);
mt19937_64 gen3(seed);

auto gen = gen3;

uniform_real_distribution<double> random_double(0.0, 1.0);

char black_pixel = '#';
char white_pixel = '.';

double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

vector<string> split(const string &str, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream token_stream(str);
    while (getline(token_stream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

bool on_left(int_pair point, int_pair line_point1, int_pair line_point2)
{
    int_pair a = {point.first - line_point1.first, point.second - line_point1.second};
    int_pair normal = {-line_point2.second + line_point1.second,
                       line_point2.first - line_point1.first};
    return (normal.first * a.first + normal.second * a.second) >= 0;
}

bool on_left_strict(int_pair point, int_pair line_point1,
                    int_pair line_point2)
{
    int_pair a = {point.first - line_point1.first, point.second - line_point1.second};
    int_pair normal = {-line_point2.second + line_point1.second,
                       line_point2.first - line_point1.first};
    return (normal.first * a.first + normal.second * a.second) > 0;
}

vector<double> get_random_point(double a, double b)
{
    uniform_real_distribution<double> random_range(a, b);
    return {random_range(gen), random_range(gen)};
}

int_pair get_random_point(int a, int b)
{
    uniform_int_distribution<int> random_range(a, b);
    return {random_range(gen), random_range(gen)};
}

double get_vector_length(int_pair point)
{
    return sqrt(point.first * point.first + point.second * point.second);
}

double get_point_line_distance(int_pair point, int_pair line_point1,
                               int_pair line_point2)
{
    int_pair a = {point.first - line_point1.first, point.second - line_point1.second};
    int_pair normal = {-line_point2.second + line_point1.second,
                       line_point2.first - line_point1.first};
    return (normal.first * a.first + normal.second * a.second) / get_vector_length(normal);
}

void build_gradient(char *matrix[], int n, function<double(double)> pdf)
{
    uniform_int_distribution<int> random_range(0, n - 1);
    auto line_point1 = make_pair(random_range(gen), random_range(gen));
    auto line_point2 = make_pair(random_range(gen), random_range(gen));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = ((random_double(gen) < pdf(
                                                      get_point_line_distance(make_pair(i, j),
                                                                              line_point1, line_point2)))
                                ? black_pixel
                                : white_pixel);
}

void build_p_image(char *matrix[], int n, double p)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = (random_double(gen) < p ? black_pixel : white_pixel);
}

void build_halfplane(char *matrix[], int n)
{
    build_gradient(matrix, n, [](double dist) -> double
                   { return (dist < 0) ? 0 : 1; });
}

void build_epsilon_close(char *matrix[], int n, double epsilon)
{
    build_halfplane(matrix, n);
    uniform_int_distribution<int> random_range(0, n - 1);
    int cap = epsilon * n * n;
    for (; cap != 0; cap--)
    {
        int x = random_range(gen), y = random_range(gen);
        matrix[x][y] = (matrix[x][y] == black_pixel ? white_pixel : black_pixel);
    }
}

void build_worst_case(char *matrix[], int n, double epsilon, double threshold)
{
    uniform_int_distribution<int> random_range(0, n - 1);
    auto line_point1 = make_pair(random_range(gen), random_range(gen));
    auto line_point2 = make_pair(random_range(gen), random_range(gen));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            matrix[i][j] = (get_point_line_distance(make_pair(i, j), line_point1, line_point2) < 0 ? black_pixel : white_pixel);

    int cap = epsilon * n * n;
    while (cap != 0)
    {
        int x = random_range(gen), y = random_range(gen);
        double dist = get_point_line_distance(make_pair(x, y), {line_point1}, {line_point2});
        if (abs(dist) > threshold)
            continue;
        matrix[x][y] = (matrix[x][y] == black_pixel ? white_pixel : black_pixel);
        cap--;
    }
}

void print_image(char *matrix[], int n)
{
    for (int i = n - 1; i >= 0; i--)
    {
        for (int j = 0; j < n; j++)
            cout << matrix[i][j];
        cout << '\n';
    }
}

void save_image(char *matrix[], int n, int abs_distance)
{ // image -> txt file
    uniform_int_distribution<int> randomId(0, INT_MAX);
    stringstream ss;
    double label = (double)abs_distance / (double)(n * n);
    ss << root_dir + "/All images/"
       << n << "_" << setprecision(numeric_limits<double>::max_digits10) << label << "_" << randomId(gen);
    ofstream outfile(ss.str());
    if (not outfile.is_open())
    {
        cerr << "Error: Unable to open the file." << endl;
        return;
    }
    outfile << n << '\n';
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            outfile << matrix[i][j];
        outfile << '\n';
    }
    outfile.close();
}

template <typename T>
struct point
{
    T x, y;

    point(T x = 0, T y = 0) : x(x), y(y) {}

    point operator+(const point a)
    {
        return point(x + a.x, y + a.y);
    }

    point operator-(const point a)
    {
        return point(x - a.x, y - a.y);
    }

    T operator*(const point a)
    {
        return x * a.y - y * a.x;
    }

    T operator%(const point a)
    {
        return x * a.x + y * a.y;
    }

    int pos()
    {
        return !(y > 0 || (y == 0 && x > 0));
    }
};

template <typename T>
ostream &operator<<(ostream &out, const point<T> &a)
{
    return out << "(" << a.x << ", " << a.y << ")";
}

template <typename T>
istream &operator>>(istream &in, point<T> &a)
{
    return in >> a.x >> a.y;
}

template <typename T>
bool operator<(const point<T> &a, const point<T> &b)
{
    if (a.y == b.y)
        return a.x < b.x;
    return a.y < b.y;
}

template <typename T>
bool operator==(const point<T> &a, const point<T> &b)
{
    return a.x == b.x && a.y == b.y;
}

using pt = point<long long>;
using points = vector<pt>;

void add(points &h, pt &p)
{
    if (h.size() <= 1)
    {
        h.push_back(p);
        return;
    }
    if (h.back() == p)
        return;
    while (h.size() > 1 && (h.end()[-1] - h.end()[-2]) * (p - h.end()[-1]) <= 0)
        h.pop_back();
    h.push_back(p);
    return;
}

points hull(points s)
{
    if (s.size() == 0)
        return s;
    sort(s.begin(), s.end());
    points lower, upper;
    for (auto p : s)
        add(lower, p);
    reverse(s.begin(), s.end());
    for (auto p : s)
        add(upper, p);
    lower.pop_back();
    for (auto p : upper)
        lower.push_back(p);
    lower.pop_back();
    return lower;
}

bool check(pt v, pt u)
{
    if (v.pos() == u.pos())
        return v * u > 0;
    return v.pos() < u.pos();
}

bool intersect(points &A, points &B)
{
    if (A.empty() || B.empty())
        return false;
    points V;
    for (auto &p : B)
    {
        p.x *= -1;
        p.y *= -1;
    }
    rotate(B.begin(), min_element(B.begin(), B.end()), B.end());
    int a = 0, b = 0;
    while (a < (int)A.size() || b < (int)B.size())
    {
        pt v = (A[(a + 1) % A.size()] - A[(a) % A.size()]), u = (B[(b + 1) % B.size()] - B[(b) % B.size()]);
        if (a == (int)A.size())
        {
            V.push_back(u);
            b++;
            continue;
        }
        if (b == (int)B.size())
        {
            V.push_back(v);
            a++;
            continue;
        }
        if (check(v, u))
        {
            V.push_back(v);
            a++;
        }
        else
        {
            V.push_back(u);
            b++;
        }
    }
    pt mn = A[0] + B[0];
    for (auto p : V)
    {
        if (mn * p < 0)
            return false;
        mn = mn + p;
    }
    return true;
}

bool property_test(char *matrix[], int n, double epsilon)
{
    int sample_count = ceil(1 / epsilon);

    points W, B;

    uniform_int_distribution<int> rand_coord(0, n - 1);
    for (int i = 0; i < sample_count; ++i)
    {
        int x = rand_coord(gen), y = rand_coord(gen);
        if (matrix[x][y] == white_pixel)
            W.push_back(pt(x, y));
        else
            B.push_back(pt(x, y));
    }
    // for (int x = 0; x < n; x++)
    // {
    //     for (int y = 0; y < n; y++)
    //     {
    //         if (find(W.begin(), W.end(), pt(x, y)) != W.end())
    //             cout << white_pixel;
    //         else if (find(B.begin(), B.end(), pt(x, y)) != B.end())
    //             cout << black_pixel;
    //         else
    //             cout << " ";
    //     }
    //     cout << endl;
    // }
    W = hull(W);
    B = hull(B);
    return not intersect(W, B);
}

int_pair get_normal(int_pair p1, int_pair p2)
{
    if (p1 == p2)
        return make_pair(1, 1);
    p2.first -= p1.first;
    p2.second -= p1.second;
    int d = gcd(abs(p2.first), abs(p2.second));
    return {-p2.second / d, p2.first / d};
}

struct point_pair_hasher
{
    size_t operator()(const pair<int_pair, int_pair> &point_pair) const
    {
        hash<int> h;
        return (h(point_pair.first.first) << 48) ^ (h(point_pair.first.second) << 32) ^ (h(point_pair.second.first) << 16) ^ h(point_pair.second.second);
    }
};

bool elizondo(char *matrix[], int n)
{                                    // checks if an image is halfplane
    int_pair a = {0, 0}, b = {0, 0}; // a - black, b - white
    int black_count = 0, white_count;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (matrix[i][j] == black_pixel)
            {
                black_count++;
                a.first += i;
                a.second += j;
            }
            else
            {
                b.first += i;
                b.second += j;
            }
        }
    }
    if (black_count == 0 or black_count == n * n)
        return true;
    white_count = n * n - black_count;

    a.first /= black_count; // gravity centers
    a.second /= black_count;
    b.first /= white_count;
    b.second /= white_count;

    // cout << format("({}, {}) ({}, {})\n", a.first, a.second, b.first, b.second);
    int_pair normal = get_normal(a, b);
    // cout << "Start" << endl;
    unordered_set<pair<int_pair, int_pair>, point_pair_hasher> visited_pairs;
    // int iter = 0;
    while (true)
    {
        // cout << iter << endl;
        // iter++;
        if (visited_pairs.count(make_pair(a, b)))
            return false;
        visited_pairs.insert(make_pair(a, b));

        int max_val = INT_MIN, min_val = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int product = normal.first * i + normal.second * j;
                if (matrix[i][j] == black_pixel)
                {
                    if (product <= min_val)
                    {
                        min_val = product;
                        a.first = i;
                        a.second = j;
                    }
                }
                else
                {
                    if (product >= max_val)
                    {
                        max_val = product;
                        b.first = i;
                        b.second = j;
                    }
                }
            }
        }
        if (max_val <= min_val)
            break;
        normal = get_normal(a, b);
    }
    return true;
}

int epsilon_distance_for_line(char *matrix[], int n, int_pair line_point1,
                              int_pair line_point2, int ans)
{
    int black_left = 0, white_left = 0, black_right = 0, white_right = 0;
    for (int x = 0; x < n; x++)
    {
        for (int y = 0; y < n; y++)
        {
            bool left = on_left({x, y}, line_point1, line_point2);
            if (left)
            {
                if (matrix[x][y] == black_pixel)
                    black_left++;
                else
                    white_left++;
            }
            else
            {
                if (matrix[x][y] == black_pixel)
                    black_right++;
                else
                    white_right++;
            }
            if (min(white_left + black_right, black_left + white_right) >= ans)
                return ans;
        }
    }
    return min(white_left + black_right, black_left + white_right);
}

int epsilon_distance(char *matrix[], const int n) // Calculate epsilon distane of an image to being a halfplane
{
    int black_total = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (matrix[i][j] == black_pixel)
                black_total++;
        }
    }
    int white_total = n * n - black_total;
    vector<unsigned short int> white_counts(3 * n * n, 0);
    vector<unsigned short int> black_counts(3 * n * n, 0);
    int dist = n * n;
    for (int ny = 0; ny < n; ny++)
    {
        for (int nx = -(n - 1); nx < n; nx++)
        {
            if (gcd(ny, abs(nx)) != 1)
                continue;
            for (int y = 0; y < n; y++)
            {
                for (int x = 0; x < n; x++)
                {
                    int dot_product = ny * y + nx * x + n * n;
                    if (matrix[y][x] == black_pixel)
                        black_counts[dot_product]++;
                    else
                        white_counts[dot_product]++;
                }
            }

            int pre_black = 0, pre_white = 0;
            for (int prod = 0; prod < 3 * n * n; prod++)
            {
                pre_black += black_counts[prod];
                pre_white += white_counts[prod];
                dist = min({dist, pre_black + white_total - pre_white, pre_white + black_total - pre_black});
                black_counts[prod] = 0;
                white_counts[prod] = 0;
            }
        }
    }
    return dist;
}

int epsilon_distance_steps(char *matrix[], const int n) // Calculate epsilon distane and output each step
{
    int black_total = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (matrix[i][j] == black_pixel)
                black_total++;
    int white_total = n * n - black_total;
    vector<unsigned short int> white_counts(3 * n * n, 0);
    vector<unsigned short int> black_counts(3 * n * n, 0);
    vector<pair<double, int>> table;
    for (int ny = 0; ny < n; ny++)
    {
        for (int nx = -(n - 1); nx < n; nx++)
        {
            if (gcd(ny, abs(nx)) != 1)
                continue;

            for (int y = 0; y < n; y++)
            {
                for (int x = 0; x < n; x++)
                {
                    int dot_product = ny * y + nx * x + n * n;
                    if (matrix[y][x] == black_pixel)
                        black_counts[dot_product]++;
                    else
                        white_counts[dot_product]++;
                }
            }

            int pre_black = 0, pre_white = 0;
            int curr_min = n * n;
            for (int prod = 0; prod < 3 * n * n; prod++)
            {
                pre_black += black_counts[prod];
                pre_white += white_counts[prod];
                // dist = min({dist, pre_black + white_total - pre_white, pre_white + black_total - pre_black});
                curr_min = min({curr_min, pre_black + white_total - pre_white, pre_white + black_total - pre_black});
                black_counts[prod] = 0;
                white_counts[prod] = 0;
            }
            table.push_back({atan2(ny, nx), curr_min});
        }
    }
    int dist = n * n;
    double best_angle;
    sort(table.begin(), table.end());
    for (auto &elem : table)
    {
        cout << elem.first * 180 / M_PI << " " << elem.second << endl;
        if (elem.second <= dist)
        {
            dist = elem.second;
            best_angle = elem.first;
        }
    }
    return best_angle * 180 / M_PI;
}

double approximate_distance(char *matrix[], int n, double error, double shift = 0) // Approximate epsilon distane of an image to being a halfplane
{
    if (shift == 0)
        shift = error;
    uniform_int_distribution<int> rand_coord(0, n - 1);
    int sample_size = ceil(6 / (error * error) * log(7 / error));
    int_pair sample[sample_size];
    int black_total = 0;
    for (int i = 0; i < sample_size; i++)
    {
        int x = rand_coord(gen), y = rand_coord(gen);
        sample[i] = make_pair(x, y);
        if (matrix[x][y] == black_pixel)
            black_total++;
    }
    int white_total = sample_size - black_total;

    double delta_angle = shift, delta_thresh = shift * n / sqrt(2);
    int bucket_count = 2 * (2 / shift + 1);
    int_pair buckets[bucket_count]; // first = black count, second = white count
    for (auto &count : buckets)
        count = make_pair(0, 0);
    double min_distance = 0.5;
    for (double angle = 0; angle < 2 * M_PI; angle += delta_angle)
    {
        for (auto pnt : sample)
        {
            double prod = pnt.first * cos(angle) + pnt.second * sin(angle) + n * sqrt(2);
            int bucket = prod / delta_thresh;
            if (matrix[pnt.first][pnt.second] == black_pixel)
                buckets[bucket].first++;
            else
                buckets[bucket].second++;
        }

        int pre_black = 0, pre_white = 0;
        for (int bucket = 0; bucket < bucket_count; bucket++)
        {
            pre_black += buckets[bucket].first;
            pre_white += buckets[bucket].second;
            min_distance = min(min_distance, (pre_black + white_total - pre_white) / ((double)sample_size));
            buckets[bucket].first = 0;
            buckets[bucket].second = 0;
        }
    }
    return min_distance;
}

int elizondo_approximate(char *matrix[], int n)
{                                    // approximate image's epsilon_distance
    int_pair a = {0, 0}, b = {0, 0}; // a - black, b - white
    int black_count = 0, white_count;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (matrix[i][j] == black_pixel)
            {
                black_count++;
                a.first += i;
                a.second += j;
            }
            else
            {
                b.first += i;
                b.second += j;
            }
        }
    }
    white_count = n * n - black_count;

    a.first /= black_count; // gravity centers
    a.second /= black_count;
    b.first /= white_count;
    b.second /= white_count;

    int_pair normal = get_normal(a, b);
    int_pair bucket_normal = normal;
    if (bucket_normal.second < 0)
    {
        bucket_normal.first = -bucket_normal.first;
        bucket_normal.second = -bucket_normal.second;
    }
    unordered_set<pair<int_pair, int_pair>, point_pair_hasher> visited_pairs;
    vector<unsigned short int> white_counts(3 * n * n, 0);
    vector<unsigned short int> black_counts(3 * n * n, 0);
    int dist = n * n;
    while (true)
    {
        // cout << dist << endl;
        if (visited_pairs.count(make_pair(a, b)))
            break;
        visited_pairs.insert(make_pair(a, b));

        int max_val = INT_MIN, min_val = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                int product = normal.first * i + normal.second * j;
                int bucket = bucket_normal.first * i + bucket_normal.second * j + n * n;
                if (bucket < 0)
                {
                    cout << bucket << endl;
                    return -1;
                }
                if (matrix[i][j] == black_pixel)
                {
                    if (product <= min_val)
                    {
                        min_val = product;
                        a.first = i;
                        a.second = j;
                    }
                    black_counts[bucket]++;
                }
                else
                {
                    if (product >= max_val)
                    {
                        max_val = product;
                        b.first = i;
                        b.second = j;
                    }
                    white_counts[bucket]++;
                }
            }
        }
        int pre_black = 0, pre_white = 0;
        for (int prod = 0; prod < 3 * n * n; prod++)
        {
            pre_black += black_counts[prod];
            pre_white += white_counts[prod];
            dist = min({dist, pre_black + white_count - pre_white, pre_white + black_count - pre_black});
            black_counts[prod] = 0;
            white_counts[prod] = 0;
        }
        if (max_val <= min_val)
            break;
        normal = get_normal(a, b);
        bucket_normal = normal;
        if (bucket_normal.second < 0)
        {
            bucket_normal.first = -bucket_normal.first;
            bucket_normal.second = -bucket_normal.second;
        }
    }
    return dist;
}