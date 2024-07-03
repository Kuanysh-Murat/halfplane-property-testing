#include <bits/stdc++.h>
using namespace std;

auto seed1 = std::chrono::system_clock::now().time_since_epoch().count();
auto seed2 = chrono::steady_clock::now().time_since_epoch().count();
auto seed3 = random_device()();
auto seed = seed3;

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
    {
        return;
    }
    while (h.size() > 1 && (h.end()[-1] - h.end()[-2]) * (p - h.end()[-1]) <= 0)
    {
        h.pop_back();
    }
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
    {
        add(lower, p);
    }
    reverse(s.begin(), s.end());
    for (auto p : s)
    {
        add(upper, p);
    }
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
    /*
    for (int i = 0; i < A.size(); ++i) {
        V.push_back(A[(i + 1) % A.size()] - A[i]);
    }
    for (int i = 0; i < B.size(); ++i) {
        V.push_back(B[(i + 1) % B.size()] - B[i]);
    }
    sort(V.begin(), V.end(), check);
    */
    while (a < A.size() || b < B.size())
    {
        // break;
        pt v = (A[(a + 1) % A.size()] - A[(a) % A.size()]), u = (B[(b + 1) % B.size()] - B[(b) % B.size()]);
        if (a == A.size())
        {
            V.push_back(u);
            b++;
            continue;
        }
        if (b == B.size())
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

int main(int argc, char *argv[])
{
    freopen(argv[1], "r", stdin);
    double eps = stod(argv[2]);
    int n;
    cin >> n;
    char pic[n][n];
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            cin >> pic[i][j];
        }
    }

    int m = ceil(18 / eps);
    mt19937_64 rng(seed);
    uniform_int_distribution<> get(0, n - 1);

    int accept_count = 0;

    int tests = 10000;

    for (int i = 0; i < tests; i++)
    {
        points W, B;
        for (int j = 0; j < m; ++j)
        {
            int x = get(rng), y = get(rng);
            if (pic[x][y] == '#')
            {
                W.push_back(pt(x, y));
            }
            else
            {
                B.push_back(pt(x, y));
            }
        }
        W = hull(W);
        B = hull(B);
        if (not intersect(W, B))
        {
            accept_count++;
        }
    }

    cout << 100.0 * ((double)accept_count) / tests << "%" << endl;
}
